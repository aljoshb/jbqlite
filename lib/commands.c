#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/input.h"
#include "../include/codes.h"

/**
 * Compute the appropriate action when a meta-command
 * (non-SQL command i.e. command starting with '.') 
 * is typed.
 */
MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit")==0) {
        exit(EXIT_SUCCESS);
    }
    else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}