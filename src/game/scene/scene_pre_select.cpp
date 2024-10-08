#include "scene_pre_select.h"

#include <cstdlib>
#include <future>
#include <string_view>
#include <utility>

#include <boost/format.hpp>
#include <imgui.h>

#include "common/sysutil.h"
#include "common/coursefile/lr2crs.h"
#include "common/entry/entry_arena.h"
#include "common/entry/entry_course.h"
#include "common/entry/entry_table.h"
#include "config/config_mgr.h"
#include "game/runtime/i18n.h"
#include "git_version.h"
#include "scene_context.h"

ScenePreSelect::ScenePreSelect(): SceneBase(nullptr, SkinType::PRE_SELECT, 240)
{
    _type = SceneType::PRE_SELECT;

	_updateCallback = std::bind(&ScenePreSelect::updateLoadSongs, this);

    rootFolderProp = SongListProperties{
        {},
        ROOT_FOLDER_HASH,
        "",
        {},
        {},
        0
    };

    graphics_set_maxfps(30);

    LOG_INFO << "[List] ------------------------------------------------------------";

    if (gNextScene == SceneType::PRE_SELECT)
    {
        // score db
        LOG_INFO << "[List] Initializing score.db...";
        g_pScoreDB = std::make_shared<ScoreDB>(ConfigMgr::Profile()->getPath() / "score.db");
        g_pScoreDB->preloadScore();

        // song db
        LOG_INFO << "[List] Initializing song.db...";
        Path dbPath = Path(GAMEDATA_PATH) / "database";
        if (!fs::exists(dbPath)) fs::create_directories(dbPath);
        g_pSongDB = std::make_shared<SongDB>(dbPath / "song.db");

        std::unique_lock l(gSelectContext._mutex);
        gSelectContext.entries.clear();
        gSelectContext.backtrace.clear();

        textHint = i18n::s(i18nText::INITIALIZING);
    }

    LOG_INFO << "[List] ------------------------------------------------------------";
}

ScenePreSelect::~ScenePreSelect()
{
    if (loadSongEnd.valid())
        loadSongEnd.get();
    if (loadTableEnd.valid())
        loadTableEnd.get();
    if (loadCourseEnd.valid())
        loadCourseEnd.get();
}

bool ScenePreSelect::readyToStopAsync() const
{
    auto futureReady = [](const std::future<void>& future) {
        return !future.valid() || future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    };
    return futureReady(loadSongEnd) && futureReady(loadTableEnd) && futureReady(loadCourseEnd) &&
           futureReady(updateScoreCacheEnd);
}

void ScenePreSelect::_updateAsync()
{
    if (gNextScene != SceneType::PRE_SELECT && gNextScene != SceneType::SELECT) return;

    if (gAppIsExiting)
    {
        gNextScene = SceneType::EXIT_TRANS;
        g_pSongDB->stopLoading();
        return;
    }

	_updateCallback();
}

