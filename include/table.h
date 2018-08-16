#ifndef TABLE_HEADER_FILE
#define TABLE_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define ID_SIZE size_of_attribute(Row, id)
#define USERNAME_SIZE size_of_attribute(Row, username)
#define EMAIL_SIZE size_of_attribute(Row, email)
#define ID_OFFSET 0
#define USERNAME_OFFSET ID_OFFSET+ID_SIZE
#define EMAIL_OFFSET USERNAME_OFFSET+USERNAME_SIZE
#define ROW_SIZE ID_SIZE+USERNAME_SIZE+EMAIL_SIZE
#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100
#define ROWS_PER_PAGE PAGE_SIZE/ROW_SIZE
#define TABLE_MAX_ROWS ROWS_PER_PAGE*TABLE_MAX_PAGES

struct Row_t {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
};
typedef struct Row_t Row;

struct Table_t {
    void* pages[TABLE_MAX_PAGES];
    uint32_t num_rows;
};
typedef struct Table_t Table;

/* Function Declarations */
Table* new_table();
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* row_slot(Table* table, uint32_t row_num);
void print_row(Row* row);

#endif