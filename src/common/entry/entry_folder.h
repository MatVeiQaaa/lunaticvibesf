#pragma once

#include <memory>
#include <vector>

#include "common/hash.h"
#include "entry.h"

class EntryFolderBase : public EntryBase
{
public:
    EntryFolderBase() = delete;
    EntryFolderBase(const HashMD5& md5, StringContentView name = "", StringContentView name2 = "");
    ~EntryFolderBase() override = default;

protected:
    std::vector<std::shared_ptr<EntryBase>> entries;

public:
    virtual std::shared_ptr<EntryBase> getEntry(size_t idx);
    virtual void pushEntry(std::shared_ptr<EntryBase> f);
    virtual size_t getContentsCount() { return entries.size(); }
    virtual bool empty() { return entries.empty(); }
};
