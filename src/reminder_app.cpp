#include "reminder_app.h"
#include "reminder_popup_window.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <libayatana-appindicator/app-indicator.h>

ReminderApp::ReminderApp(bool start_minimized) : m_main_box(Gtk::ORIENTATION_VERTICAL, 10),
                                                 m_input_box(Gtk::ORIENTATION_HORIZONTAL, 5),
                                                 m_thread_running(true),
                                                 m_start_minimized(start_minimized)
{
    // Initialize libnotify
    notify_init("ReminderApp");

    // Create the popup window
    m_popup_window = std::make_unique<ReminderPopupWindow>();

    // Preemptively make sure no windows are visible at startup
    // This ensures nothing flashes on screen while initializing
    hide_window();
    if (m_popup_window)
    {
        m_popup_window->hide();
    }

    // Set up the UI components
    setup_ui();

    // Connect signal handlers
    connect_signals();

    // Initialize database
    initialize_database();

    // Load existing reminders
    load_reminders();

    // Create system tray icon
    create_tray_icon();

    // Start notification thread
    start_notification_thread();

    // The window should already be hidden at this point,
    // m_start_minimized is kept for potential future use
}

ReminderApp::~ReminderApp()
{
    // Stop notification thread
    m_thread_running = false;
    if (m_notification_thread.joinable())
    {
        m_notification_thread.join();
    }

    // Make sure the popup window is closed
    if (m_popup_window)
    {
        m_popup_window->hide();
    }

    // Remove the system tray icon
    if (m_status_icon)
    {
        m_status_icon->set_visible(false);
        m_status_icon.reset();
    }

    // Close database
    if (m_db)
    {
        sqlite3_close(m_db);
    }

    // Uninitialize libnotify
    notify_uninit();
}

Gtk::Window &ReminderApp::get_window()
{
    return m_window;
}

