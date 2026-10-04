#include "SkeletonManager.h"

SkeletonID SkeletonManager::addSkeleton(std::unique_ptr<Skeleton> skeleton, std::string& name)
{
    SkeletonRecord record;
    record.skeleton = std::move(skeleton);
    record.name     = std::move(name);

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

SkeletonID SkeletonManager::instantiate(SkeletonID sourceId)
{
    const Skeleton* source = getSkeleton(sourceId);
    if (!source) return {};

    auto clone = std::make_unique<Skeleton>(*source);
    std::string name = source->name + "_instance";
    return addSkeleton(std::move(clone), name);
}