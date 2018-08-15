#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/codes.h"
#include "../include/input.h"
#include "../include/statements.h"
#include "../include/interface.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit")==0) {
        exit(EXIT_SUCCESS);
    }
    else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

/* Entry point of the program */
int main(int argc, char const *argv[]) {

    /* Welcome message */
    welcome_message();

    /* A buffer to hold the command line inputs */
    InputBuffer* input_buffer = new_input_buffer();

    /* REPL loop */
    while(1) {

        /* Print the database command line prompt */
        print_prompt();

        /* Read the user's input */
        read_input(input_buffer);

        /* 
         * Check if the command was a meta-command (command beginning 
         * with '.' i.e. a non sql statement like .exit) 
         * */
        if (input_buffer->buffer[0] == '.') {
            switch ( do_meta_command(input_buffer) ) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command: '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        /* Find out what type of SQL operation the user is requesting */
        Statement statement;
        switch(prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }
        
        /* Execute the SQL statement */
        execute_statement(&statement);
        printf("Executed.\n");
    }

    return 0;
}