void ReminderApp::setup_ui()
{
    // Window setup
    m_window.set_title("Reminder App");
    m_window.set_default_size(500, 400);
    m_window.set_border_width(10);

    // Hide the window initially - will only be shown when explicitly requested
    m_window.set_visible(false);

    // Add the main box to the window
    m_window.add(m_main_box);

    // Create a frame for input fields with title
    m_input_frame.set_label("New Reminder");
    m_input_frame.set_label_align(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
    m_input_frame.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    m_input_frame.set_border_width(5);

    // Create a box for the input frame content
    auto input_frame_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 10));
    input_frame_box->set_border_width(10);
    m_input_frame.add(*input_frame_box);

    // Setup title input with label
    auto title_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    auto title_label = Gtk::manage(new Gtk::Label("Title:"));
    title_label->set_width_chars(8);
    title_box->pack_start(*title_label, Gtk::PACK_SHRINK);

    m_title_entry.set_placeholder_text("Enter reminder title");
    m_title_entry.set_activates_default(true);
    title_box->pack_start(m_title_entry, Gtk::PACK_EXPAND_WIDGET);

    // Setup time selector with label
    auto time_label_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    auto time_label = Gtk::manage(new Gtk::Label("Time:"));
    time_label->set_width_chars(8);
    time_label_box->pack_start(*time_label, Gtk::PACK_SHRINK);

    // Create time box (HH:MM)
    m_time_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    m_time_box.set_spacing(5);

    // Populate hour combo (1-12 for 12-hour format)
    m_hour_combo.set_tooltip_text("Hour");
    for (int i = 1; i <= 12; i++)
    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << i;
        m_hour_combo.append(ss.str());
    }

    // Time separator (:)
    m_time_separator.set_text(":");

    // Populate minute combo (00-59)
    m_minute_combo.set_tooltip_text("Minute");
    for (int i = 0; i < 60; i++)
    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << i;
        m_minute_combo.append(ss.str());
    }

    // Add AM/PM selector
    m_ampm_combo.set_tooltip_text("AM/PM");
    m_ampm_combo.append("AM");
    m_ampm_combo.append("PM");

    // Set default time to current system time
    std::string current_time = get_current_time();
    int hour24 = std::stoi(current_time.substr(0, 2));
    std::string minute_str = current_time.substr(3, 2);

    // Convert to 12-hour format
    int hour12 = hour24;
    if (hour24 > 12)
    {
        hour12 = hour24 - 12;
        m_ampm_combo.set_active(1); // PM
    }
    else if (hour24 == 12)
    {
        m_ampm_combo.set_active(1); // PM
    }
    else if (hour24 == 0)
    {
        hour12 = 12;
        m_ampm_combo.set_active(0); // AM
    }
    else
    {
        m_ampm_combo.set_active(0); // AM
    }

    // Set active selections
    std::stringstream hour_ss;
    hour_ss << std::setw(2) << std::setfill('0') << hour12;
    m_hour_combo.set_active_text(hour_ss.str());
    m_minute_combo.set_active_text(minute_str);

    // Set specific width for better UX
    m_hour_combo.property_width_request() = 60;
    m_minute_combo.property_width_request() = 60;
    m_ampm_combo.property_width_request() = 60;

    // Add widgets to time box with smaller spacing
    m_time_box.pack_start(m_hour_combo, Gtk::PACK_SHRINK);
    m_time_box.pack_start(m_time_separator, Gtk::PACK_SHRINK);
    m_time_box.pack_start(m_minute_combo, Gtk::PACK_SHRINK);
    m_time_box.pack_start(m_ampm_combo, Gtk::PACK_SHRINK);

    // Set fixed width for the time box to prevent it from extending too far
    m_time_box.property_width_request() = 200;
    m_time_box.set_halign(Gtk::ALIGN_START);

    time_label_box->pack_start(m_time_box, Gtk::PACK_SHRINK);

    // Description label and text view
    auto desc_label = Gtk::manage(new Gtk::Label("Description:"));
    desc_label->set_halign(Gtk::ALIGN_START);

    m_description_textview.set_size_request(-1, 60);
    auto desc_scroll = Gtk::manage(new Gtk::ScrolledWindow());
    desc_scroll->add(m_description_textview);
    desc_scroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    desc_scroll->set_shadow_type(Gtk::SHADOW_IN);

    // Setup add button
    m_add_button.set_label("Add Reminder");
    m_add_button.set_size_request(120, -1);
    m_add_button.set_halign(Gtk::ALIGN_END);
    m_add_button.set_can_default(true);
    m_window.set_default(m_add_button);

    // Add all elements to the input frame box
    input_frame_box->pack_start(*title_box, Gtk::PACK_SHRINK);
    input_frame_box->pack_start(*time_label_box, Gtk::PACK_SHRINK);
    input_frame_box->pack_start(*desc_label, Gtk::PACK_SHRINK);
    input_frame_box->pack_start(*desc_scroll, Gtk::PACK_EXPAND_WIDGET);
    input_frame_box->pack_start(m_add_button, Gtk::PACK_SHRINK);

    // Add input frame to main box
    m_main_box.pack_start(m_input_frame, Gtk::PACK_SHRINK);

    // Add a separator
    auto separator = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL));
    separator->set_margin_top(10);
    separator->set_margin_bottom(10);
    m_main_box.pack_start(*separator, Gtk::PACK_SHRINK);

    // Add a label for the reminders list
    auto reminders_label = Gtk::manage(new Gtk::Label("<b>Your Reminders</b>"));
    reminders_label->set_use_markup(true);
    reminders_label->set_halign(Gtk::ALIGN_START);
    m_main_box.pack_start(*reminders_label, Gtk::PACK_SHRINK);

    // Setup list with scrolling
    m_list_box.set_selection_mode(Gtk::SELECTION_SINGLE);
    m_scrolled_window.add(m_list_box);
    m_scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_scrolled_window.set_min_content_height(200);
    m_scrolled_window.set_shadow_type(Gtk::SHADOW_IN);

    // Add list to main box
    m_main_box.pack_start(m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

    // Show all widgets
    m_window.show_all_children();
}

void ReminderApp::connect_signals()
{
    m_add_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ReminderApp::on_add_button_clicked));

    // Connect window hide signal to minimize to tray instead of closing
    m_window.signal_delete_event().connect(
        sigc::mem_fun(*this, &ReminderApp::on_window_delete_event));

    // Connect popup window reminder toggle signal
    if (m_popup_window)
    {
        m_popup_window->signal_reminder_toggled().connect(
            sigc::mem_fun(*this, &ReminderApp::on_popup_reminder_toggled));
    }
}

