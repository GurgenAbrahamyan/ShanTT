#include "SkeletonManager.h"

SkeletonID SkeletonManager::addSkeleton(std::unique_ptr<Skeleton> skeleton)
{
    SkeletonRecord record;
    record.skeleton = std::move(skeleton);

    return skeletonPool.insert(std::move(record));
}

Skeleton* SkeletonManager::getSkeleton(SkeletonID id)
{
    SkeletonRecord* record = skeletonPool.get(id);
    return record ? record->skeleton.get() : nullptr;
}

const Skeleton* SkeletonManager::getSkeleton(SkeletonID id) const
{
    const SkeletonRecord* record = skeletonPool.get(id);
    return record ? record->skeleton.get() : nullptr;
}

void SkeletonManager::removeSkeleton(SkeletonID id)
{
    skeletonPool.remove(id);
}