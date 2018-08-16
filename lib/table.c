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

/* Initialize the table */
Table* new_table() {
    Table* table = malloc(sizeof(Table));
    table->num_rows = 0;

    return table;
}

/* Serialize a row database understandable form */
void serialize_row(Row* source, void* destination) {
    memcpy(destination+ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination+USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination+EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/* Deserialize a row into user readable form */
void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination->id), source+ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source+USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source+EMAIL_OFFSET, EMAIL_SIZE);
}

/* Determine where to read/write in memory for a particular row */
void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = table->pages[page_num];

    if (!page) {
        /* Only allocate memory when we try to access a page */
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }

    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    
    return page + byte_offset;
}

/* Print a row */
void print_row(Row* row) {
    printf("| %d | %s | %s |\n", row->id, row->username, row->email);
}