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

    Table* table = malloc(sizeof(Table));
    table->pager = pager;

    if (pager->num_pages == 0) {
        /* This is a new database file, so intialize page 0 as leaf node */
        void* root_node = get_page(pager, 0);
        initialize_leaf_node(root_node);
        set_node_root(root_node, true);
    }

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

    for (uint32_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
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
    uint32_t page_num = cursor->page_num;
    void* page = get_page(cursor->table->pager, page_num);
    
    return leaf_node_value(page, cursor->cell_num);
}

/* Print a row */
void print_row(Row* row) {
    printf("| %d | %s | %s |\n", row->id, row->username, row->email);
}

Cursor* table_start(Table* table) {
    Cursor* cursor = table_find(table, 0);

    void* node = get_page(table->pager, cursor->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    cursor->end_of_table = (num_cells == 0);

    return cursor;
}

Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key) {
    void* node = get_page(table->pager, page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = page_num;

    /* Binary search */
    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;
    while(one_past_max_index != min_index) {
        uint32_t index = (min_index+one_past_max_index)/2;
        uint32_t key_at_index = *leaf_node_key(node, index);
        if (key == key_at_index) {
            cursor->cell_num = index;
            return cursor;
        }
        if (key<key_at_index) {
            one_past_max_index = index;
        }
        else {
            min_index = index+1;
        }
    }

    cursor->cell_num = min_index;
    return cursor;
}

Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key) {
    void* node = get_page(table->pager, page_num);
    uint32_t num_keys = *internal_node_num_keys(node);

    /* Binary search to find index of child to search */
    uint32_t min_index = 0;
    uint32_t max_index = num_keys; // There is one more child than there is key

    while(min_index != max_index) {
        uint32_t index = (min_index+max_index)/2;
        uint32_t key_to_right = *internal_node_key(node, index);
        if (key_to_right >= key) {
            max_index = index;
        }
        else {
            min_index = index +1;
        }
    }

    uint32_t child_num = *internal_node_child(node, min_index);
    void* child = get_page(table->pager, child_num);
    switch (get_node_type(child)) {
        case NODE_LEAF:
            return leaf_node_find(table, child_num, key);
        case NODE_INTERNAL:
            return internal_node_find(table, child_num, key);
    }
}

/* Return the position of the given key.
 * If the key is not present, return the position
 * where it should be inserted.
*/
Cursor* table_find(Table* table, uint32_t key) {
    uint32_t root_page_num = table->root_page_num;
    void* root_node = get_page(table->pager, root_page_num);

    if (get_node_type(root_node) == NODE_LEAF) {
        return leaf_node_find(table, root_page_num, key);
    }
    else {
        return internal_node_find(table, root_page_num, key);
    }
}

void cursor_advance(Cursor* cursor) {
    uint32_t page_num = cursor->page_num;
    void* node = get_page(cursor->table->pager, page_num);
    cursor->cell_num +=1;

    if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
        /* Advance to next leaf node */
        uint32_t next_page_num = *leaf_node_next_leaf(node);
        if (next_page_num == 0) {
            /* The rightmost leaf */
            cursor->end_of_table = true;
        }
        else {
            cursor->page_num = next_page_num;
            cursor->cell_num = 0;
        }
    }
}

uint32_t* leaf_node_num_cells(void* node) {
    return node+LEAF_NODE_NUM_CELLS_OFFSET;
}

void* leaf_node_cell(void* node, uint32_t cell_num) {
    return (char*)node+LEAF_NODE_HEADER_SIZE+(cell_num*LEAF_NODE_CELL_SIZE);
}

uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num);
}

void* leaf_node_value(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num)+LEAF_NODE_KEY_SIZE;
}

void initialize_leaf_node(void* node) {
    set_node_type(node, NODE_LEAF);
    set_node_root(node, false);
    *leaf_node_num_cells(node) = 0;
    *leaf_node_next_leaf(node) = 0; // 0 means no siblings
}

void initialize_internal_node(void* node) {
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
}

uint32_t* leaf_node_next_leaf(void* node) {
    return node+LEAF_NODE_NEXT_LEAF_OFFSET;
}

