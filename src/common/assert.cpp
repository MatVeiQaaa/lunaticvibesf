#include "assert.h"

#include <common/in_test_mode.h>
#include <common/log.h>
#include <common/sysutil.h>

[[noreturn]] static void die_on_assertion_fail(const char* file, int line, const char* msg)
{
    panic("Internal error: assertion failed", msg);
}

void lunaticvibes::assert_failed(const char* msg, std::source_location loc)
{
    LOG_FATAL << "Fatal assertion failed at " << loc.file_name() << ":" << loc.line() << ": " << msg;
    die_on_assertion_fail(loc.file_name(), loc.line(), msg);
}

void lunaticvibes::verify_failed(const char* msg, std::source_location loc)
{
    LOG_FATAL << "Assertion failed at " << loc.file_name() << ":" << loc.line() << ": " << msg;
    if (lunaticvibes::in_test_mode())
        die_on_assertion_fail(loc.file_name(), loc.line(), msg);
}
