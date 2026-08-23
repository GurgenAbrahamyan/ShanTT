#pragma once
#include "AnimationClip.h"
#include "resources/managers/ResourcePool.h"
#include "resources/managers/ResourcePool.h"
#include "AnimationHandleTypes.h"

#include <string>
#include <json.h>

class AnimationManager
{
    using json = nlohmann::json;
public:

    std::vector<AnimationID> LoadAll(const std::string& path);
    AnimationID Load(const std::string& path);
    AnimationID Register(AnimationClip clip);

    AnimationClip& Get(AnimationID h)             { return *pool_.get(h); }
    const AnimationClip& Get(AnimationID h) const { return *pool_.get(h); }

    void Unload(AnimationID h) { pool_.remove(h); }

private:

    ResourcePool<AnimationClip, AnimationTag> pool_;

    AnimationClip LoadClipFromGltf(const std::string& path);

    std::vector<unsigned char> loadBufferData(const std::string& directory, const std::string& uri);
    std::vector<float> getFloats(const json& JSON, const std::vector<unsigned char>& bufferData, const json& accessor);

   
    AnimationClip ParseAnimationEntry(const json& JSON, const std::vector<unsigned char>& bufferData,
                                       const json& anim, const std::string& sourcePath, const std::string& clipName);

};