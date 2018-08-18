#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#include "../include/codes.h"
#include "../include/input.h"
#include "../include/statements.h"
#include "../include/interface.h"
#include "../include/commands.h"
#include "../include/table.h"
#include "../include/pager.h"

/**
 * Open a connection to the database
 * i.e. open the database file, initialize
 * a pager data structure and initialize a 
 * table data structure.
 */
Table* db_open(const char* filename) {
    
    Pager* pager = pager_open(filename);
    uint32_t num_rows = pager->file_length/ROW_SIZE;

    Table* table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;

    return table;
}

/**
 * db_close is called when a user wants to exit
 * the database and it flushes the page cache to disk,
 * closes the database file and frees the memory for the
 * Pager and Table structures. 
 */
void db_close(Table* table) {

    Pager* pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        printf("about to enter pager_flush\n");
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing database file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void* page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
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
void* cursor_value(Cursor* cursor) {
    uint32_t row_num = cursor->row_num;
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = get_page(cursor->table->pager, page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    
    return page + byte_offset;
}

/* Print a row */
void print_row(Row* row) {
    printf("| %d | %s | %s |\n", row->id, row->username, row->email);
}

Cursor* table_start(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);

    return cursor;
}

Cursor* table_end(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;

    return cursor;
}

void cursor_advance(Cursor* cursor) {
    cursor->row_num += 1;
    if (cursor->row_num >= cursor->table->num_rows) {
        cursor->end_of_table = true;
    }
}
