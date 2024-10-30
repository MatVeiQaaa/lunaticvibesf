#pragma once
#include "scene.h"
#include <mutex>

enum class eDecideState
{
    START,
    SKIP,
    CANCEL,
};

class SceneDecide : public SceneBase
{
public:
    explicit SceneDecide(const std::shared_ptr<SkinMgr>& skinMgr);
    ~SceneDecide() override;

protected:
    // Looper callbacks
    void _updateAsync() override;
    std::function<void()> _updateCallback;
    void updateStart();
    void updateSkip();
    void updateCancel();

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const lunaticvibes::Time&);
    void inputGameHold(InputMask&, const lunaticvibes::Time&);
    void inputGameRelease(InputMask&, const lunaticvibes::Time&);

private:
    eDecideState state;
    InputMask _inputAvailable;
};