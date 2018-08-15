#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Database welcome messages */
void welcome_message() {
    printf("\n");
    printf("Welcome to jbqlite.\n");
    printf("To exit at any time type: .exit.\n");
    printf("\n");
}

/* Print the database command prompt */
void print_prompt() {
    printf("jbqlite >> ");
}