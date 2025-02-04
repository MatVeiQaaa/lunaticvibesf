#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>

#include "common/log.h"
#include "config/cfg_general.h"
#include "config/cfg_input.h"
#include "config/cfg_profile.h"
#include "config/cfg_skin.h"

class ConfigMgr
{
private:
    ConfigMgr() = default;
    ~ConfigMgr() = default;
    static ConfigMgr& getInst()
    {
        static ConfigMgr inst;
        return inst;
    }

    void _init()
    {
        LOG_INFO << "[cfg] Initializing...";

        G = std::make_shared<ConfigGeneral>(GAMEDATA_PATH "/config.yml");
        profileName = G->get(cfg::E_PROFILE, cfg::PROFILE_DEFAULT);
        P = std::make_shared<ConfigProfile>(profileName);
        I5 = std::make_shared<ConfigInput>(profileName, 5);
        I7 = std::make_shared<ConfigInput>(profileName, 7);
        I9 = std::make_shared<ConfigInput>(profileName, 9);
        S = std::make_shared<ConfigSkin>(profileName);

        G->setDefaults();
        P->setDefaults();
        I5->setDefaults();
        I7->setDefaults();
        I9->setDefaults();
        S->setDefaults();

        LOG_INFO << "[cfg] Init finished. ";
    }

    void _load()
    {
        LOG_INFO << "[cfg] Load Profile: " << profileName;
        std::unique_lock l(_mutex);
        G->load();
        P->load();
        I5->load();
        I7->load();
        I9->load();
        S->load();
    }
    void _save()
    {
        LOG_INFO << "[cfg] Save Profile: " << profileName;
        std::shared_lock l(_mutex);
        G->save();
        P->save();
        I5->save();
        I7->save();
        I9->save();
        S->save();
    }
    int _selectProfile(const std::string& name);
    int _createProfile(const std::string& newProfile, const std::string& oldProfile);
    std::string _getProfileName() { return P->getName(); }
    void _setGlobals();

    template <class Ty_v> Ty_v _get(char type, const std::string& key, const Ty_v& fallback)
    {
        std::shared_lock l(_mutex);
        switch (type)
        {
        case 'A':                                                      // Audio
        case 'E':                                                      // Audio
        case 'V': return G ? G->get<Ty_v>(key, fallback) : fallback;   // Video
        case 'P': return P ? P->get<Ty_v>(key, fallback) : fallback;   // Play
        case '5': return I5 ? I5->get<Ty_v>(key, fallback) : fallback; // Input
        case '7': return I7 ? I7->get<Ty_v>(key, fallback) : fallback; // Input
        case '9': return I9 ? I9->get<Ty_v>(key, fallback) : fallback; // Input
        case 'S': return S ? S->get<Ty_v>(key, fallback) : fallback;   // Skin
        }
        return Ty_v();
    }
    template <class Ty_v> void _set(char type, const std::string& key, const Ty_v& value) noexcept
    {
        std::unique_lock l(_mutex);
        switch (type)
        {
        case 'A': // Audio
        case 'E': // Audio
        case 'V':
            if (G)
                return G->set<Ty_v>(key, value);
            break; // Play
        case 'P':
            if (P)
                return P->set<Ty_v>(key, value);
            break; // Play
        case '5':
            if (I5)
                return I5->set<Ty_v>(key, value);
            break; // Input
        case '7':
            if (I7)
                return I7->set<Ty_v>(key, value);
            break; // Input
        case '9':
            if (I9)
                return I9->set<Ty_v>(key, value);
            break; // Input
        case 'S':
            if (S)
                return S->set<Ty_v>(key, value);
            break; // Skin
        }
    }

    KeyMap _getKeyBindings(GameModeKeys mode, Input::Pad key)
    {
        std::shared_lock l(_mutex);
        switch (mode)
        {
        case 5:
            if (I5)
                return I5->getBindings(key);
            break;
        case 7:
            if (I7)
                return I7->getBindings(key);
            break;
        case 9:
            if (I9)
                return I9->getBindings(key);
            break;
        }
        return {};
    }

protected:
    std::shared_ptr<ConfigGeneral> G;
    std::shared_ptr<ConfigProfile> P;
    std::shared_ptr<ConfigInput> I5;
    std::shared_ptr<ConfigInput> I7;
    std::shared_ptr<ConfigInput> I9;
    std::shared_ptr<ConfigSkin> S;
    std::string profileName;
    std::shared_mutex _mutex;

public:
    std::shared_ptr<ConfigGeneral> _General() { return G; }
    std::shared_ptr<ConfigProfile> _Profile() { return P; }
    std::shared_ptr<ConfigSkin> _Skin() { return S; };
    std::shared_ptr<ConfigInput> _Input(GameModeKeys mode)
    {
        switch (mode)
        {
        case 5: return I5;
        case 7: return I7;
        case 9: return I9;
        default: return nullptr;
        }
    }

public:
    static void init() { getInst()._init(); }
    static void load() { getInst()._load(); }
    static void save() { getInst()._save(); }
    static int selectProfile(const std::string& name) { return getInst()._selectProfile(name); }
    static int createProfile(const std::string& newProfile, const std::string& oldProfile)
    {
        return getInst()._createProfile(newProfile, oldProfile);
    }
    static void setGlobals() { getInst()._setGlobals(); }
    static std::shared_ptr<ConfigGeneral> General() { return getInst()._General(); }
    static std::shared_ptr<ConfigProfile> Profile() { return getInst()._Profile(); }
    static std::shared_ptr<ConfigInput> Input(GameModeKeys mode) { return getInst()._Input(mode); }
    static std::shared_ptr<ConfigSkin> Skin() { return getInst()._Skin(); }

    template <class Ty_v> static Ty_v get(char type, const std::string& key, const Ty_v& fallback)
    {
        static_assert(!std::is_same_v<Ty_v, std::string_view>, "string_view isn't supported by YAML-cpp");
        return getInst()._get(type, key, fallback);
    }

    static std::string get(char type, const std::string& key, const std::string& fallback)
    {
        return get<std::string>(type, key, fallback);
    }

    template <class Ty_v> static void set(char type, const std::string& key, const Ty_v& value) noexcept
    {
        static_assert(!std::is_same_v<Ty_v, std::string_view>, "string_view isn't supported by YAML-cpp");
        return getInst()._set(type, key, value);
    }

    static void set(char type, const std::string& key, const std::string& value) noexcept
    {
        return set<std::string>(type, key, value);
    }

public:
};
