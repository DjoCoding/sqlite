#ifndef META_H_
#define META_H_

#include "table.h"

typedef enum {
    META_CMD_EXIT = 0,
    META_CMD_UNRECOGNIZED,
} MetaCommand;

MetaCommand metacmd_lookup_command(StringView cmd);

void metacmd_exit(Table *table);

#endif // META_H_