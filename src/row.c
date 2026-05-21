#include <string.h>

#include "types.h"
#include "table.h"

#include "row.h"

Row row_init(int id, StringView username, StringView email) {
    Row row = {0};

    row.id = id;
    sv_to_str(username, row.username, ROW_USERNAME_SIZE);
    sv_to_str(email, row.email, ROW_EMAIL_SIZE);

    return row;
}

Row row_init_from_raw(RawRow raw) {
    return row_init(raw.id, raw.username, raw.email);
}

void row_serialize(Row self, char *buffer) {
    memcpy(buffer, &self, sizeof(Row));
}


Row row_from_bytes(char *buffer) {
    Row self = {0};
    memcpy(&self, buffer, sizeof(Row));
    return self;
}
