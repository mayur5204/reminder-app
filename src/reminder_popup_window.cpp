#include "reminder_popup_window.h"
#include "reminder_app.h" // For the Reminder struct
#include <iostream>
#include <iomanip>
#include <sstream>

ReminderPopupWindow::ReminderPopupWindow() : m_main_box(Gtk::ORIENTATION_VERTICAL, 10)
{
    // Set up the UI components
    setup_ui();

    // Connect signal handlers
    connect_signals();
}

ReminderPopupWindow::~ReminderPopupWindow()
{
    // Nothing specific to clean up
}

void ReminderPopupWindow::setup_ui()
{
    // Window setup
    m_window.set_title("Reminders");
    m_window.set_default_size(350, 400);
    m_window.set_border_width(10);
    m_window.set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    m_window.set_skip_taskbar_hint(true); // Don't show in taskbar
    m_window.set_decorated(true);         // Keep window decorations for dragging
    m_window.set_visible(false);          // Hide the window initially

    // Add the main box to the window
    m_window.add(m_main_box);

    // Add a title label at the top
    auto title_label = Gtk::manage(new Gtk::Label());
    title_label->set_markup("<b>Your Reminders</b>");
    title_label->set_margin_bottom(10);
    m_main_box.pack_start(*title_label, Gtk::PACK_SHRINK);

    // Add a separator
    auto separator = Gtk::manage(new Gtk::Separator(Gtk::ORIENTATION_HORIZONTAL));
    separator->set_margin_bottom(10);
    m_main_box.pack_start(*separator, Gtk::PACK_SHRINK);

    // Set up the scrolled window
    m_scrolled_window.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    m_scrolled_window.set_min_content_height(300);
    m_scrolled_window.set_shadow_type(Gtk::SHADOW_IN);
    m_main_box.pack_start(m_scrolled_window, Gtk::PACK_EXPAND_WIDGET);

    // Set up list box with better styling
    m_list_box.set_selection_mode(Gtk::SELECTION_NONE);
    m_list_box.set_margin_top(5);
    m_list_box.set_margin_bottom(5);
    m_scrolled_window.add(m_list_box);

    // Add a close button at the bottom
    m_close_button.set_label("Close");
    m_close_button.set_halign(Gtk::ALIGN_CENTER);
    m_close_button.set_margin_top(10);
    m_close_button.property_width_request() = 100;
    m_main_box.pack_start(m_close_button, Gtk::PACK_SHRINK);

    // Make sure all components are visible
    m_window.show_all_children();
}

void ReminderPopupWindow::connect_signals()
{
    // Connect close button
    m_close_button.signal_clicked().connect(
        sigc::mem_fun(*this, &ReminderPopupWindow::on_close_button_clicked));

    // Connect window delete event
    m_window.signal_delete_event().connect(
        sigc::mem_fun(*this, &ReminderPopupWindow::on_window_delete_event));
}

void ReminderPopupWindow::show()
{
    // Show and bring to front
    m_window.show();
    m_window.present();
}

void ReminderPopupWindow::hide()
{
    m_window.hide();
}

Gtk::Window &ReminderPopupWindow::get_window()
{
    return m_window;
}

void ReminderPopupWindow::set_reminders(const std::vector<Reminder> &reminders)
{
    refresh_list(reminders);
}

void ReminderPopupWindow::refresh_list(const std::vector<Reminder> &reminders)
{
    // Clear existing widgets
    auto children = m_list_box.get_children();
    for (auto child : children)
    {
        m_list_box.remove(*child);
        delete child;
    }

    // Add reminder widgets
    for (const auto &reminder : reminders)
    {
        auto widget = create_reminder_widget(reminder);
        if (widget)
        {
            m_list_box.add(*widget);
            widget->show_all();
        }
    }
}

