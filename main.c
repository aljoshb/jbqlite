#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* InputBuffer will store the input text the user types in the command prompt */
struct InputBuffer_t {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
};
typedef struct InputBuffer_t InputBuffer;

/* Allocate a new InputBuffer */
InputBuffer* new_input_buffer() {

    InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

/* Print the database prompt. So the user knows it's in the REPL (read evaluate print loop) */
void print_prompt() {
    printf("jbqlite >> ");
}

/* Read the user input from the command line */
void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read-1; // Ignore the trailing newline
    input_buffer->buffer[bytes_read-1] = 0; // Include the NULL terminator
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
