#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include <ctype.h>

// Structure to hold process information
typedef struct {
    int pid;
    char user[256];
    char name[256];
    char state;
    char state_desc[64];
} ProcessInfo;

// Function to check if a string is a valid number
int is_number(char *str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
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
                sscanf(buffer, "State:\t%c (%63[^)])", &proc->state, proc->state_desc);
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

// Function to list all processes
void list_all_processes() {
    DIR *procdir;
    struct dirent *entry;
    ProcessInfo proc;

    procdir = opendir("/proc");
    if (!procdir) {
        perror("Could not open /proc directory");
        return;
    }

    printf("\n%-10s %-25s %-25s %-20s %s\n", "PID", "USER", "NAME", "DESCRIPTION", "STATE");
    printf("-------------------------------------------------------------------------------------------\n");

    while ((entry = readdir(procdir)) != NULL) {
        if (is_number(entry->d_name)) {
            int pid = atoi(entry->d_name);
            get_process_info(pid, &proc);
            printf("%-10d %-25s %-25s %-20s %c \n", proc.pid, proc.user, proc.name, proc.state_desc, proc.state);
        }
    }

    closedir(procdir);
    printf("\n");
}

// Function to list processes grouped by user
void list_processes_by_user() {
    DIR *procdir;
    struct dirent *entry;
    ProcessInfo *processes = NULL;
    int proc_count = 0;
    char **users = NULL;
    int user_count = 0;

    procdir = opendir("/proc");
    if (!procdir) {
        perror("Could not open /proc directory");
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
        perror("Memory allocation failed");
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
    printf("\nProcesses grouped by user:\n");
    for (i = 0; i < user_count; i++) {
        printf("\n[User: %s]\n", users[i]);
        printf("%-10s %-25s %-20s %s\n", "PID", "NAME", "DESCRIPTION", "STATE");
        printf("-------------------------------------------------------------------\n");

        for (int j = 0; j < proc_count; j++) {
            if (strcmp(processes[j].user, users[i]) == 0) {
                printf("%-10d %-25s %20s %c\n", processes[j].pid, processes[j].name, processes[j].state_desc, processes[j].state);
            }
        }
    }

    // Cleanup
    for (i = 0; i < user_count; i++) {
        free(users[i]);
    }
    free(users);
    free(processes);
    printf("\n");
}

// Function to show all PIDs only
void show_all_pids() {
    DIR *procdir;
    struct dirent *entry;

    procdir = opendir("/proc");
    if (!procdir) {
        perror("Could not open /proc directory");
        return;
    }

    printf("\nAll Process IDs:\n");

    while ((entry = readdir(procdir)) != NULL) {
        if (is_number(entry->d_name)) {
            printf("%s ", entry->d_name);
        }
    }

    closedir(procdir);
    printf("\n\n");
}

// Function to run a new process
void run_process() {
    char command[256];

    printf("Enter the command to run: ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = 0;  // Remove newline

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
    } else if (pid == 0) {
        system(command);
        exit(0);
    } else {
        printf("Process started with PID: %d\n", pid);
    }
}

// Function to stop a process
void stop_process() {
    int pid;
    int signal_num;

    printf("Enter the PID of the process to send a signal: ");
    scanf("%d", &pid);
    getchar();  // Consume newline

    printf("Choose signal to send:\n");
    printf("1. SIGTERM (15) - Termination\n");
    printf("2. SIGKILL (9) - Kill\n");
    printf("3. SIGSTOP (19) - Stop\n");
    printf("4. SIGCONT (18) - Continue\n");
    printf("Enter choice: ");
    scanf("%d", &signal_num);
    getchar();  // Consume newline

    int sig;
    switch (signal_num) {
        case 1: sig = SIGTERM; break;
        case 2: sig = SIGKILL; break;
        case 3: sig = SIGSTOP; break;
        case 4: sig = SIGCONT; break;
        default: sig = SIGTERM;
    }

    if (kill(pid, sig) == 0) {
        printf("Signal sent successfully to process %d\n", pid);
    } else {
        perror("Failed to send signal");
    }
}

// Function to send a custom signal
void send_signal() {
    int pid, sig;

    printf("Enter the PID to send signal to: ");
    scanf("%d", &pid);
    getchar();  // Consume newline

    printf("Enter the signal number to send: ");
    scanf("%d", &sig);
    getchar();  // Consume newline

    if (kill(pid, sig) == 0) {
        printf("Signal %d sent successfully to process %d\n", sig, pid);
    } else {
        perror("Failed to send signal");
    }
}

// Function to display the menu
void display_menu() {
    printf("\n===== Process Manager =====\n");
    printf("1. List all processes\n");
    printf("2. List processes grouped by user\n");
    printf("3. Show all process IDs\n");
    printf("4. Run a new process\n");
    printf("5. Stop/signal a process\n");
    printf("6. Send custom signal to a process\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

// Main function
int main() {
    int choice;

    while (1) {
        display_menu();
        scanf("%d", &choice);
        getchar();  // Consume newline

        switch (choice) {
            case 1:
                list_all_processes();
                break;
            case 2:
                list_processes_by_user();
                break;
            case 3:
                show_all_pids();
                break;
            case 4:
                run_process();
                break;
            case 5:
                stop_process();
                break;
            case 6:
                send_signal();
                break;
            case 0:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}