void ReminderApp::initialize_database()
{
    // Open database
    std::string db_path = std::string(getenv("HOME")) + "/.local/share/reminders.db";
    int rc = sqlite3_open(db_path.c_str(), &m_db);

    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
        return;
    }

    // Create table if it doesn't exist
    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS reminders("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "time TEXT NOT NULL,"
        "completed INTEGER DEFAULT 0,"
        "notified INTEGER DEFAULT 0);";

    char *err_msg = nullptr;
    rc = sqlite3_exec(m_db, create_table_sql, nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }

    // Check if the notified column exists - add it for existing databases
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(m_db, "PRAGMA table_info(reminders);", -1, &stmt, nullptr);

    if (rc == SQLITE_OK)
    {
        bool has_notified_column = false;

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::string column_name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            if (column_name == "notified")
            {
                has_notified_column = true;
                break;
            }
        }

        sqlite3_finalize(stmt);

        // Add the column if it doesn't exist
        if (!has_notified_column)
        {
            std::cout << "Adding 'notified' column to existing database..." << std::endl;
            const char *alter_table_sql = "ALTER TABLE reminders ADD COLUMN notified INTEGER DEFAULT 0;";
            rc = sqlite3_exec(m_db, alter_table_sql, nullptr, nullptr, &err_msg);

            if (rc != SQLITE_OK)
            {
                std::cerr << "SQL error when adding notified column: " << err_msg << std::endl;
                sqlite3_free(err_msg);
            }
        }
    }

    // Initialize current date
    m_current_date = get_current_date();

    // Reset notification status for a new day
    reset_notification_status();
}

void ReminderApp::load_reminders()
{
    if (!m_db)
        return;

    // Clear existing reminders
    m_reminders.clear();

    // Prepare SQL statement to get reminders for today
    const char *sql = "SELECT id, title, description, time, completed, notified FROM reminders;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    // Execute query and retrieve results
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Reminder reminder;
        reminder.id = sqlite3_column_int(stmt, 0);
        reminder.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

        const unsigned char *description = sqlite3_column_text(stmt, 2);
        if (description)
        {
            reminder.description = reinterpret_cast<const char *>(description);
        }

        reminder.time = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        reminder.completed = sqlite3_column_int(stmt, 4) != 0;
        reminder.notified = sqlite3_column_int(stmt, 5) != 0;

        m_reminders.push_back(reminder);
    }

    sqlite3_finalize(stmt);

    // Refresh the list view
    refresh_list();
}

void ReminderApp::on_add_button_clicked()
{
    Reminder reminder;
    reminder.title = m_title_entry.get_text();

    // Get time from combo boxes
    Glib::ustring hourStr = m_hour_combo.get_active_text();
    Glib::ustring minute = m_minute_combo.get_active_text();
    Glib::ustring ampm = m_ampm_combo.get_active_text();

    // Convert 12-hour format to 24-hour format for storage
    int hour = std::stoi(hourStr);
    if (ampm == "PM" && hour != 12)
    {
        hour += 12;
    }
    else if (ampm == "AM" && hour == 12)
    {
        hour = 0;
    }

    // Format the time string in 24-hour format (HH:MM)
    std::stringstream timeStream;
    timeStream << std::setw(2) << std::setfill('0') << hour << ":" << minute;
    reminder.time = timeStream.str();
    auto buffer = m_description_textview.get_buffer();
    reminder.description = buffer->get_text();
    reminder.completed = false;
    reminder.notified = false;

    // Validate input
    if (reminder.title.empty())
    {
        auto dialog = Gtk::MessageDialog(m_window, "Error", false, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text("Title cannot be empty!");
        dialog.run();
        return;
    }

    // We don't need to validate time format since it's from combo boxes

    // Add reminder to database
    add_reminder(reminder);

    // Clear input fields
    m_title_entry.set_text("");
    buffer->set_text("");
    // Leave time at current selection
}

void ReminderApp::add_reminder(const Reminder &reminder)
{
    if (!m_db)
        return;

    const char *sql = "INSERT INTO reminders (title, description, time, completed, notified) "
                      "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, reminder.title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, reminder.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, reminder.time.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, reminder.completed ? 1 : 0);
    sqlite3_bind_int(stmt, 5, reminder.notified ? 1 : 0);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(m_db) << std::endl;
    }

    sqlite3_finalize(stmt);

    // Reload reminders
    load_reminders();
}