uint32_t* internal_node_num_keys(void* node) {
    return node+INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t* internal_node_right_child(void* node) {
    return node+INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t* internal_node_cell(void* node, uint32_t cell_num) {
    return node+INTERNAL_NODE_HEADER_SIZE+cell_num*INTERNAL_NODE_CELL_SIZE;
}

uint32_t* internal_node_child(void* node, uint32_t child_num) {
    uint32_t num_keys = *internal_node_num_keys(node);
    if (child_num>num_keys) {
        printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
        exit(EXIT_FAILURE);
    }
    else if (child_num == num_keys) {
        return internal_node_right_child(node);
    }
    else {
        return internal_node_cell(node, child_num);
    }
}

uint32_t* internal_node_key(void* node, uint32_t key_num) {
    return internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

uint32_t get_node_max_key(void* node) {
    switch (get_node_type(node)) {
        case NODE_INTERNAL:
            return *internal_node_key(node, *internal_node_num_keys(node) -1);
        case NODE_LEAF:
            return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
    }
}

bool is_node_root(void* node) {
    uint8_t value = *((uint8_t*)(node+IS_ROOT_OFFSET));
    return (bool)value;
}

void set_node_root(void* node, bool is_root) {
    uint8_t value = is_root;
    *((uint8_t*)(node+IS_ROOT_OFFSET)) = value;
}

/**
 * Until we start recycling free pages, new pages will always
 * go onto the end of the database file
 */
uint32_t get_unused_page_num(Pager* pager) {
    return pager->num_pages;
}

void create_new_root(Table* table, uint32_t right_child_page_num) {

    /*
     * Handle splitting the root. Old root copied to
     * new page, becomes left child. Address of right
     * child passed in. Re-initialize root page to contain
     * the new root node. New root node points to two
     * children.
     */

     void* root = get_page(table->pager, table->root_page_num);
     void* right_child = get_page(table->pager, right_child_page_num);
     uint32_t left_child_page_num = get_unused_page_num(table->pager);
     void* left_child = get_page(table->pager, left_child_page_num);

     /* Left child has data copied from old root */
     memcpy(left_child, root, PAGE_SIZE);
     set_node_root(left_child, false);

     /* Root node is a new internal node with one key and two children */
     initialize_internal_node(root);
     set_node_root(root, true);
     *internal_node_num_keys(root) = 1;
     *internal_node_child(root, 0) = left_child_page_num;
     uint32_t left_child_max_key = get_node_max_key(left_child);
     *internal_node_key(root, 0) = left_child_max_key;
     *internal_node_right_child(root) = right_child_page_num;
}

void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value) {
    
    /* Create a new node and move half the cells over.
     * Insert the new value in one of the two nodes.
     * Update parent or create a new parent.
     */
     void* old_node = get_page(cursor->table->pager, cursor->page_num);
     uint32_t new_page_num = get_unused_page_num(cursor->table->pager);
     void* new_node = get_page(cursor->table->pager, new_page_num);
     initialize_leaf_node(new_node);
     *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
     *leaf_node_next_leaf(old_node) = new_page_num;

     /* All existing keys plus new key should be divided
      * evenly between old (left) and new (right) nodes.
      * Starting from the right, move each key to correct position.
      */
      for (int32_t i=LEAF_NODE_MAX_CELLS; i>=0; i--) {
          void* destination_node;
          if (i>=LEAF_NODE_LEFT_SPLIT_COUNT) {
              destination_node = new_node;
          }
          else {
              destination_node = old_node;
          }
          uint32_t index_within_node = i%LEAF_NODE_LEFT_SPLIT_COUNT;
          void* destination = leaf_node_cell(destination_node, index_within_node);

          if (i == cursor->cell_num) {
              serialize_row(value,leaf_node_value(destination_node, index_within_node));
              *leaf_node_key(destination_node, index_within_node) = key;
          }
          else if (i > cursor->cell_num) {
              memcpy(destination, leaf_node_cell(old_node, i-1), LEAF_NODE_CELL_SIZE);
          }
          else {
              memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
          }
      }

      /* Update cell counts in each node's header */
      *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
      *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

      if (is_node_root(old_node)) {
          return create_new_root(cursor->table, new_page_num);
      }
      else {
          printf("Need to implement updating parent after split\n");
          exit(EXIT_FAILURE);
      }
}

void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = get_page(cursor->table->pager, cursor->page_num);

    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        /* Node is full */
        leaf_node_split_and_insert(cursor, key, value);
        return;
    }

    if (cursor->cell_num < num_cells) {
        /* Make room for a new cell */
        for (uint32_t i=num_cells; i > cursor->cell_num; i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i-1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->cell_num)) = key;
    serialize_row(value, leaf_node_value(node, cursor->cell_num));
}

NodeType get_node_type(void* node) {
    uint8_t value = *((uint8_t*)(node+NODE_TYPE_OFFSET));
    return (NodeType)value;
}

void set_node_type(void* node, NodeType type) {
    uint8_t value = type;
    *((uint8_t*)(node + NODE_TYPE_OFFSET)) = value;
}

void indent(uint32_t level) {
    for (uint32_t i=0; i<level; i++) {
        printf("  ");
    }
}

void print_tree(Pager* pager, uint32_t page_num, uint32_t indentation_level) {
    void* node = get_page(pager, page_num);
    uint32_t num_keys, child;

    switch (get_node_type(node)) {
        case (NODE_LEAF):
            num_keys = *leaf_node_num_cells(node);
            indent(indentation_level);
            printf("- leaf (size %d)\n", num_keys);
            for (uint32_t i=0; i<num_keys; i++) {
                indent(indentation_level+1);
                printf("- %d\n", *leaf_node_key(node, i));
            }
            break;
        case (NODE_INTERNAL):
            num_keys = *internal_node_num_keys(node);
            indent(indentation_level);
            printf("- internal (size %d)\n", num_keys);
            for (uint32_t i = 0; i < num_keys; i++) {
                child = *internal_node_child(node, i);
                print_tree(pager, child, indentation_level + 1);
                indent(indentation_level);
                printf("- key %d\n", *internal_node_key(node, i));
            }
            child = *internal_node_right_child(node);
            print_tree(pager, child, indentation_level + 1);
            break;
    }
}