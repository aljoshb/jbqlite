#ifndef TABLE_HEADER_FILE
#define TABLE_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define COLUMN_USERNAME_SIZE          32
#define COLUMN_EMAIL_SIZE             255
#define ID_SIZE                       size_of_attribute(Row, id)
#define USERNAME_SIZE                 size_of_attribute(Row, username)
#define EMAIL_SIZE                    size_of_attribute(Row, email)
#define ID_OFFSET                     0
#define USERNAME_OFFSET               (ID_OFFSET+ID_SIZE)
#define EMAIL_OFFSET                  (USERNAME_OFFSET+USERNAME_SIZE)
#define ROW_SIZE                      (ID_SIZE+USERNAME_SIZE+EMAIL_SIZE)
#define PAGE_SIZE                     4096
#define TABLE_MAX_PAGES               100
#define ROWS_PER_PAGE                 (PAGE_SIZE/ROW_SIZE)
#define TABLE_MAX_ROWS                (ROWS_PER_PAGE*TABLE_MAX_PAGES)

struct Row_t {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
};
typedef struct Row_t Row;

struct Pager_t {
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
};
typedef struct Pager_t Pager;

struct Table_t {
    Pager* pager;
    uint32_t num_rows;
};
typedef struct Table_t Table;

/* Cursor will be used to move around a table */
struct Cursor_t {
    Table* table;
    uint32_t row_num;
    bool end_of_table; // One position past the last element in the table
};
typedef struct Cursor_t Cursor;

/* Function Declarations */
Table* db_open(const char* filename);
void db_close(Table* table);
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* cursor_value(Cursor* cursor);
void print_row(Row* row);
Cursor* table_start(Table* table);
Cursor* table_end(Table* table);
void cursor_advance(Cursor* cursor);

#endif