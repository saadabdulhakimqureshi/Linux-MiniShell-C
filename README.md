# Linux-MiniShell

This a linux minishell. It supports a variety of operations such as I/O direction, background jobs, piping(max 2), and many linux built in commands.

Course: CS 232 Operating Systems

## Instructions

Apart from running executables, the shell supports the following essential built-in commands:

1. jobs - provides a list of all background processes and their local pIDs.

2. cd PATHNAME - sets the PATHNAME as working directory.

3. history - prints a list of previously executed commands. Assume 10 as the maximum history size.

4. kill PID - terminates the background process identified locally with PID in the jobs list.

5. !CMD - runs the command numbered CMD in the command history.

6. exit - terminates the shell only if there are no background jobs.

7. help - prints the list of builtin commands along with their description.

## Run

Open Linux Terminal and change root directory to project.

Write "make final" and then "./final" to run the shell.

## Implementation

Programming Language: C

## Requirements

Linux Terminal
