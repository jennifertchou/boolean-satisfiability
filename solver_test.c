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
        char* command = malloc(strlen("./solver ") + strlen(yes_cases[i]) + 3);
        strcpy(command, "./solver ");
        strcat(command, "'");
        strcat(command, yes_cases[i]);
        strcat(command, "'");

        if ((fp = popen(command, "r")) == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }
        while (fgets(buf, BUFSIZE, fp) != NULL) {
            printf(buf);
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
        char* command = malloc(strlen("./solver ") + strlen(no_cases[i]) + 3);
        strcpy(command, "./solver ");
        strcat(command, "'");
        strcat(command, no_cases[i]);
        strcat(command, "'");

        if ((fp = popen(command, "r")) == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }
        while (fgets(buf, BUFSIZE, fp) != NULL) {
            printf(buf);
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