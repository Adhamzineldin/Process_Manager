<#
.SYNOPSIS
Command-line system process monitoring and management utility.

.DESCRIPTION
procmanager provides a menu-driven command-line interface for monitoring and managing system processes.

.MENU OPTIONS
1. List all processes
   Displays a table with PID, user, name, state description, and state of all processes.

2. List processes grouped by user
   Shows processes organized by user accounts, with detailed information for each process.

3. Show all process IDs
   Lists all active process IDs in a readable format.

4. Run a new process
   Prompts for a command to execute as a new process.

5. Stop/signal a process
   Allows sending common signals (SIGTERM, SIGKILL, SIGSTOP, SIGCONT) to a process.

6. Send custom signal to a process
   Enables sending any signal by its number to a specific process.

0. Exit
   Terminates the application.

.PROCESS STATES
R - Running or runnable (on run queue)
S - Interruptible sleep (waiting for an event)
D - Uninterruptible sleep (usually IO)
Z - Defunct or "zombie" process
T - Stopped, either by a job control signal or because it's being traced

.EXAMPLE
procmanager
Starts the process manager in command-line mode.

.NOTES
Some operations may require administrator privileges, especially when managing processes owned by other users.

.RELATED LINKS
procmanager-gui
Get-Process
Stop-Process
Start-Process

.AUTHOR
Your Name <your.email@example.com>
#> 