#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include <ctype.h>
#include <gtk/gtk.h>

// Structure to hold process information
typedef struct {
    int pid;
    char user[256];
    char name[256];
    char state;
} ProcessInfo;

// Global GTK widgets
GtkWidget *window;
GtkWidget *text_view;
GtkTextBuffer *buffer;
GtkWidget *pid_entry;
GtkWidget *command_entry;
GtkWidget *signal_combo;

// Function to check if a string is a valid number
int is_number(char *str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

// Function to append text to the text view
void append_text(const char *text) {
    GtkTextIter iter;

    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, text, -1);

    // Scroll to the end
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(text_view), &iter, 0.0, TRUE, 0.0, 1.0);
}

// Function to clear the text view
void clear_text() {
    gtk_text_buffer_set_text(buffer, "", -1);
}

// Function to get process information
void get_process_info(int pid, ProcessInfo *proc) {
    char path[256];
    char buffer[1024];
    FILE *status_file;

    // Get process status
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    status_file = fopen(path, "r");

    if (status_file) {
        proc->pid = pid;

        // Default values
        strcpy(proc->name, "Unknown");
        strcpy(proc->user, "Unknown");
        proc->state = '?';

        while (fgets(buffer, sizeof(buffer), status_file)) {
            // Get process name
            if (strncmp(buffer, "Name:", 5) == 0) {
                sscanf(buffer, "Name:\t%255s", proc->name);
            }
                // Get process state
            else if (strncmp(buffer, "State:", 6) == 0) {
                sscanf(buffer, "State:\t%c", &proc->state);
            }
                // Get process uid
            else if (strncmp(buffer, "Uid:", 4) == 0) {
                int uid;
                sscanf(buffer, "Uid:\t%d", &uid);
                struct passwd *pw = getpwuid(uid);
                if (pw) {
                    strncpy(proc->user, pw->pw_name, sizeof(proc->user)-1);
                    proc->user[sizeof(proc->user)-1] = '\0';
                }
            }
        }
        fclose(status_file);
    }
}

// Callback for listing all processes
void list_all_processes_callback(GtkWidget *widget, gpointer data) {
    DIR *procdir;
    struct dirent *entry;
    ProcessInfo proc;
    char line[512];

    clear_text();

    procdir = opendir("/proc");
    if (!procdir) {
        append_text("Could not open /proc directory\n");
        return;
    }

    append_text("PID     USER            NAME                STATE\n");
    append_text("------------------------------------------------\n");

    while ((entry = readdir(procdir)) != NULL) {
        if (is_number(entry->d_name)) {
            int pid = atoi(entry->d_name);
            get_process_info(pid, &proc);

            snprintf(line, sizeof(line), "%-8d %-15s %-20s %c\n",
                     proc.pid, proc.user, proc.name, proc.state);
            append_text(line);
        }
    }

    closedir(procdir);
}