void ReminderApp::update_reminder(const Reminder &reminder)
{
    if (!m_db)
        return;

    const char *sql = "UPDATE reminders SET title = ?, description = ?, "
                      "time = ?, completed = ?, notified = ? WHERE id = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, reminder.title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, reminder.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, reminder.time.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, reminder.completed ? 1 : 0);
    sqlite3_bind_int(stmt, 5, reminder.notified ? 1 : 0);
    sqlite3_bind_int(stmt, 6, reminder.id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(m_db) << std::endl;
    }

    sqlite3_finalize(stmt);

    // Reload reminders
    load_reminders();
}

void ReminderApp::delete_reminder(int id)
{
    if (!m_db)
        return;

    const char *sql = "DELETE FROM reminders WHERE id = ?;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(m_db) << std::endl;
    }

    sqlite3_finalize(stmt);

    // Reload reminders
    load_reminders();
}

void ReminderApp::refresh_list()
{
    // Remove all children
    auto children = m_list_box.get_children();
    for (auto child : children)
    {
        m_list_box.remove(*child);
        delete child;
    }

    // Add reminders to list
    for (const auto &reminder : m_reminders)
    {
        m_list_box.add(*create_reminder_widget(reminder));
    }

    m_list_box.show_all();

    // Also update the popup window if it exists
    if (m_popup_window)
    {
        m_popup_window->set_reminders(m_reminders);
    }
}

Gtk::Widget *ReminderApp::create_reminder_widget(const Reminder &reminder)
{
    // Create a box to hold the reminder
    auto box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 5));
    box->set_border_width(5);

    // Create checkbox for completion status
    auto check = Gtk::manage(new Gtk::CheckButton());
    check->set_active(reminder.completed);
    check->signal_toggled().connect([this, reminder, check]()
                                    {
        Reminder updated = reminder;
        updated.completed = check->get_active();
        update_reminder(updated); });

    // Create title label
    auto title = Gtk::manage(new Gtk::Label(reminder.title));
    title->set_halign(Gtk::ALIGN_START);
    title->set_hexpand(true);

    // Create time label with 12-hour format
    std::string displayTime = convert_to_12hour_format(reminder.time);
    auto time = Gtk::manage(new Gtk::Label(displayTime));

    // Create edit button
    auto edit_btn = Gtk::manage(new Gtk::Button("Edit"));
    edit_btn->signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &ReminderApp::on_edit_button_clicked), reminder.id));

    // Create delete button
    auto delete_btn = Gtk::manage(new Gtk::Button("Delete"));
    delete_btn->signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &ReminderApp::on_delete_button_clicked), reminder.id));

    // Add widgets to box
    box->pack_start(*check, Gtk::PACK_SHRINK);
    box->pack_start(*title, Gtk::PACK_EXPAND_WIDGET);
    box->pack_start(*time, Gtk::PACK_SHRINK);
    box->pack_start(*edit_btn, Gtk::PACK_SHRINK);
    box->pack_start(*delete_btn, Gtk::PACK_SHRINK);

    // Store the reminder ID in the box
    box->set_data("reminder_id", (gpointer)(long)reminder.id);

    return box;
}

