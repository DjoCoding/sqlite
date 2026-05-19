#ifndef ROW_H_
#define ROW_H_

#include <sv.h>

#define ROW_USERNAME_SIZE 32
#define ROW_EMAIL_SIZE    255

typedef struct {
    int id;
    char username[ROW_USERNAME_SIZE];
    char email[ROW_EMAIL_SIZE];
} Row;

Row     row_init(int id, StringView username, StringView email);
size_t  row_serialize(Row self, char *buffer);
Row     row_from_bytes(char *buffer);

#endif // ROW_H_
