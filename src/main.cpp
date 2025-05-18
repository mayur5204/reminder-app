#include "reminder_app.h"
#include <gtkmm.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>

// Function to check for another running instance
bool is_another_instance_running()
{
    int pid_file = open("/tmp/reminder_app.lock", O_CREAT | O_RDWR, 0666);
    if (pid_file == -1)
    {
        std::cerr << "Failed to open lock file" << std::endl;
        return false;
    }

    // Try to get an exclusive lock
    if (flock(pid_file, LOCK_EX | LOCK_NB) == -1)
    {
        close(pid_file);
        return true; // Another instance is running
    }

    // Keep the file descriptor open and locked
    // It will be automatically closed when the program exits
    return false;
}

int main(int argc, char *argv[])
{
    // Check for another running instance
    if (is_another_instance_running())
    {
        std::cerr << "Another instance of Reminder App is already running." << std::endl;
        return 1;
    }

    // Default to start minimized (only system tray icon visible)
    bool start_minimized = true;

    // Create a clean copy of arguments without our custom flags
    std::vector<char *> clean_args;
    clean_args.push_back(argv[0]); // Program name

    // Parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--minimize") == 0 || strcmp(argv[i], "-m") == 0)
        {
            start_minimized = true;
            // Don't add this to the clean args
        }
        else if (strcmp(argv[i], "--show") == 0 || strcmp(argv[i], "-s") == 0)
        {
            start_minimized = false;
            // Don't add this to the clean args
        }
        else
        {
            // Keep other arguments
            clean_args.push_back(argv[i]);
        }
    }

    // Convert back to C-style array
    int clean_argc = clean_args.size();
    char **clean_argv = clean_args.data();

    // Initialize GTK
    Gtk::Main kit(clean_argc, clean_argv);

    // Create our reminder app
    ReminderApp reminderApp(start_minimized);

    // Run the main loop without directly associating it with any window
    // This allows the app to keep running when all windows are closed
    Gtk::Main::run();
}
