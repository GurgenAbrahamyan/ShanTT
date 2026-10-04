#include "SkeletonAnimationPoseSystem.h"

#include "../AnimationState.h"
#include "../AnimationManager.h"
#include "../AnimationSampler.h"

#include "ecs/components/core/SkeletonAnimationTarget.h"
#include "../AnimationPoseComponent.h"

#include "resources/managers/SkeletonManager.h"
#include "scene/SceneContext.h"

#include <cmath>

void SkeletonAnimationPoseSystem::Update(SceneContext& ctx, float dt)
{
    AnimationManager& animMgr = ctx.engine.assets.animations();
    SkeletonManager& skelMgr  = ctx.engine.assets.skeletons();

    auto view = registry.view<
        AnimationState,
        SkeletalAnimationTarget,
        AnimatedPoseComponent
    >();

    for (auto [entity, state, target, pose] : view.each())
    {
        Skeleton& skeleton = *skelMgr.getSkeleton(target.skeleton);

        const size_t boneCount = skeleton.bones.size();

        if (pose.localPositions.size() != boneCount ||
            pose.localRotations.size() != boneCount ||
            pose.localScales.size() != boneCount)
        {
            pose.resize(boneCount);
        }

        // Start from the skeleton's bind/default local pose.
        for (size_t i = 0; i < boneCount; ++i)
        {
            const Bone& bone = skeleton.bones[i];

            pose.localPositions[i] = bone.pos;
            pose.localRotations[i] = bone.rot;
            pose.localScales[i]    = bone.scale;
        }

        if (!state.playing)
            continue;

        const AnimationClip& clip = animMgr.Get(state.clip);

        if (state.lastSeenClipVersion != clip.version)
        {
            state.cacheIndices.clear();
            state.lastSeenClipVersion = clip.version;
        }

        if (state.cacheIndices.size() != clip.tracks.size())
            state.cacheIndices.resize(clip.tracks.size());

        state.time += dt * state.speed;

        if (clip.duration > 0.0f)
        {
            if (state.looping)
            {
                state.time = std::fmod(state.time, clip.duration);

                if (state.time < 0.0f)
                    state.time += clip.duration;
            }
            else if (state.time >= clip.duration)
            {
                state.time = clip.duration;
                state.playing = false;
            }
        }

        for (size_t t = 0; t < clip.tracks.size(); ++t)
        {
            const int jointIdx = target.trackToJoint[t];

            if (jointIdx < 0 ||
                jointIdx >= static_cast<int>(boneCount))
            {
                continue;
            }

            const std::vector<SampledValue> sampled =
                SampleTrack(
                    clip.tracks[t],
                    state.time,
                    state.cacheIndices[t]
                );

            if (sampled.size() > 0)
                pose.localPositions[jointIdx] =
                    std::get<Vector3>(sampled[0]);

            if (sampled.size() > 1)
                pose.localRotations[jointIdx] =
                    std::get<Quat>(sampled[1]);

            if (sampled.size() > 2)
                pose.localScales[jointIdx] =
                    std::get<Vector3>(sampled[2]);
        }
    }
}