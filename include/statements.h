#ifndef STATEMENTS_HEADER_FILE
#define STATEMENTS_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./input.h"
#include "./table.h"
#include "./codes.h"

enum StatementType_t {
    STATEMENT_INSERT,
    STATEMENT_SELECT
};
typedef enum StatementType_t StatementType;

struct Statement_t {
    StatementType type;
    Row row_to_insert;
};
typedef struct Statement_t Statement;

/* Function Declarations */
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement);
ExecuteResult execute_statement(Statement* statement, Table* table);
ExecuteResult execute_insert(Statement* statement, Table* table);
ExecuteResult execute_select(Statement* statement, Table* table);

#endif