void ReminderApp::on_edit_button_clicked(int id)
{
    // Find the reminder
    auto it = std::find_if(m_reminders.begin(), m_reminders.end(),
                           [id](const Reminder &r)
                           { return r.id == id; });

    if (it == m_reminders.end())
        return;

    // Create dialog for editing
    Gtk::Dialog dialog("Edit Reminder", m_window, true);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Save", Gtk::RESPONSE_OK);
    dialog.set_default_size(400, 300);

    // Add content area
    auto content_area = dialog.get_content_area();
    content_area->set_border_width(10);
    content_area->set_spacing(10);

    // Parse the time string (HH:MM in 24-hour format)
    int hour24 = std::stoi(it->time.substr(0, 2));
    std::string minute_str = it->time.substr(3, 2);

    // Convert to 12-hour format for display
    int hour12 = hour24;
    std::string ampm = "AM";

    if (hour24 >= 12)
    {
        ampm = "PM";
        if (hour24 > 12)
        {
            hour12 = hour24 - 12;
        }
    }
    else if (hour24 == 0)
    {
        hour12 = 12;
    }

    // Format the hour as a string with leading zero if needed
    std::stringstream hour_ss;
    hour_ss << std::setw(2) << std::setfill('0') << hour12;
    std::string hour_str = hour_ss.str();

    // Create input fields with a grid layout for better organization
    Gtk::Grid input_grid;
    input_grid.set_row_spacing(10);
    input_grid.set_column_spacing(10);

    // Title row
    Gtk::Label title_label("Title:");
    Gtk::Entry title_entry;
    title_entry.set_text(it->title);
    title_entry.set_hexpand(true);

    input_grid.attach(title_label, 0, 0, 1, 1);
    input_grid.attach(title_entry, 1, 0, 2, 1);

    // Time row
    Gtk::Label time_label("Time:");

    // Hour combo (12-hour format: 1-12)
    Gtk::ComboBoxText hour_combo;
    for (int i = 1; i <= 12; i++)
    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << i;
        hour_combo.append(ss.str());
    }
    hour_combo.set_active_text(hour_str);

    // Minute combo
    Gtk::ComboBoxText minute_combo;
    for (int i = 0; i < 60; i++)
    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << i;
        minute_combo.append(ss.str());
    }
    minute_combo.set_active_text(minute_str);

    // AM/PM combo
    Gtk::ComboBoxText ampm_combo;
    ampm_combo.append("AM");
    ampm_combo.append("PM");
    ampm_combo.set_active_text(ampm);

    // Time separator
    Gtk::Label time_separator;
    time_separator.set_text(":");

    Gtk::Box time_box(Gtk::ORIENTATION_HORIZONTAL, 5);

    // Set fixed width for combo boxes
    hour_combo.property_width_request() = 60;
    minute_combo.property_width_request() = 60;
    ampm_combo.property_width_request() = 60;

    time_box.pack_start(hour_combo, Gtk::PACK_SHRINK);
    time_box.pack_start(time_separator, Gtk::PACK_SHRINK);
    time_box.pack_start(minute_combo, Gtk::PACK_SHRINK);
    time_box.pack_start(ampm_combo, Gtk::PACK_SHRINK);

    // Set fixed width for entire time box
    time_box.property_width_request() = 200;

    input_grid.attach(time_label, 0, 1, 1, 1);
    input_grid.attach(time_box, 1, 1, 2, 1);

    // Description row
    Gtk::Label desc_label("Description:");
    Gtk::TextView desc_textview;
    desc_textview.get_buffer()->set_text(it->description);
    desc_textview.set_hexpand(true);
    desc_textview.set_vexpand(true);

    // Scrolled window for description
    Gtk::ScrolledWindow desc_scroll;
    desc_scroll.add(desc_textview);
    desc_scroll.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    desc_scroll.set_shadow_type(Gtk::SHADOW_IN);
    desc_scroll.set_size_request(-1, 100);

    input_grid.attach(desc_label, 0, 2, 3, 1);
    input_grid.attach(desc_scroll, 0, 3, 3, 1);

    // Status row
    Gtk::CheckButton completed_check("Completed");
    completed_check.set_active(it->completed);
    input_grid.attach(completed_check, 0, 4, 3, 1);

    // Add grid to content area
    content_area->pack_start(input_grid, true, true, 0);

    dialog.show_all_children();

    int result = dialog.run();

    if (result == Gtk::RESPONSE_OK)
    {
        Reminder updated = *it;
        updated.title = title_entry.get_text();
        updated.description = desc_textview.get_buffer()->get_text();

        // Convert time from 12-hour format to 24-hour format for storage
        int hour = std::stoi(hour_combo.get_active_text());
        std::string minute = minute_combo.get_active_text();
        std::string ampm = ampm_combo.get_active_text();

        // Convert to 24-hour format
        if (ampm == "PM" && hour != 12)
        {
            hour += 12;
        }
        else if (ampm == "AM" && hour == 12)
        {
            hour = 0;
        }

        // Format the time string (24-hour format)
        std::stringstream timeStream;
        timeStream << std::setw(2) << std::setfill('0') << hour << ":" << minute;
        updated.time = timeStream.str();

        updated.completed = completed_check.get_active();

        // Reset notification status if time has changed
        if (updated.time != it->time)
        {
            updated.notified = false;
        }

        update_reminder(updated);
    }
}

