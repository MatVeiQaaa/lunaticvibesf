#include "gmock/gmock.h"
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Severity.h>
#include "common/sysutil.h"
#include "config/config_mgr.h"

int main(int argc, char** argv)
{
    executablePath = GetExecutablePath();
    std::filesystem::create_directories("test");
    std::filesystem::current_path("test");

    SetThreadAsMainThread();

    lunaticvibes::InitLogger("apptest.log");
    lunaticvibes::SetLogLevel(lunaticvibes::LogLevel::Verbose);

    ConfigMgr::init();
    ConfigMgr::load();

    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    
    int n = RUN_ALL_TESTS();

    return n;
}