void ScenePreSelect::updateLoadSongs()
{
    if (!startedLoadSong)
    {
        startedLoadSong = true;
        LOG_INFO << "[List] Start loading songs...";

        // wait for Initializing... text to draw
        pushAndWaitMainThreadTask<void>([] {});

        // wait for another frame
        pushAndWaitMainThreadTask<void>([] {});


        // load files
        loadSongEnd = std::async(std::launch::async, [this]() {
            textHint = i18n::s(i18nText::CHECKING_FOLDERS);

            loadSongTimer = std::chrono::system_clock::now();

            // get folders from config
            auto folderList = ConfigMgr::General()->getFoldersPath();
            std::vector<Path> pathList;
            pathList.reserve(folderList.size());
            for (auto& f : folderList)
            {
                pathList.emplace_back(f);
            }

            LOG_INFO << "[List] Refreshing folders...";
            g_pSongDB->initializeFolders(pathList);
            LOG_INFO << "[List] Refreshing folders complete.";

            LOG_INFO << "[List] Building song list cache...";
            g_pSongDB->prepareCache();
            LOG_INFO << "[List] Building song list cache finished.";

            LOG_INFO << "[List] Generating root folders...";
            auto top = g_pSongDB->browse(ROOT_FOLDER_HASH, false);
            if (top && !top->empty())
            {
                for (size_t i = 0; i < top->getContentsCount(); ++i)
                {
                    if (gAppIsExiting) break;
                    auto entry = top->getEntry(i);

                    bool deleted = true;
                    for (auto& f : folderList)
                    {
                        if (gAppIsExiting) break;
                        if (fs::exists(f) && fs::exists(entry->getPath()) && fs::equivalent(f, entry->getPath()))
                        {
                            deleted = false;
                            break;
                        }
                    }
                    if (!deleted)
                    {
                        g_pSongDB->browse(entry->md5, true);
                        rootFolderProp.dbBrowseEntries.emplace_back(std::move(entry), nullptr);
                    }
                }
            }
            if (gAppIsExiting) return;
            LOG_INFO << "[List] Added " << rootFolderProp.dbBrowseEntries.size() << " root folders";

            g_pSongDB->optimize();

            // NEW SONG
            LOG_INFO << "[List] Generating NEW SONG folder...";

            if (auto newSongList = g_pSongDB->findChartFromTime(
                    ROOT_FOLDER_HASH, getFileTimeNow() - State::get(IndexNumber::NEW_ENTRY_SECONDS));
                !newSongList.empty())
            {
                LOG_INFO << "[List] Adding " << newSongList.size() << " entries to NEW SONGS";
                auto entry = std::make_shared<EntryFolderNewSong>("NEW SONGS");
                for (auto&& c : newSongList)
                {
                    if (gAppIsExiting) break;
                    entry->pushEntry(std::make_shared<EntryFolderSong>(std::move(c)));
                }
                rootFolderProp.dbBrowseEntries.insert(rootFolderProp.dbBrowseEntries.begin(), {entry, nullptr});
            } else {
                LOG_INFO << "[List] No NEW SONG entries";
            }

            // ARENA
            LOG_INFO << "[List] Generating ARENA folder...";
            if (!rootFolderProp.dbBrowseEntries.empty())
            {
                auto entry = std::make_shared<EntryFolderArena>(i18n::s(i18nText::ARENA_FOLDER_TITLE), i18n::s(i18nText::ARENA_FOLDER_SUBTITLE));

                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::HOST_LOBBY, i18n::s(i18nText::ARENA_HOST), i18n::s(i18nText::ARENA_HOST_DESCRIPTION)));
                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::JOIN_LOBBY, i18n::s(i18nText::ARENA_JOIN), i18n::s(i18nText::ARENA_JOIN_DESCRIPTION)));
                entry->pushEntry(std::make_shared<EntryArenaCommand>(EntryArenaCommand::Type::LEAVE_LOBBY, i18n::s(i18nText::ARENA_LEAVE), i18n::s(i18nText::ARENA_LEAVE_DESCRIPTION)));

                // TODO load lobby list from file

                rootFolderProp.dbBrowseEntries.emplace_back(std::move(entry), nullptr);
            }
            LOG_INFO << "[List] ARENA has " << 0 << " known hosts (placeholder)";

            });
    }

    if (g_pSongDB->addChartTaskFinishCount != prevChartLoaded)
    {
        std::shared_lock l(g_pSongDB->addCurrentPathMutex);

        prevChartLoaded = g_pSongDB->addChartTaskFinishCount;
        textHint = (
            boost::format(i18n::c(i18nText::LOADING_CHARTS))
                % g_pSongDB->addChartTaskFinishCount
                % g_pSongDB->addChartTaskCount
            ).str();
        textHint2 = g_pSongDB->addCurrentPath;
    }
    
    if (loadSongEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        g_pSongDB->waitLoadingFinish();
        loadSongEnd.get();
        LOG_INFO << "[List] Loading songs complete.";
        LOG_INFO << "[List] ------------------------------------------------------------";

        _updateCallback = std::bind(&ScenePreSelect::updateLoadTables, this);
    }
}

static const std::string_view to_str(const DifficultyTable::UpdateResult result) 
{
    switch (result) {
    case DifficultyTable::UpdateResult::OK: return "OK";
    case DifficultyTable::UpdateResult::INTERNAL_ERROR: return "INTERNAL_ERROR";
    case DifficultyTable::UpdateResult::WEB_PATH_ERROR: return "WEB_PATH_ERROR";
    case DifficultyTable::UpdateResult::WEB_CONNECT_ERR: return "WEB_CONNECT_ERR";
    case DifficultyTable::UpdateResult::WEB_TIMEOUT: return "WEB_TIMEOUT";
    case DifficultyTable::UpdateResult::WEB_PARSE_FAILED: return "WEB_PARSE_FAILED";
    case DifficultyTable::UpdateResult::HEADER_PATH_ERROR: return "HEADER_PATH_ERROR";
    case DifficultyTable::UpdateResult::HEADER_CONNECT_ERR: return "HEADER_CONNECT_ERR";
    case DifficultyTable::UpdateResult::HEADER_TIMEOUT: return "HEADER_TIMEOUT";
    case DifficultyTable::UpdateResult::HEADER_PARSE_FAILED: return "HEADER_PARSE_FAILED";
    case DifficultyTable::UpdateResult::DATA_PATH_ERROR: return "DATA_PATH_ERROR";
    case DifficultyTable::UpdateResult::DATA_CONNECT_ERR: return "DATA_CONNECT_ERR";
    case DifficultyTable::UpdateResult::DATA_TIMEOUT: return "DATA_TIMEOUT";
    case DifficultyTable::UpdateResult::DATA_PARSE_FAILED: return "DATA_PARSE_FAILED";
    }
    abort();
}