void ReminderApp::on_delete_button_clicked(int id)
{
    // Ask for confirmation
    Gtk::MessageDialog dialog(m_window, "Delete Reminder", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text("Are you sure you want to delete this reminder?");

    int result = dialog.run();

    if (result == Gtk::RESPONSE_YES)
    {
        delete_reminder(id);
    }
}

void ReminderApp::start_notification_thread()
{
    m_notification_thread = std::thread(&ReminderApp::notification_loop, this);
}

void ReminderApp::notification_loop()
{
    while (m_thread_running)
    {
        // Get current time and date
        std::string current_time = get_current_time();
        std::string current_date = get_current_date();

        // Check if date has changed
        if (current_date != m_current_date)
        {
            std::cout << "Date changed from " << m_current_date << " to " << current_date << ", resetting notification status." << std::endl;
            m_current_date = current_date;
            reset_notification_status();
        }

        // Check for reminders that should be triggered
        for (const auto &reminder : m_reminders)
        {
            if (!reminder.completed && !reminder.notified && should_notify(reminder.time))
            {
                show_notification(reminder.title, reminder.description);

                // Mark the reminder as notified to prevent duplicate notifications
                Reminder updated = reminder;
                updated.notified = true;
                update_reminder(updated);
            }
        }

        // Sleep for just 15 seconds to make checking more frequent
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}

void ReminderApp::show_notification(const std::string &title, const std::string &body)
{
    if (!notify_is_initted())
    {
        notify_init("ReminderApp");
    }

    NotifyNotification *notification = notify_notification_new(
        title.c_str(),
        body.c_str(),
        "dialog-information");

    notify_notification_set_timeout(notification, NOTIFY_EXPIRES_DEFAULT);

    GError *error = nullptr;
    if (!notify_notification_show(notification, &error))
    {
        std::cerr << "Failed to show notification: " << (error ? error->message : "unknown error") << std::endl;
        if (error)
        {
            g_error_free(error);
        }
    }
    else
    {
        std::cout << "Notification sent: " << title << std::endl;
    }

    g_object_unref(G_OBJECT(notification));
}

std::string ReminderApp::get_current_time()
{
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_time);

    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << now_tm->tm_hour << ":"
       << std::setw(2) << std::setfill('0') << now_tm->tm_min;

    return ss.str();
}

bool ReminderApp::should_notify(const std::string &reminder_time)
{
    std::string current_time = get_current_time();

    // Get current minutes since midnight
    int current_hour = std::stoi(current_time.substr(0, 2));
    int current_minute = std::stoi(current_time.substr(3, 2));
    int current_total_minutes = current_hour * 60 + current_minute;

    // Get reminder minutes since midnight
    int reminder_hour = std::stoi(reminder_time.substr(0, 2));
    int reminder_minute = std::stoi(reminder_time.substr(3, 2));
    int reminder_total_minutes = reminder_hour * 60 + reminder_minute;

    // If the current time is within the last 15 seconds of checking, we should notify
    // This helps avoid missing notifications due to the 15-second check interval
    return (current_total_minutes == reminder_total_minutes);
}

std::string ReminderApp::convert_to_12hour_format(const std::string &time24h)
{
    // Parse hour and minute from 24-hour format (HH:MM)
    int hour = std::stoi(time24h.substr(0, 2));
    std::string minute = time24h.substr(3, 2);

    // Convert to 12-hour format
    std::string period = (hour >= 12) ? "PM" : "AM";

    // Convert hour from 24-hour to 12-hour format
    if (hour > 12)
    {
        hour -= 12;
    }
    else if (hour == 0)
    {
        hour = 12;
    }

    // Format the time string
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << hour << ":"
       << minute << " " << period;

    return ss.str();
}

void ReminderApp::show_window()
{
    m_window.show();
    m_window.present();
}

void ReminderApp::hide_window()
{
    m_window.hide();
}

void ReminderApp::show_window_and_focus_title()
{
    show_window();
    m_title_entry.grab_focus();
}

void ReminderApp::show_popup_window()
{
    // Update the reminders in the popup window
    m_popup_window->set_reminders(m_reminders);
    m_popup_window->show();
}

void ReminderApp::hide_popup_window()
{
    m_popup_window->hide();
}

void ReminderApp::create_tray_icon()
{
    try
    {
        // Create the system tray indicator
        AppIndicator *indicator = app_indicator_new(
            "reminder-app",
            "reminder",
            APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

        // Check if icon is found - if not, try a fallback icon
        if (g_file_test("/usr/share/icons/hicolor/128x128/apps/reminder.png", G_FILE_TEST_EXISTS) ||
            g_file_test("/usr/share/pixmaps/reminder.png", G_FILE_TEST_EXISTS))
        {
            app_indicator_set_icon_full(indicator, "reminder", "Reminder App");
        }
        else
        {
            // Fallback to a standard icon if our custom icon isn't found
            app_indicator_set_icon_full(indicator, "appointment-soon", "Reminder App");
        }

        // Set the status to active
        app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);

        // Create the menu for the indicator
        GtkWidget *menu = gtk_menu_new();

        // Create "Show" menu item
        GtkWidget *show_item = gtk_menu_item_new_with_label("Show Reminders");
        g_signal_connect(show_item, "activate", G_CALLBACK(+[](GtkMenuItem *, gpointer user_data)
                                                           {
                                                               static_cast<ReminderApp *>(user_data)->show_popup_window();
                                                           }),
                         this);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), show_item);

        // Create "Add New" menu item
        GtkWidget *add_item = gtk_menu_item_new_with_label("Add New Reminder");
        g_signal_connect(add_item, "activate", G_CALLBACK(+[](GtkMenuItem *, gpointer user_data)
                                                          {
                                                              auto app = static_cast<ReminderApp *>(user_data);
                                                              app->show_window_and_focus_title();
                                                          }),
                         this);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), add_item);

        // Separator
        GtkWidget *separator = gtk_separator_menu_item_new();
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), separator);

        // Create "Quit" menu item
        GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
        g_signal_connect(quit_item, "activate", G_CALLBACK(+[](GtkMenuItem *, gpointer user_data)
                                                           {
                                                               auto app = static_cast<ReminderApp *>(user_data);
                                                               app->get_window().hide();
                                                               // Make sure we exit the main GTK loop
                                                               Gtk::Main::quit();
                                                           }),
                         this);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_item);

        // Show all menu items
        gtk_widget_show_all(menu);

        // Set the menu for the indicator
        app_indicator_set_menu(indicator, GTK_MENU(menu));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to create system tray icon: " << e.what() << std::endl;
        std::cerr << "The application will run without system tray integration." << std::endl;

        // If we can't create a tray icon, make sure the window is shown
        if (m_start_minimized)
        {
            show_window();
        }
    }
}

