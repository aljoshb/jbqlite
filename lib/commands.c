#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/input.h"
#include "../include/codes.h"
#include "../include/table.h"
#include "../include/pager.h"

/**
 * Compute the appropriate action when a meta-command
 * (non-SQL command i.e. command starting with '.') 
 * is typed.
 */
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if (strcmp(input_buffer->buffer, ".exit")==0) {
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}