# Minishell
This practice asked to design and implement, in C language and over the UNIX/Linux Operating System, a program that acts like a shell. A shell allows the user to communicate with the kernel of the Operating System using simple commandsor a sequence of commands and commands with redirections. For the management of processes, we will use the POSIX system calls such as fork, wait, exit. For process communication pipe, dup, close and signal system calls.
The minishell reads and executes the commands using the standard input (file descriptor = 0). It uses the standard output (file descriptor = 1) to show on the screen each result. And it uses the standard error (file descriptor = 2 ) to show the occurred errors.
For the development of this lab a parser is given to us. This parser can read the commands introduced by the user.
