#include "assert.h"

#include <common/log.h>
#include <common/sysutil.h>

void lunaticvibes::die_on_assertion_fail(const char* file, int line, const char* msg)
{
    LOG_ERROR << "die_on_assertion_fail at " << file << ":" << line << ": " << msg;
    panic("Internal error", msg);
}
