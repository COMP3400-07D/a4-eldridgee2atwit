#include <stdbool.h>
#include <stddef.h>

// Needed headers for fork/exec/wait and I/O
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/*
For this assignment, you are to write a program that uses the *fork-exec* flow. The parent
process will wait for the child process, which runs the program `grep` to complete. If the child
process is successful, print FOUND, if not, print an error message.

### Background of the grep command

In this part, you will use the `grep` command. This command searches for patterns (called "regular
expressions") in a file. If the pattern is found, `grep` program exists with status 0. If not, it
exists with a non-zero exit status.

For example,

    $ grep -s fox tests/file1.txt

Will exit with status 0 because the file contains the word `fox`.

On the other hand,

    $ grep -s deer tests/file1.txt

Will exit with status 1 because the word `deer` does not exist in the file.

### Background on using wait()

The wait system call allows a parent process to *wait* for the child process to exit. The child
process can produce an exit status in two ways: the `main` function returns an int value, or calls
the `exit` system call with the value.

THe parent process can collect the said value by using the `wait` system call.

### The Task

Write a program that will take a command line argument, which is the word to search for. First check
if you have two arguments. If not, then print a help message and exit with status 1 (return 1 from
main).

Once you have all the arguments, fork a child process and run the `/usr/bin/grep` command from the
child process. The argument to the `grep` program should be "-s" to make errors quiet, "-q" to make
the output also quiet, the word you want to search for, and the name of the file to search in. So
there are a total of 4 arguments (5 if you include the `NULL`).

If the word is found, the parent process will get an exit status of 0 and print that the word has
been found. If not, the parent process should print what the error is. View grep's manpage to find
out what exit status' exist. If the error indicated that the word can't be found, print "NOT FOUND"
with your own exit status of 0. If the error indicated that the file couldn't be found, print "ERROR" and exit
with status 2.

For example:

    $ ./partc fox tests/file1.txt
    FOUND: fox

    $ ./partc deer tests/file1.txt
    NOT FOUND: deer

    $ ./partc deer tests/no_such_file.txt
    ERROR: tests/no_such_file.txt doesn't exist
    $ echo $?
    2

Like Part A, if there are no arguments, return 1 from main.

    $ ./partc
    ERROR: no arguments
    $ echo $?
    1

The "tests" directory contains input files.

[!NOTE]
> There are two sets of command line arguments: your own, and grep's. You need to take your own
> command line arguments and create a new array of grep's command line arguments.
>
> There are also two sets of exist status: *your own exit status*, and the "grep" command's exit
> status.
>
> If you have extra output, like below, this means you didn't use the "-q" argument. Add it to
> your args list.

    $ ./partc deer tests/file1.txt
    The quick brown fox jumps over the lazy dog
    NOT FOUND: deer
*/



int main(int argc, const char* argv[]) {
    /* Expect two command line arguments: the word to search, and the filename.
       argv[0] - program
       argv[1] - word
       argv[2] - filename
    */
    if (argc < 3) {
        /* Tests expect this exact message when no arguments are provided */
        const char* msg = "ERROR: No arguments\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return 1;
    }

    const char* word = argv[1];
    const char* filename = argv[2];

    pid_t pid = fork();
    if (pid < 0) {
        const char* msg = "ERROR: fork failed\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return 1;
    }

    if (pid == 0) {
        /* Child: exec /usr/bin/grep with args: grep -s -q word filename */
        char* eargs[6];
        eargs[0] = "grep";
        eargs[1] = "-s";
        eargs[2] = "-q";
        /* cast away const because execv expects char*[] */
        eargs[3] = (char*)word;
        eargs[4] = (char*)filename;
        eargs[5] = NULL;

        execv("/usr/bin/grep", eargs);
        /* If execv returns, it failed */
        const char* msg = "ERROR: exec failed\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        _exit(1);
    }

    /* Parent: wait for child and interpret grep's exit status */
    int status = 0;
    pid_t w = waitpid(pid, &status, 0);
    if (w == -1) {
        const char* msg = "ERROR: waitpid failed\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return 1;
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code == 0) {
            /* Found */
            char out[256];
            /* ensure formatting matches tests: "FOUND: word" */
            snprintf(out, sizeof(out), "FOUND: %s\n", word);
            write(STDOUT_FILENO, out, strlen(out));
            return 0;
        } else if (code == 1) {
            /* Not found */
            char out[256];
            snprintf(out, sizeof(out), "NOT FOUND: %s\n", word);
            write(STDOUT_FILENO, out, strlen(out));
            /* As the assignment requests, return our own 0 on not-found */
            return 0;
        } else if (code == 2) {
            /* Grep error such as file not found */
            char out[512];
            snprintf(out, sizeof(out), "ERROR: %s doesn't exist\n", filename);
            write(STDOUT_FILENO, out, strlen(out));
            return 2;
        } else {
            /* Other exit codes: treat as generic error */
            const char* msg = "ERROR: grep failed\n";
            write(STDOUT_FILENO, msg, strlen(msg));
            return 1;
        }
    } else if (WIFSIGNALED(status)) {
        const char* msg = "ERROR: child terminated by signal\n";
        write(STDOUT_FILENO, msg, strlen(msg));
        return 1;
    }

    return 1;
}
