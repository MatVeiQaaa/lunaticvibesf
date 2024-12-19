#pragma once

#include "soundset.h"

#include <game/skin/skin.h>

#include <random>
#include <string>
#include <string_view>
#include <vector>

class SoundSetLR2 final : public vSoundSet
{
public:
    SoundSetLR2();
    explicit SoundSetLR2(Path p);
    explicit SoundSetLR2(std::mt19937 gen);
    ~SoundSetLR2() override = default;
    void loadCSV(Path p);
    bool parseHeader(const std::vector<StringContent>& tokens);
    bool parseBody(const std::vector<StringContent>& tokens);

private:
    struct CustomFile
    {
        StringContent title;
        size_t value;
        std::vector<StringContent> label;

        // file
        StringContent filepath;
        size_t defIdx;
    };
    std::vector<CustomFile> customfiles;
    std::vector<size_t> customizeRandom;

    Path filePath;
    Path thumbnailPath;

    std::mt19937 _gen;

    unsigned csvLineNumber = 0; // line parsing index

    std::map<std::string, Path> soundFilePath;
    bool loadPath(const std::string& key, std::string_view rawpath);

public:
    Path getPathBGMSelect() const override;
    Path getPathBGMDecide() const override;

    Path getPathSoundOpenFolder() const override;
    Path getPathSoundCloseFolder() const override;
    Path getPathSoundOpenPanel() const override;
    Path getPathSoundClosePanel() const override;
    Path getPathSoundOptionChange() const override;
    Path getPathSoundDifficultyChange() const override;

    Path getPathSoundScreenshot() const override;

    Path getPathBGMResultClear() const override;
    Path getPathBGMResultFailed() const override;
    Path getPathSoundFailed() const override;
    Path getPathSoundLandmine() const override;
    Path getPathSoundScratch() const override;

    Path getPathBGMCourseClear() const override;
    Path getPathBGMCourseFailed() const override;

public:
    size_t getCustomizeOptionCount() const;
    SkinBase::CustomizeOption getCustomizeOptionInfo(size_t idx) const;
    StringPath getFilePath() const;
    StringPath getThumbnailPath() const;

    // Set in-memory value, without writing to config.
    // This must be called after parsing headers, but before parsing body.
    bool setCustomFileOptionForBodyParsing(std::string_view title, std::string_view value);
};
