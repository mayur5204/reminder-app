#pragma once

#include <gtkmm.h>
#include <vector>
#include <string>

// Forward declaration
struct Reminder;

class ReminderPopupWindow
{
public:
    ReminderPopupWindow();
    virtual ~ReminderPopupWindow();

    void show();
    void hide();
    void set_reminders(const std::vector<Reminder> &reminders);
    Gtk::Window &get_window();

    // Signal accessor
    typedef sigc::signal<void, int, bool> type_signal_reminder_toggled;
    type_signal_reminder_toggled signal_reminder_toggled();

private:
    // GUI components
    Gtk::Window m_window;
    Gtk::Box m_main_box;
    Gtk::ScrolledWindow m_scrolled_window;
    Gtk::ListBox m_list_box;
    Gtk::Button m_close_button;

    // Signal for reminder completion toggled
    type_signal_reminder_toggled m_signal_reminder_toggled;

    // Helper methods
    void setup_ui();
    void connect_signals();
    void refresh_list(const std::vector<Reminder> &reminders);
    Gtk::Widget *create_reminder_widget(const Reminder &reminder);

    // Signal handlers
    void on_close_button_clicked();
    bool on_window_delete_event(GdkEventAny *event);
    void on_reminder_toggled(Gtk::CheckButton *check, int reminder_id);
};
