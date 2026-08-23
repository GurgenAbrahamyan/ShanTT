#pragma once
#include <vector>
#include "AnimationClip.h"
#include "resources/managers/SkeletonHandleTypes.h" 
#include "resources/assets/Skeleton/Skeleton.h"

struct SkeletalAnimationTarget
{
    SkeletonID skeleton;
    std::vector<int> trackToBone; 
};

inline SkeletalAnimationTarget BindClipToSkeleton(const AnimationClip& clip,
                                                    SkeletonID skeletonId,
                                                    const Skeleton& skeleton)
{
    SkeletalAnimationTarget target;
    target.skeleton = skeletonId;
    target.trackToBone.assign(clip.tracks.size(), -1);

    for (size_t trackIdx = 0; trackIdx < clip.tracks.size(); ++trackIdx)
    {
        auto it = clip.trackSourceNames.find((int)trackIdx);
        if (it == clip.trackSourceNames.end())
            continue;

        const std::string& boneName = it->second;

        for (size_t boneIdx = 0; boneIdx < skeleton.bones.size(); ++boneIdx)
        {
            if (skeleton.bones[boneIdx].name == boneName)
            {
                target.trackToBone[trackIdx] = (int)boneIdx;
                break;
            }
        }
    }

    return target;
}