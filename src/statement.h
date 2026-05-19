#ifndef STATEMENT_H_
#define STATEMENT_H_

#include "row.h"

typedef enum {
    STAT_TYPE_INVALID = 0,
    STAT_TYPE_INSERT,
    STAT_TYPE_SELECT
} StatementType;

typedef struct {
    Row row;
} InsertStatement;

typedef struct {
    char _;
} SelectStatement;

typedef struct {
    StatementType type;
    union {
        InsertStatement insert;
        SelectStatement select;
    } as;
} Statement;

typedef struct {
    Statement *items;
    size_t     len;
    size_t     size;
} StatementList;


#endif // STATEMENT_H_