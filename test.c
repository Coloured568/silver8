#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h> // Required for EXIT_FAILURE
#include <unistd.h> // Required for access function

bool includes_extension(const char *file) {
    const char *ext = strrchr(file, '.');
    if (ext != NULL && strcmp(ext, ".s8") == 0) {
        return true;
    }
    return false;
}

bool file_exists(const char *file) {
    return access(file, F_OK) == 0;
}

int main(int argc, char **argv)
{
    printf("You have entered %d arguments:\n", argc - 1);

    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
        if (includes_extension(argv[i])) {
            printf("File %s has the .s8 extension.\n", argv[i]);
            if (file_exists(argv[i])) {
                printf("File %s exists.\n", argv[i]);
            } else {
                printf("File %s does not exist.\n", argv[i]);
            }
        } else {
            printf("File %s does not have the .s8 extension.\n", argv[i]);
        }
    }

    return 0;
}
