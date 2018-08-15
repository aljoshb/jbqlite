#ifndef COMMAND_HEADER_FILE
#define COMMAND_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./codes.h"
#include "./input.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer);

#endif