void ScenePreSelect::updateLoadTables()
{
    if (!startedLoadTable)
    {
        startedLoadTable = true;
        LOG_INFO << "[List] Start loading tables...";

        loadTableEnd = std::async(std::launch::async, [this]() {
            textHint = i18n::s(i18nText::CHECKING_TABLES);

            // initialize table list
            auto tableList = ConfigMgr::General()->getTablesUrl();
            size_t tableIndex = 0;
            for (auto& tableUrl : tableList)
            {
                if (gAppIsExiting) break;
                LOG_INFO << "[List] Add table " << tableUrl;
                textHint2 = tableUrl;

                gSelectContext.tables.emplace_back();
                DifficultyTableBMS& t = gSelectContext.tables.back();
                t.setUrl(tableUrl);

                auto convertTable = [&](DifficultyTableBMS& t)
                {
                    auto tbl = std::make_shared<EntryFolderTable>(t.getName(), tableIndex);
                    size_t levelIndex = 0;
                    for (const auto& lv : t.getLevelList())
                    {
                        auto tblLevel = std::make_shared<EntryFolderTable>(t.getSymbol() + lv, levelIndex);
                        for (const auto& r : t.getEntryList(lv))
                        {
                            if (gAppIsExiting) break;
                            auto charts = g_pSongDB->findChartByHash(r->md5, false);
                            for (auto& c : charts)
                            {
                                if (fs::exists(c->absolutePath))
                                {
                                    tblLevel->pushEntry(std::make_shared<EntryFolderSong>(c));
                                    break;
                                }
                            }
                        }
                        tbl->pushEntry(std::move(tblLevel));
                        levelIndex += 1;
                    }
                    return tbl;
                };

                textHint = (boost::format(i18n::c(i18nText::LOADING_TABLE)) % t.getUrl()).str();
                textHint2 = "";

                if (t.loadFromFile())
                {
                    // TODO should re-download the table if outdated
                    LOG_INFO << "[List] Local table file found: " << t.getFolderPath();
                    rootFolderProp.dbBrowseEntries.emplace_back(convertTable(t), nullptr);
                }
                else
                {
                    LOG_INFO << "[List] Local file not found. Downloading... " << t.getFolderPath();
                    textHint2 = i18n::s(i18nText::DOWNLOADING_TABLE);

                    t.updateFromUrl([&](DifficultyTable::UpdateResult result)
                        {
                            if (result == DifficultyTable::UpdateResult::OK)
                            {
                                LOG_INFO << "[List] Table file download complete: " << t.getFolderPath();
                                rootFolderProp.dbBrowseEntries.emplace_back(convertTable(t), nullptr);
                            }
                            else
                            {
                                LOG_WARNING << "[List] Update table " << tableUrl << " failed: " << to_str(result);
                            }
                        });
                    tableIndex += 1;
                }
            }

            });
    }

    if (loadTableEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        loadTableEnd.get();
        LOG_INFO << "[List] Loading tables complete.";
        LOG_INFO << "[List] ------------------------------------------------------------";

        _updateCallback = std::bind(&ScenePreSelect::updateLoadCourses, this);
    }
}

