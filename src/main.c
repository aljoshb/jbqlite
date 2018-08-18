#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/codes.h"
#include "../include/input.h"
#include "../include/statements.h"
#include "../include/interface.h"
#include "../include/commands.h"
#include "../include/table.h"
#include "../include/pager.h"

/* Entry point of the program */
int main(int argc, char const *argv[]) {
    /* Get the database filename */
    if (argc < 2) {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }

    /* Open the database */
    const char* filename = argv[1];
    Table* table = db_open(filename);

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
         */
        if (input_buffer->buffer[0] == '.') {
            switch ( do_meta_command(input_buffer, table) ) {
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
            case (PREPARE_NEGATIVE_ID):
                printf("ID must be positive.\n");
                continue;
            case (PREPARE_STRING_TOO_LONG):
                printf("String is too long.\n");
                continue;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Could not parse statment.\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        /* Execute the SQL statement */
        switch(execute_statement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }
    
    }

    return 0;
}
