#include "assert.h"

#include <common/in_test_mode.h>
#include <common/log.h>
#include <common/sysutil.h>

[[noreturn]] static void die_on_assertion_fail(const char* file, int line, const char* msg)
{
    LOG_FATAL << "die_on_assertion_fail at " << file << ":" << line << ": " << msg;
    panic("Internal error: assertion failed", msg);
}

void lunaticvibes::assert_failed(const char* file, int line, const char* msg)
{
    die_on_assertion_fail(file, line, msg);
}

void lunaticvibes::verify_failed(const char* file, int line, const char* msg)
{
    LOG_FATAL << "Assertion failed at " << file << ":" << line << ": " << msg;
    if (lunaticvibes::in_test_mode())
        die_on_assertion_fail(file, line, msg);
}
