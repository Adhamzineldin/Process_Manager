<#
.SYNOPSIS
Graphical system process monitoring and management utility.

.DESCRIPTION
procmanager-gui provides a graphical interface for monitoring and managing system processes.

.FEATURES
List All Processes
   Displays a list of all running processes with their PID, user, name, state description, and state.

Group By User
   Groups processes by their owners and displays them in organized sections.

Show All PIDs
   Lists all process IDs currently active in the system.

Run Process
   Allows executing a new command or application through an input field.

Send Signal
   Enables sending signals to processes by specifying a PID and selecting a signal type from a dropdown menu.

.INTERFACE
Button Panel
   Contains action buttons for listing processes, grouping by user, and showing PIDs.

Text View
   Displays the results of operations in a scrollable window.

Run Process Panel
   Contains an input field for entering commands and a Run button.

Send Signal Panel
   Contains a PID input field, a signal selection dropdown, and a Send button.

.PROCESS STATES
R - Running or runnable (on run queue)
S - Interruptible sleep (waiting for an event)
D - Uninterruptible sleep (usually IO)
Z - Defunct or "zombie" process
T - Stopped, either by a job control signal or because it's being traced

.SIGNALS
SIGTERM (15) - Termination request, allowing cleanup operations
SIGKILL (9)  - Immediate termination without cleanup
SIGSTOP (19) - Suspend execution until a SIGCONT is sent
SIGCONT (18) - Resume a previously stopped process
SIGHUP (1)   - Hangup, often used to reload configurations
SIGUSR1 (10) - User-defined signal 1
SIGUSR2 (12) - User-defined signal 2

.EXAMPLE
procmanager-gui
Starts the process manager in graphical mode.

.NOTES
Some operations may require administrator privileges, especially when managing processes owned by other users.

.RELATED LINKS
procmanager
Get-Process
Stop-Process
Start-Process

.AUTHOR
Your Name <your.email@example.com>
#> 