#pragma once
#include "scene.h"
#include "scene_context.h"

class ScenePreSelect : public SceneBase
{
public:
    ScenePreSelect();
    ~ScenePreSelect() override;

protected:
    // Looper callbacks
    bool readyToStopAsync() const override;
    void _updateAsync() override;
    std::function<void()> _updateCallback;

    void updateLoadSongs();
    void updateLoadTables();
    void updateLoadCourses();
    void updateUpdateScoreCache();
    void loadFinished();

    bool shouldShowImgui() const override;
    void updateImgui() override;

protected:
    SongListProperties rootFolderProp;
    bool startedLoadSong = false;
    bool startedLoadTable = false;
    bool startedLoadCourse = false;
    bool startedUpdateScoreCache = false;
    std::chrono::system_clock::time_point loadSongTimer;
    std::future<void> loadSongEnd;
    std::future<void> loadTableEnd;
    std::future<void> loadCourseEnd;
    std::future<void> updateScoreCacheEnd;
    int prevChartLoaded = 0;
    std::string textHint;
    std::string textHint2;
    bool loadingFinished = false;

public:
    bool isLoadingFinished() const;
};
