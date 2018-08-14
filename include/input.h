#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* InputBuffer will store the input text the user types in the command prompt */
typedef struct InputBuffer_t {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

/* Function to allocations */
InputBuffer* new_input_buffer();
void read_input(InputBuffer* input_buffer);