// Callback for listing processes grouped by user
void list_processes_by_user_callback(GtkWidget *widget, gpointer data) {
    DIR *procdir;
    struct dirent *entry;
    ProcessInfo *processes = NULL;
    int proc_count = 0;
    char **users = NULL;
    int user_count = 0;
    char line[512];

    clear_text();

    procdir = opendir("/proc");
    if (!procdir) {
        append_text("Could not open /proc directory\n");
        return;
    }

    // Count total number of processes
    while ((entry = readdir(procdir)) != NULL) {
        if (is_number(entry->d_name)) {
            proc_count++;
        }
    }

    // Allocate memory for processes
    processes = (ProcessInfo *)malloc(proc_count * sizeof(ProcessInfo));
    if (!processes) {
        append_text("Memory allocation failed\n");
        closedir(procdir);
        return;
    }

    // Reset directory stream
    rewinddir(procdir);

    // Get all processes info
    int i = 0;
    while ((entry = readdir(procdir)) != NULL && i < proc_count) {
        if (is_number(entry->d_name)) {
            int pid = atoi(entry->d_name);
            get_process_info(pid, &processes[i]);
            i++;
        }
    }

    closedir(procdir);

    // Extract unique users
    for (i = 0; i < proc_count; i++) {
        int j;
        int found = 0;

        for (j = 0; j < user_count; j++) {
            if (strcmp(processes[i].user, users[j]) == 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            users = (char **)realloc(users, (user_count + 1) * sizeof(char *));
            users[user_count] = strdup(processes[i].user);
            user_count++;
        }
    }

    // Print processes grouped by user
    append_text("Processes grouped by user:\n\n");

    for (i = 0; i < user_count; i++) {
        snprintf(line, sizeof(line), "[User: %s]\n", users[i]);
        append_text(line);
        append_text("PID     NAME                STATE\n");
        append_text("--------------------------------\n");

        for (int j = 0; j < proc_count; j++) {
            if (strcmp(processes[j].user, users[i]) == 0) {
                snprintf(line, sizeof(line), "%-8d %-20s %c\n",
                         processes[j].pid, processes[j].name, processes[j].state);
                append_text(line);
            }
        }
        append_text("\n");
    }

    // Cleanup
    for (i = 0; i < user_count; i++) {
        free(users[i]);
    }
    free(users);
    free(processes);
}

// Callback for showing all PIDs
void show_all_pids_callback(GtkWidget *widget, gpointer data) {
    DIR *procdir;
    struct dirent *entry;
    char line[512];

    clear_text();

    procdir = opendir("/proc");
    if (!procdir) {
        append_text("Could not open /proc directory\n");
        return;
    }

    append_text("All Process IDs:\n\n");

    int count = 0;
    while ((entry = readdir(procdir)) != NULL) {
        if (is_number(entry->d_name)) {
            snprintf(line, sizeof(line), "%s ", entry->d_name);
            append_text(line);
            count++;

            // Add a newline every 10 PIDs for readability
            if (count % 10 == 0) {
                append_text("\n");
            }
        }
    }

    closedir(procdir);
    append_text("\n");
}

// Callback for running a process
void run_process_callback(GtkWidget *widget, gpointer data) {
    const char *command = gtk_entry_get_text(GTK_ENTRY(command_entry));
    char result[512];

    if (strlen(command) == 0) {
        append_text("Please enter a command to run\n");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        append_text("Fork failed\n");
    } else if (pid == 0) {
        // Child process
        system(command);
        exit(0);
    } else {
        snprintf(result, sizeof(result), "Process started with PID: %d\n", pid);
        append_text(result);
    }

    // Clear the command entry
    gtk_entry_set_text(GTK_ENTRY(command_entry), "");
}

// Callback for sending a signal to a process
void send_signal_callback(GtkWidget *widget, gpointer data) {
    const char *pid_text = gtk_entry_get_text(GTK_ENTRY(pid_entry));
    int pid;
    char result[512];

    if (strlen(pid_text) == 0) {
        append_text("Please enter a valid PID\n");
        return;
    }

    pid = atoi(pid_text);
    if (pid <= 0) {
        append_text("Invalid PID\n");
        return;
    }

    // Get selected signal
    int signal_index = gtk_combo_box_get_active(GTK_COMBO_BOX(signal_combo));
    int sig;

    switch (signal_index) {
        case 0: sig = SIGTERM; break;
        case 1: sig = SIGKILL; break;
        case 2: sig = SIGSTOP; break;
        case 3: sig = SIGCONT; break;
        case 4: sig = SIGHUP; break;
        case 5: sig = SIGUSR1; break;
        case 6: sig = SIGUSR2; break;
        default: sig = SIGTERM;
    }

    if (kill(pid, sig) == 0) {
        snprintf(result, sizeof(result), "Signal %d sent successfully to process %d\n", sig, pid);
        append_text(result);
    } else {
        append_text("Failed to send signal. Check if you have the necessary permissions.\n");
    }
}

// Main function
int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Process Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to organize widgets
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create buttons section (horizontal box)
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 5);

    // Create buttons
    GtkWidget *list_button = gtk_button_new_with_label("List All Processes");
    GtkWidget *list_by_user_button = gtk_button_new_with_label("Group By User");
    GtkWidget *show_pids_button = gtk_button_new_with_label("Show All PIDs");

    // Pack buttons into the button box
    gtk_box_pack_start(GTK_BOX(button_box), list_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), list_by_user_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), show_pids_button, TRUE, TRUE, 0);

    // Connect button signals
    g_signal_connect(list_button, "clicked", G_CALLBACK(list_all_processes_callback), NULL);
    g_signal_connect(list_by_user_button, "clicked", G_CALLBACK(list_processes_by_user_callback), NULL);
    g_signal_connect(show_pids_button, "clicked", G_CALLBACK(show_all_pids_callback), NULL);

    // Create text view for output
    text_view = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

    // Create scroll window for text view
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Create controls section
    GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), control_box, FALSE, FALSE, 5);

    // Create "Run Process" section
    GtkWidget *run_frame = gtk_frame_new("Run Process");
    GtkWidget *run_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(run_frame), run_box);

    command_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(command_entry), "Enter command");
    GtkWidget *run_button = gtk_button_new_with_label("Run");

    gtk_box_pack_start(GTK_BOX(run_box), command_entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(run_box), run_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(control_box), run_frame, TRUE, TRUE, 0);

    g_signal_connect(run_button, "clicked", G_CALLBACK(run_process_callback), NULL);

    // Create "Send Signal" section
    GtkWidget *signal_frame = gtk_frame_new("Send Signal to Process");
    GtkWidget *signal_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(signal_frame), signal_box);

    pid_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(pid_entry), "PID");
    gtk_widget_set_size_request(pid_entry, 80, -1);

    signal_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGTERM (15)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGKILL (9)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGSTOP (19)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGCONT (18)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGHUP (1)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGUSR1 (10)");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(signal_combo), "SIGUSR2 (12)");
    gtk_combo_box_set_active(GTK_COMBO_BOX(signal_combo), 0);

    GtkWidget *send_button = gtk_button_new_with_label("Send");

    gtk_box_pack_start(GTK_BOX(signal_box), pid_entry, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(signal_box), signal_combo, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(signal_box), send_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(control_box), signal_frame, TRUE, TRUE, 0);

    g_signal_connect(send_button, "clicked", G_CALLBACK(send_signal_callback), NULL);

    // Set initial text
    gtk_text_buffer_set_text(buffer, "Welcome to Process Manager!\n"
                                     "Use the buttons above to manage processes.\n", -1);

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}