Gtk::Widget *ReminderPopupWindow::create_reminder_widget(const Reminder &reminder)
{
    // Create a box for the reminder row
    auto row_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));
    row_box->set_border_width(5);

    // Create a frame for better visual organization
    auto frame = Gtk::manage(new Gtk::Frame());
    frame->set_shadow_type(Gtk::SHADOW_ETCHED_IN);

    // Set margin to create spacing between reminders
    frame->set_margin_bottom(6);
    frame->set_margin_top(2);
    frame->set_margin_start(2);
    frame->set_margin_end(2);

    // Style the frame based on completion status
    if (reminder.completed)
    {
        // Set a subtle background color for completed reminders
        Gtk::StyleContext::add_provider_for_screen(
            Gdk::Screen::get_default(),
            Gtk::CssProvider::create(),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        frame->get_style_context()->add_class("completed-reminder");
        frame->set_name("completed-reminder-" + std::to_string(reminder.id));

        // Add inline CSS for the completed reminder
        auto css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data(
            "#completed-reminder-" + std::to_string(reminder.id) + " { "
                                                                   "background-color: rgba(200, 255, 200, 0.5); "
                                                                   "border: 1px solid rgba(100, 200, 100, 0.5); "
                                                                   "}");

        frame->get_style_context()->add_provider(
            css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    frame->add(*row_box);

    // Add a checkbox to mark reminder as complete
    auto complete_check = Gtk::manage(new Gtk::CheckButton());
    complete_check->set_active(reminder.completed);
    complete_check->set_tooltip_text("Mark as complete");

    // Make the checkbox more visible
    complete_check->set_margin_start(5);
    complete_check->set_margin_end(5);

    // Store the reminder ID as data in the checkbox for the signal handler
    complete_check->set_data("reminder_id", GINT_TO_POINTER(reminder.id));

    // Connect checkbox toggled signal
    complete_check->signal_toggled().connect(
        sigc::bind(sigc::mem_fun(*this, &ReminderPopupWindow::on_reminder_toggled),
                   complete_check, reminder.id));

    row_box->pack_start(*complete_check, Gtk::PACK_SHRINK);

    // Create a vertical box for title and description
    auto content_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 4));
    content_box->set_margin_start(3);
    content_box->set_margin_end(5);
    content_box->set_margin_top(5);
    content_box->set_margin_bottom(5);
    row_box->pack_start(*content_box, Gtk::PACK_EXPAND_WIDGET);

    // Title with bold formatting
    auto title_label = Gtk::manage(new Gtk::Label());
    std::string markup = "<b>" + reminder.title + "</b>";

    // Add strikethrough if completed
    if (reminder.completed)
    {
        markup = "<b><s>" + reminder.title + "</s></b>";
    }

    title_label->set_markup(markup);
    title_label->set_halign(Gtk::ALIGN_START);
    title_label->set_ellipsize(Pango::ELLIPSIZE_END);
    title_label->set_max_width_chars(35);
    content_box->pack_start(*title_label, Gtk::PACK_SHRINK);

    // Description text
    if (!reminder.description.empty())
    {
        auto desc_label = Gtk::manage(new Gtk::Label());

        // Add strikethrough if completed
        if (reminder.completed)
        {
            desc_label->set_markup("<small><s>" + reminder.description + "</s></small>");
        }
        else
        {
            desc_label->set_markup("<small>" + reminder.description + "</small>");
        }

        desc_label->set_line_wrap(true);
        desc_label->set_line_wrap_mode(Pango::WRAP_WORD_CHAR);
        desc_label->set_halign(Gtk::ALIGN_START);
        desc_label->set_xalign(0.0);
        desc_label->set_max_width_chars(35);
        desc_label->set_ellipsize(Pango::ELLIPSIZE_END);
        content_box->pack_start(*desc_label, Gtk::PACK_SHRINK);
    }

    // Create a horizontal box for time and status indicators
    auto info_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 10));
    info_box->set_margin_top(2);
    content_box->pack_start(*info_box, Gtk::PACK_SHRINK);

    // Format time in 12-hour format
    std::string time_str = reminder.time;
    // Extract hour and minute
    int hour = std::stoi(reminder.time.substr(0, 2));
    std::string minute = reminder.time.substr(3, 2);
    std::string ampm = "AM";

    // Convert to 12-hour format
    if (hour >= 12)
    {
        ampm = "PM";
        if (hour > 12)
        {
            hour -= 12;
        }
    }
    else if (hour == 0)
    {
        hour = 12;
    }

    std::stringstream time_ss;
    time_ss << hour << ":" << minute << " " << ampm;
    time_str = time_ss.str();

    // Time with icon
    auto time_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 3));

    auto time_label = Gtk::manage(new Gtk::Label());
    if (reminder.completed)
    {
        time_label->set_markup("<small><s>" + time_str + "</s></small>");
    }
    else
    {
        time_label->set_markup("<small><i>⏰ " + time_str + "</i></small>");
    }
    time_label->set_halign(Gtk::ALIGN_START);
    time_box->pack_start(*time_label, Gtk::PACK_SHRINK);

    info_box->pack_start(*time_box, Gtk::PACK_SHRINK);

    // Add status indicator (completed, notified)
    std::string status;
    std::string status_icon;
    if (reminder.completed)
    {
        status = "✓ Completed";
        status_icon = "✓";
    }
    else if (reminder.notified)
    {
        status = "🔔 Notified";
        status_icon = "🔔";
    }

    if (!status.empty())
    {
        auto status_label = Gtk::manage(new Gtk::Label());
        status_label->set_markup("<small><i>" + status + "</i></small>");
        status_label->set_halign(Gtk::ALIGN_START);
        info_box->pack_start(*status_label, Gtk::PACK_SHRINK);
    }

    return frame;
}

void ReminderPopupWindow::on_close_button_clicked()
{
    hide();
}

bool ReminderPopupWindow::on_window_delete_event(GdkEventAny *event)
{
    // Just hide the window instead of destroying it
    hide();
    return true; // Prevent the window from being destroyed
}

void ReminderPopupWindow::on_reminder_toggled(Gtk::CheckButton *check, int reminder_id)
{
    bool is_active = check->get_active();

    // Emit the signal with the reminder ID and its new completion state
    m_signal_reminder_toggled.emit(reminder_id, is_active);
}

ReminderPopupWindow::type_signal_reminder_toggled ReminderPopupWindow::signal_reminder_toggled()
{
    return m_signal_reminder_toggled;
}
