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

    SkeletonID addSkeleton(std::unique_ptr<Skeleton> skeleton);

    Skeleton* getSkeleton(SkeletonID id);
    const Skeleton* getSkeleton(SkeletonID id) const;

    void removeSkeleton(SkeletonID id);

    size_t getSkeletonCount() const
    {
        return skeletonPool.size();
    }

private:
    struct SkeletonRecord
    {
        std::unique_ptr<Skeleton> skeleton;
    };

    ResourcePool<SkeletonRecord, SkeletonTag> skeletonPool;
};