#pragma once
#include "common/coursefile/lr2crs.h"
#include "common/hash.h"
#include "common/types.h"
#include "entry.h"
#include "entry_folder.h"
#include <vector>

class EntryCourse;
class EntryFolderCourse : public EntryFolderBase
{
public:
    EntryFolderCourse() = delete;
    EntryFolderCourse(StringContentView name = "", StringContentView name2 = "")
        : EntryFolderBase(HashMD5{}, name, name2)
    {
        _type = eEntryType::COURSE_FOLDER;
    }
};

// entry for courses
class EntryCourse : public EntryBase
{
public:
    std::vector<HashMD5> charts;
    enum CourseType
    {
        UNDEFINED,
        GRADE,
    } courseType = UNDEFINED;

public:
    EntryCourse() = default;
    ~EntryCourse() override = default;

    EntryCourse(const CourseLr2crs::Course& lr2crs, long long addTime)
    {
        _type = eEntryType::COURSE;
        md5 = lr2crs.getCourseHash();
        _name = lr2crs.title;
        _addTime = addTime;
        if (lr2crs.type == CourseLr2crs::Course::COURSE_GRADE)
        {
            courseType = GRADE;
        }
        charts = lr2crs.chartHash;
    }
};