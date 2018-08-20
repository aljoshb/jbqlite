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
#define NODE_TYPE_SIZE                sizeof(uint8_t)
#define NODE_TYPE_OFFSET              0
#define IS_ROOT_SIZE                  sizeof(uint8_t)
#define IS_ROOT_OFFSET                NODE_TYPE_SIZE
#define PARENT_POINTER_SIZE           sizeof(uint32_t)
#define PARENT_POINTER_OFFSET         (IS_ROOT_OFFSET+IS_ROOT_SIZE)
#define COMMON_NODE_HEADER_SIZE       (NODE_TYPE_SIZE+IS_ROOT_SIZE+PARENT_POINTER_SIZE)
#define LEAF_NODE_NUM_CELLS_SIZE      sizeof(uint32_t)
#define LEAF_NODE_NUM_CELLS_OFFSET    COMMON_NODE_HEADER_SIZE
#define LEAF_NODE_HEADER_SIZE         (COMMON_NODE_HEADER_SIZE+LEAF_NODE_NUM_CELLS_SIZE)
#define LEAF_NODE_KEY_SIZE            sizeof(uint32_t)
#define LEAF_NODE_KEY_OFFSET          0
#define LEAF_NODE_VALUE_SIZE          ROW_SIZE
#define LEAF_NODE_VALUE_OFFSET              (LEAF_NODE_KEY_OFFSET+LEAF_NODE_KEY_SIZE)
#define LEAF_NODE_CELL_SIZE                 (LEAF_NODE_KEY_SIZE+LEAF_NODE_VALUE_SIZE)
#define LEAF_NODE_SPACE_FOR_CELLS           (PAGE_SIZE-LEAF_NODE_HEADER_SIZE)
#define LEAF_NODE_MAX_CELLS                 (LEAF_NODE_SPACE_FOR_CELLS/LEAF_NODE_CELL_SIZE)
#define LEAF_NODE_RIGHT_SPLIT_COUNT         ((LEAF_NODE_MAX_CELLS+1)/2)
#define LEAF_NODE_LEFT_SPLIT_COUNT          (LEAF_NODE_MAX_CELLS+1 - LEAF_NODE_RIGHT_SPLIT_COUNT)
#define INTERNAL_NODE_NUM_KEYS_SIZE         sizeof(uint32_t)
#define INTERNAL_NODE_NUM_KEYS_OFFSET       COMMON_NODE_HEADER_SIZE
#define INTERNAL_NODE_RIGHT_CHILD_SIZE      sizeof(uint32_t)
#define INTERNAL_NODE_RIGHT_CHILD_OFFSET    (INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE)
#define INTERNAL_NODE_HEADER_SIZE           (COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE)
#define INTERNAL_NODE_KEY_SIZE              sizeof(uint32_t)
#define INTERNAL_NODE_CHILD_SIZE            sizeof(uint32_t)
#define INTERNAL_NODE_CELL_SIZE             (INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE)

struct Row_t {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
};
typedef struct Row_t Row;

struct Pager_t {
    int file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;
    void* pages[TABLE_MAX_PAGES];
};
typedef struct Pager_t Pager;

struct Table_t {
    Pager* pager;
    uint32_t root_page_num;
};
typedef struct Table_t Table;

/* Cursor will be used to move around a table */
struct Cursor_t {
    Table* table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table; // One position past the last element in the table
};
typedef struct Cursor_t Cursor;

/* Keep track of the type of node: leaf or internal */
enum NodeType_t {
    NODE_INTERNAL,
    NODE_LEAF
};
typedef enum NodeType_t NodeType;

/* Function Declarations */
Table* db_open(const char* filename);
void db_close(Table* table);
void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* cursor_value(Cursor* cursor);
void print_row(Row* row);
bool is_node_root(void* node);
void set_node_root(void* node, bool is_root);
Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key);
uint32_t* internal_node_num_keys(void* node);
uint32_t* internal_node_right_child(void* node);
uint32_t* internal_node_cell(void* node, uint32_t cell_num);
uint32_t* internal_node_child(void* node, uint32_t child_num);
uint32_t* internal_node_key(void* node, uint32_t key_num);
uint32_t get_unused_page_num(Pager* pager);
uint32_t get_node_max_key(void* node);
void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);
void create_new_root(Table* table, uint32_t right_child_page_num);
Cursor* table_start(Table* table);
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);
Cursor* table_find(Table* table, uint32_t key);
NodeType get_node_type(void* node);
void set_node_type(void* node, NodeType type);
uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
void initialize_leaf_node(void* node);
void initialize_internal_node(void* node);
void cursor_advance(Cursor* cursor);
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);
void indent(uint32_t level);
void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level);

#endif