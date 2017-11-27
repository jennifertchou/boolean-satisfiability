#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

char* yes_cases[6] = {
    "",
    "a",
    "a ^ b",
    "a ^ a",
    "a ^ (b v a) ^ (a v d v e)",
    "a ^ (b v a) ^ (d v e)"
};

char* no_cases[4] = {
    "a ^ ~a",
    "a ^ b ^ (~a ^ ~b)",
    "(a v b) ^ (a v ~b) ^ (~a v b) ^ (~a v ~b)",
    "(xvyvz)^(xvyv~z)^(xv~yvz)^(xv~yv~z)^(~xvyvz)^(~xvyv~z)^(~xv~yvz)^(~xv~yv~z)"
};

FILE *fp;
char buf[BUFSIZE];

int main() {
    printf("SATISFIABLE FORMULAS:\n");
    for (int i = 0; i < 6; i++) {
        memset(buf, 0, BUFSIZE);
        char* command = malloc(strlen("./solver -f ' '") + strlen(yes_cases[i]));
        strcpy(command, "./solver -f ");
        strcat(command, "'");
        strcat(command, yes_cases[i]);
        strcat(command, "'");

        if ((fp = popen(command, "r")) == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }

        // Get the last line of output which is 
        // SATISFIABLE or NOT SATISFIABLE
        while (fgets(buf, BUFSIZE, fp) != NULL) {}

        // Assert that the output is "SATISFIABLE".
        if (buf[0] == 'S') {
            printf("%s...Test case passed\n", yes_cases[i]);
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
        printf("\n");
        free(command);
    }

    printf("NOT SATISFIABLE FORMULAS:\n");
    for (int i = 0; i < 4; i++) {
        memset(buf, 0, BUFSIZE);
        char* command = malloc(strlen("./solver -f ' '") + strlen(no_cases[i]));
        strcpy(command, "./solver -f ");
        strcat(command, "'");
        strcat(command, no_cases[i]);
        strcat(command, "'");

        if ((fp = popen(command, "r")) == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }

        // Get the last line of output which is 
        // SATISFIABLE or NOT SATISFIABLE
        while (fgets(buf, BUFSIZE, fp) != NULL) {}

        // Assert that the output is "NOT SATISFIABLE".
        if (buf[0] == 'N') {
            printf("%s...Test case passed\n", no_cases[i]);
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
        printf("\n");
        free(command);
    }
    return 0;
}