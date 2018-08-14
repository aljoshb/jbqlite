#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/input.h"

/* Print the database command prompt */
void print_prompt() {
    printf("jbqlite >> ");
}

/* Entry point of the program */
int main(int argc, char const *argv[]) {

    /* Creating a simple REPL for the user to interact with*/
    InputBuffer* input_buffer = new_input_buffer();

    printf("\n");
    printf("Welcome to jbqlite.\n");
    printf("To exit at any time type: .exit.\n");
    printf("\n");

    /* REPL loop */
    while(1) {
        print_prompt();
        read_input(input_buffer);

        /* Check if the user wants to exit */
        if (strcmp(input_buffer->buffer, ".exit") == 0) {
            exit(EXIT_SUCCESS);
        }
        else {
            printf("Unrecognized command: '%s'\n", input_buffer->buffer);
        }

    }

    return 0;
}
