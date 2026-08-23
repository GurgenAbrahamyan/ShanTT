#pragma once

#include <memory>
#include <cstddef>

#include "ResourcePool.h"
#include "SkeletonHandleTypes.h"
#include "resources/assets/Skeleton/Skeleton.h"

class SkeletonManager
{
public:
    SkeletonManager() = default;
    ~SkeletonManager() = default;

    SkeletonID addSkeleton(std::unique_ptr<Skeleton> skeleton, std::string& name);

    Skeleton* getSkeleton(SkeletonID id);
    const Skeleton* getSkeleton(SkeletonID id) const;

    void removeSkeleton(SkeletonID id);

    size_t getSkeletonCount() const
    {
        return skeletonPool.size();
    }

    template<typename Fn>
    void ForEachSkeleton(Fn&& fn) const {
        skeletonPool.forEachAlive([&](Handle<SkeletonTag> h, const SkeletonRecord& rec) { fn(h, rec.name); });
    }

    std::string& GetName(SkeletonID id){
        return skeletonPool.get(id)->name;
    }

    SkeletonID instantiate(SkeletonID sourceId);

private:
    struct SkeletonRecord {
        std::unique_ptr<Skeleton> skeleton;
        std::string name;
    };

    ResourcePool<SkeletonRecord, SkeletonTag> skeletonPool;
};