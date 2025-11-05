#include <stdbool.h>
#include <stddef.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*  a program that echoes the middle command line arguments
    using the /bin/echo program
    @param argc - number of command line arguments
    @param argv - array of command line argument strings
    @return int - exit status of the program
*/
int main(int argc, char* argv[]) {
    if (argc < 2) {
        const char* error_msg = "ERROR: No arguments\n";
        write(STDOUT_FILENO, error_msg, strlen(error_msg));
        return 1;
    }

    int n = argc - 1; /* number of user-provided args */
    int mid_start;
    if (n % 2 == 1) {
        /* odd: single middle element at (n/2)+1 (1-based) */
        mid_start = n / 2 + 1;
    } else {
        /* even: two middle elements start at n/2 (1-based) */
        mid_start = n / 2;
    }
    int mid_end = (n % 2 == 1) ? mid_start : (mid_start + 1);
    int num_args = mid_end - mid_start + 1;

    char** eargs = malloc((num_args + 2) * sizeof(char*));
    if (eargs == NULL) {
        const char* error_msg = "ERROR: memory allocation failed\n";
        write(STDOUT_FILENO, error_msg, strlen(error_msg));
        return 1;
    }

    eargs[0] = "echo";
    for (int i = 0; i < num_args; i++) {
        /* mid_start is 1-based index into argv (argv[1]..argv[n]) */
        eargs[i + 1] = argv[mid_start + i];
    }
    eargs[num_args + 1] = NULL;

    int eret = execv("/bin/echo", eargs);
    if (eret == -1) {
        const char* error_msg = "ERROR: exec failed\n";
        write(STDOUT_FILENO, error_msg, strlen(error_msg));
        free(eargs);
        return 1;
    }

    free(eargs);
    return 0;
}
