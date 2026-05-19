#include <sv.h>

#include "meta.h"

#define entry(k, v) (MetaCommandMapEntry) {.sv=k, .cmd=v}
#define _SV(str)    SV(str, sizeof(str) - 1)


typedef struct {
    StringView  sv;
    MetaCommand cmd;
} MetaCommandMapEntry;

static MetaCommandMapEntry META_COMMAND_MAP[] = {
    entry(_SV(".exit"), META_CMD_EXIT),
};

MetaCommand metacmd_lookup_command(StringView cmd) {
    size_t len = sizeof(META_COMMAND_MAP)/sizeof(META_COMMAND_MAP[0]);
    for(size_t i = 0; i < len; ++i) {
        MetaCommandMapEntry entry = META_COMMAND_MAP[i];
        if(sv_equal(entry.sv, cmd)) return entry.cmd;
    }
    return META_CMD_UNRECOGNIZED;
}
