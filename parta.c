#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>



/*  a function that echoes the command line arguments
    and capitalizes all alphabet letters
    @param argc - number of command line arguments
    @param argv - array of command line argument strings
    @return int - exit status of the program
*/
int main(int argc, const char* argv[]) {
    if (argc < 2) {
        printf("ERROR: No arguments\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];
        for (size_t j = 0; arg[j] != '\0'; j++) {
            putchar(toupper((unsigned char)arg[j]));
        }
        if (i < argc - 1) {
            putchar(',');
        }
    }
    putchar('\n');


    return 0;
}
