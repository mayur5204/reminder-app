#pragma once

// Standard includes
#include <gtkmm.h>
#include <sqlite3.h>
#include <libnotify/notify.h>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <chrono>

// Forward declarations
class ReminderPopupWindow;

struct Reminder
{
    int id;
    std::string title;
    std::string description;
    std::string time; // Format: "HH:MM"
    bool completed;
    bool notified; // Whether notification has been sent for this reminder today
};

class ReminderApp
{
public:
    ReminderApp(bool start_minimized = false);
    virtual ~ReminderApp();
    Gtk::Window &get_window();

    // Show/hide the main window
    void show_window();
    void hide_window();
    void show_window_and_focus_title(); // Focus the title field when showing

    // Show/hide the popup window
    void show_popup_window();
    void hide_popup_window();

    // For system tray functionality
    void create_tray_icon();
    void on_tray_icon_activate();
    bool on_window_delete_event(GdkEventAny *event);

private:
    // GUI components
    Gtk::Window m_window;
    Gtk::Box m_main_box;
    Gtk::Box m_input_box;
    Gtk::ScrolledWindow m_scrolled_window;
    Gtk::ListBox m_list_box;

    // Popup window for showing reminders
    std::unique_ptr<ReminderPopupWindow> m_popup_window;

    // System tray (AppIndicator) components
    Glib::RefPtr<Gtk::StatusIcon> m_status_icon;
    Glib::RefPtr<Gtk::Application> m_app;
    Gtk::Menu m_popup_menu;
    bool m_start_minimized;

    // Input fields
    Gtk::Entry m_title_entry;
    Gtk::TextView m_description_textview;
    Gtk::ComboBoxText m_hour_combo;
    Gtk::ComboBoxText m_minute_combo;
    Gtk::ComboBoxText m_ampm_combo;
    Gtk::Label m_time_separator;
    Gtk::Box m_time_box;
    Gtk::Button m_add_button;
    Gtk::Frame m_input_frame;

    // Database
    sqlite3 *m_db;

    // Threading for notification checks
    std::thread m_notification_thread;
    std::atomic<bool> m_thread_running;
    std::string m_current_date; // Track the current date for notification reset

    // Vector to store reminders
    std::vector<Reminder> m_reminders;

    // Signal handlers
    void on_add_button_clicked();
    void on_reminder_clicked(int id);
    void on_edit_button_clicked(int id);
    void on_delete_button_clicked(int id);
    void on_popup_reminder_toggled(int reminder_id, bool is_completed);

    // Helper methods
    void setup_ui();
    void connect_signals();
    void initialize_database();
    void load_reminders();
    void add_reminder(const Reminder &reminder);
    void update_reminder(const Reminder &reminder);
    void delete_reminder(int id);
    void refresh_list();
    Gtk::Widget *create_reminder_widget(const Reminder &reminder); // Notification related
    void start_notification_thread();
    void notification_loop();
    void show_notification(const std::string &title, const std::string &body);
    std::string get_current_time();
    std::string get_current_date();
    bool should_notify(const std::string &reminder_time);
    void reset_notification_status();

    // Time format conversion
    std::string convert_to_12hour_format(const std::string &time24h);
};
