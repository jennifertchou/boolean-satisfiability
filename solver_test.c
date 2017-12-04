#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128
#define NUMYESCASES 8
#define NUMNOCASES 4

char* yes_cases[NUMYESCASES] = {
    "",
    "1",
    "1 ^ 2",
    "1 ^ 1",
    "1 ^ (2 v 1) ^ (1 v 3 v 4)",
    "1 ^ (2 v 1) ^ (3 v 4)",
    "(1 v -2) ^ (2 v 3) ^ (-3 v -1)",
    "(1 v -2) ^ (2 v -3) ^ (3 v -4) ^ (-3 v -1)"
};

char* no_cases[NUMNOCASES] = {
    "1 ^ -1",
    "1 ^ 2 ^ (-1 ^ -2)",
    "(1 v 2) ^ (1 v -2) ^ (-1 v 2) ^ (-1 v -2)",
    "(1 v 2 v 3)^(1 v 2 v -3)^(1 v -2 v 3)^(1 v -2 v -3)^(-1 v 2 v 3)^(-1 v 2 v -3)^(-1 v -2 v 3)^(-1 v -2 v -3)"
};

FILE *fp;
char buf[BUFSIZE];

int main() {
    printf("---SATISFIABLE FORMULAS---\n");
    for (int i = 0; i < NUMYESCASES; i++) {
        memset(buf, 0, BUFSIZE);
        char* command = malloc(strlen("./solver -i ' '") + strlen(yes_cases[i]));
        strcpy(command, "./solver -i ");
        strcat(command, "'");
        strcat(command, yes_cases[i]);
        strcat(command, "'");

        if ((fp = popen(command, "r")) == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }

        printf("Test case: %s\n", yes_cases[i]);
        // Get the last line of output which is 
        // SATISFIABLE or NOT SATISFIABLE
        while (fgets(buf, BUFSIZE, fp) != NULL) {
            printf(buf);
        }

        // Assert that the output is "SATISFIABLE".
        if (buf[0] == 'S') {
            printf("Test case passed\n\n");
        } else {
        // Wrong answer!
            printf("Incorrect response for %s\n", yes_cases[i]);
            printf("actual: NOT SATISFIABLE\nexpected: SATISFIABLE\n");
            return 1;
        }

        if(pclose(fp)) {
            printf("Command not found or exited with error status\n");
            return -1;
        }
        free(command);
    }

    printf("---NOT SATISFIABLE FORMULAS---\n");
    for (int i = 0; i < NUMNOCASES; i++) {
        memset(buf, 0, BUFSIZE);
        char* command = malloc(strlen("./solver -i ' '") + strlen(no_cases[i]));
        strcpy(command, "./solver -i ");
        strcat(command, "'");
        strcat(command, no_cases[i]);
        strcat(command, "'");

        if ((fp = popen(command, "r")) == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }

        // Get the last line of output which is 
        // SATISFIABLE or NOT SATISFIABLE
        printf("Test case: %s\n", no_cases[i]);
        while (fgets(buf, BUFSIZE, fp) != NULL) {}

        // Assert that the output is "NOT SATISFIABLE".
        if (buf[0] == 'N') {
            printf("Test case passed\n\n");
        } else {
        // Wrong answer!
            printf("Incorrect response for %s\n", no_cases[i]);
            printf("actual: SATISFIABLE\nexpected: NOT SATISFIABLE\n");
            return 1;
        }

        if(pclose(fp))  {
            printf("Command not found or exited with error status\n");
            return -1;
        }
        free(command);
    }
    printf("All tests passed!\n");
    return 0;
}