void ReminderApp::on_tray_icon_activate()
{
    if (m_window.is_visible())
    {
        m_window.hide();
    }
    else
    {
        m_window.show();
        m_window.present(); // Brings window to front
    }
}

bool ReminderApp::on_window_delete_event(GdkEventAny *event)
{
    // Hide window instead of closing when the close button is clicked
    hide_window();

    // Return true to prevent the window from being destroyed
    return true;
}

std::string ReminderApp::get_current_date()
{
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *now_tm = std::localtime(&now_time);

    std::stringstream ss;
    ss << (now_tm->tm_year + 1900) << "-"
       << std::setw(2) << std::setfill('0') << (now_tm->tm_mon + 1) << "-"
       << std::setw(2) << std::setfill('0') << now_tm->tm_mday;

    return ss.str();
}

void ReminderApp::reset_notification_status()
{
    if (!m_db)
        return;

    const char *sql = "UPDATE reminders SET notified = 0;";
    char *err_msg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error when resetting notification status: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
    else
    {
        std::cout << "Notification status reset for a new day." << std::endl;
    }

    // Reload reminders to update local data
    load_reminders();
}

void ReminderApp::on_popup_reminder_toggled(int reminder_id, bool is_completed)
{
    // Find the reminder with the given ID
    auto it = std::find_if(m_reminders.begin(), m_reminders.end(),
                           [reminder_id](const Reminder &r)
                           { return r.id == reminder_id; });

    if (it == m_reminders.end())
        return;

    // Update the reminder's completed status
    Reminder updated = *it;
    updated.completed = is_completed;
    update_reminder(updated);
}
