#include <string.h>
#include "table.h"

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

size_t row_serialize(Row self, char *buffer) {
    size_t offset = 0;

    memcpy(buffer + offset, &self.id, sizeof(self.id));
    offset += sizeof(self.id);

    memcpy(buffer + offset, self.username, sizeof(self.username));
    offset += sizeof(self.username);

    memcpy(buffer + offset, self.email, sizeof(self.email));
    offset += sizeof(self.email);

    return offset;
}


Row row_from_bytes(char *buffer) {
    size_t offset = 0;

    int id = 0;
    memcpy(&id, buffer + offset, sizeof(id));
    offset += sizeof(id);

    char username[ROW_USERNAME_SIZE] = {0};
    memcpy(username, buffer + offset, sizeof(username));
    offset += sizeof(username);

    char email[ROW_EMAIL_SIZE] = {0};
    memcpy(email, buffer + offset, sizeof(email));
    offset += sizeof(email);

    return row_init(id, sv_from_cstr(username), sv_from_cstr(email));
}
