#ifndef META_H_
#define META_H_

typedef enum {
    META_CMD_EXIT = 0,
    META_CMD_UNRECOGNIZED,
} MetaCommand;

MetaCommand metacmd_lookup_command(StringView cmd);

#endif // META_H_