void ScenePreSelect::updateLoadCourses()
{
    if (!startedLoadCourse)
    {
        startedLoadCourse = true;
        LOG_INFO << "[List] Start loading courses...";

        loadCourseEnd = std::async(std::launch::async, [this]() {
            textHint = i18n::s(i18nText::LOADING_COURSES);

            std::map<EntryCourse::CourseType, std::vector<std::shared_ptr<EntryCourse>>> courses;

            // initialize table list
            Path coursePath = Path(GAMEDATA_PATH) / "courses";
            if (!fs::exists(coursePath))
                fs::create_directories(coursePath);

            LOG_INFO << "[List] Loading courses from courses/*.lr2crs...";
            for (auto& courseFile : fs::recursive_directory_iterator(coursePath))
            {
                if (!(fs::is_regular_file(courseFile)
                        && lunaticvibes::iequals(courseFile.path().extension().string(), ".lr2crs")))
                    continue;

                const Path& coursePath = courseFile.path();
                LOG_INFO << "[List] Loading course file: " << coursePath;
                textHint2 = coursePath.u8string();

                CourseLr2crs lr2crs(coursePath);
                for (auto& c : lr2crs.courses)
                {
                    auto entry = std::make_shared<EntryCourse>(c, lr2crs.addTime);
                    if (entry->courseType != EntryCourse::UNDEFINED)
                        courses[entry->courseType].push_back(std::move(entry));
                }
            }
            LOG_INFO << "[List] *.lr2crs loading complete.";

            // TODO load courses from tables

            for (auto& [type, courses] : courses)
            {
                if (courses.empty()) continue;

                std::string folderTitle;
                std::string folderTitle2;
                switch (type)
                {
                case EntryCourse::CourseType::UNDEFINED:
                    folderTitle = i18n::s(i18nText::COURSE_TITLE);
                    folderTitle2 = i18n::s(i18nText::COURSE_SUBTITLE);
                    break;
                case EntryCourse::CourseType::GRADE:
                    folderTitle = i18n::s(i18nText::CLASS_TITLE);
                    folderTitle2 = i18n::s(i18nText::CLASS_SUBTITLE);
                    break;
                }

                auto folder = std::make_shared<EntryFolderCourse>(folderTitle, folderTitle2);
                for (auto& c : courses)
                {
                    LOG_INFO << "[List] Add course: " << c->_name;
                    folder->pushEntry(c);
                }
                rootFolderProp.dbBrowseEntries.emplace_back(std::move(folder), nullptr);
            }

            });
    }

    if (loadCourseEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        loadCourseEnd.get();
        LOG_INFO << "[List] Loading courses complete.";
        LOG_INFO << "[List] ------------------------------------------------------------";

        _updateCallback = std::bind(&ScenePreSelect::updateUpdateScoreCache, this);
    }
}

void ScenePreSelect::updateUpdateScoreCache()
{
    if (!startedUpdateScoreCache)
    {
        startedUpdateScoreCache = true;
        LOG_INFO << "[List] Start updating score cache...";

        updateScoreCacheEnd = std::async(std::launch::async, [this]() {
            // TODO: i18n
            textHint = "Updating score cache...";
            textHint2.clear();
            if (g_pScoreDB->isBmsPbCacheEmpty())
                g_pScoreDB->rebuildBmsPbCache();
        });
    }

    if (updateScoreCacheEnd.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        updateScoreCacheEnd.get();
        LOG_INFO << "[List] Finished updating score cache";
        LOG_INFO << "[List] ------------------------------------------------------------";

        _updateCallback = std::bind(&ScenePreSelect::loadFinished, this);
    }
}

void ScenePreSelect::loadFinished()
{
    if (!loadingFinished)
    {
        gSelectContext.backtrace.clear();
        gSelectContext.backtrace.push_front(rootFolderProp);

        if (rootFolderProp.dbBrowseEntries.empty())
        {
            State::set(IndexText::PLAY_TITLE, i18n::s(i18nText::BMS_NOT_FOUND));
            State::set(IndexText::PLAY_ARTIST, i18n::s(i18nText::BMS_NOT_FOUND_HINT));
        }
        if (gNextScene == SceneType::PRE_SELECT)
        {
            textHint = (boost::format("%s %s %s (%s)")
                % PROJECT_NAME % PROJECT_VERSION
#ifndef NDEBUG
                % "Debug"
#else
                % ""
#endif
                % GIT_REVISION
                ).str();
            textHint2 = i18n::s(i18nText::PLEASE_WAIT);
        }

        // wait for updated text to draw
        pushAndWaitMainThreadTask<void>([] {});

        // wait for another frame
        pushAndWaitMainThreadTask<void>([] {});

        int maxFPS = ConfigMgr::get('V', cfg::V_MAXFPS, 480);
        if (maxFPS < 30 && maxFPS != 0)
            maxFPS = 30;
        graphics_set_maxfps(maxFPS);

        gNextScene = SceneType::SELECT;
        loadingFinished = true;
    }
}


void ScenePreSelect::updateImgui()
{
    if (gInCustomize) return;

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(ConfigMgr::get('V', cfg::V_DISPLAY_RES_X, CANVAS_WIDTH)),
                                    static_cast<float>(ConfigMgr::get('V', cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT))),
                             ImGuiCond_Always);
    if (ImGui::Begin("LoadSong", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::TextUnformatted(textHint.c_str());
        ImGui::TextUnformatted(textHint2.c_str());

        ImGui::End();
    }
}


bool ScenePreSelect::isLoadingFinished() const
{
    return loadingFinished;
}
