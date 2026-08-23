#include "SkeletonAnimationSystem.h"
#include "../AnimationState.h"
#include "ecs/components/core/SkeletonAnimationTarget.h"
#include "../AnimationManager.h"
#include "resources/managers/SkeletonManager.h"
#include "../AnimationSampler.h" 

#include "scene/SceneContext.h"
#include <cmath>

void SkeletonAnimationSystem::Update(SceneContext& ctx, float dt)
{
    AnimationManager& animMgr = ctx.engine.assets.animations();
    SkeletonManager& skelMgr  = ctx.engine.assets.skeletons();

    auto view = registry.view<AnimationState, SkeletalAnimationTarget>();

    for (auto [entity, state, target] : view.each())
    {
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
                if (state.time < 0.0f) state.time += clip.duration;
            }
            else
            {
                if (state.time >= clip.duration)
                {
                    state.time = clip.duration;
                    state.playing = false;
                }
            }
        }

        Skeleton& skeleton = *skelMgr.getSkeleton(target.skeleton);

        for (size_t t = 0; t < clip.tracks.size(); ++t)
        {
            int jointIdx = target.trackToJoint[t];
            if (jointIdx < 0 || jointIdx >= (int)skeleton.bones.size())
                continue; 

            std::vector<SampledValue> sampled =
                SampleTrack(clip.tracks[t], state.time, state.cacheIndices[t]);

            Bone& local = skeleton.bones[jointIdx];

            if (sampled.size() > 0)
                local.pos = std::get<Vector3>(sampled[0]);
            if (sampled.size() > 1)
                local.rot = std::get<Quat>(sampled[1]);
            if (sampled.size() > 2)
                local.scale = std::get<Vector3>(sampled[2]);
        }
    }
}