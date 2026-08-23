#include "AnimationManager.h"
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <iostream>

std::vector<AnimationID> AnimationManager::LoadAll(const std::string& path)
{
    std::string directory = path.substr(0, path.find_last_of('/') + 1);

    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open animation file: " + path);

    json JSON;
    file >> JSON;
    file.close();

    std::vector<AnimationID> results;

    if (!JSON.contains("buffers") || JSON["buffers"].empty())
        return results;

    std::vector<unsigned char> bufferData = loadBufferData(directory, JSON["buffers"][0]["uri"]);

    if (!JSON.contains("animations") || JSON["animations"].empty())
    {
        std::cout << "Warning: no animations found in " << path << "\n";
        return results;
    }

    for (size_t animIdx = 0; animIdx < JSON["animations"].size(); ++animIdx)
    {
        const json& anim = JSON["animations"][animIdx];
        std::string clipName = anim.value("name", "anim_" + std::to_string(animIdx));

        AnimationClip clip = ParseAnimationEntry(JSON, bufferData, anim, path, clipName);
        AnimationID id = pool_.insert(std::move(clip));
        results.push_back(id);

        std::cout << "Loaded animation clip '" << clipName << "' from " << path << "\n";
    }

    return results;
}


AnimationID AnimationManager::Register(AnimationClip clip)
{
    return pool_.insert(std::move(clip));
}

std::vector<unsigned char> AnimationManager::loadBufferData(const std::string& directory, const std::string& uri)
{
    std::ifstream file(directory + uri, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to load animation buffer: " + directory + uri);

    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}


std::vector<float> AnimationManager::getFloats(const nlohmann::json& JSON, const std::vector<unsigned char>& bufferData, const nlohmann::json& accessor)
{
    std::vector<float> floatVec;
    unsigned int bufferViewIndex = accessor["bufferView"];
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];

    const nlohmann::json& bufferView = JSON["bufferViews"][bufferViewIndex];
    unsigned int byteOffset = bufferView.value("byteOffset", 0);

    unsigned int numPerElement;
    if (type == "SCALAR") numPerElement = 1;
    else if (type == "VEC2") numPerElement = 2;
    else if (type == "VEC3") numPerElement = 3;
    else if (type == "VEC4") numPerElement = 4;
    else throw std::invalid_argument("Animation accessor type invalid (not SCALAR/VEC2/VEC3/VEC4)");

    unsigned int start = byteOffset + accByteOffset;
    unsigned int length = count * 4 * numPerElement;

    floatVec.reserve(count * numPerElement);
    for (unsigned int i = start; i < start + length; i += 4)
    {
        float value;
        std::memcpy(&value, &bufferData[i], sizeof(float));
        floatVec.push_back(value);
    }
    return floatVec;
}

AnimationClip AnimationManager::LoadClipFromGltf(const std::string& path)
{
    std::string directory = path.substr(0, path.find_last_of('/') + 1);

    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open animation file: " + path);

    nlohmann::json JSON;
    file >> JSON;
    file.close();

    if (!JSON.contains("buffers") || JSON["buffers"].empty())
        throw std::runtime_error("Animation glTF has no buffers: " + path);

    std::vector<unsigned char> bufferData = loadBufferData(directory, JSON["buffers"][0]["uri"]);

    AnimationClip clip;
    clip.name = path;

    if (!JSON.contains("animations") || JSON["animations"].empty())
    {
        std::cout << "Warning: no animations found in " << path << "\n";
        return clip;
    }

    
    const nlohmann::json& anim = JSON["animations"][0];

    if (!anim.contains("channels") || !anim.contains("samplers"))
        return clip;

    const nlohmann::json& samplers = anim["samplers"];

    std::unordered_map<unsigned int, int> nodeToTrackIdx;

    auto getTrackFor = [&](unsigned int nodeIndex) -> int
    {
        auto it = nodeToTrackIdx.find(nodeIndex);
        if (it != nodeToTrackIdx.end())
            return it->second;

        int idx = clip.AddTrack();
        clip.tracks[idx].AddChannel(ChannelType::Vec3); // 0: translation
        clip.tracks[idx].AddChannel(ChannelType::Quat); // 1: rotation
        clip.tracks[idx].AddChannel(ChannelType::Vec3); // 2: scale

        nodeToTrackIdx[nodeIndex] = idx;

        std::string nodeName = "node_" + std::to_string(nodeIndex);
        if (JSON["nodes"][nodeIndex].contains("name"))
            nodeName = JSON["nodes"][nodeIndex]["name"];
        clip.trackSourceNames[idx] = nodeName;

        return idx;
    };

    for (const auto& ch : anim["channels"])
    {
        const nlohmann::json& target = ch["target"];
        if (!target.contains("node")) continue;

        unsigned int nodeIndex = target["node"];
        std::string path_ = target["path"];

        if (path_ == "weights")
            continue; // morph target weights intentionally unsupported for now

        unsigned int samplerIndex = ch["sampler"];
        const nlohmann::json& sampler = samplers[samplerIndex];

        unsigned int inputAcc  = sampler["input"];
        unsigned int outputAcc = sampler["output"];

        std::vector<float> times = getFloats(JSON, bufferData, JSON["accessors"][inputAcc]);

        std::string interpStr = sampler.value("interpolation", "LINEAR");
        Interpolation interp = interpStr == "STEP" ? Interpolation::Step
                              : interpStr == "CUBICSPLINE" ? Interpolation::CubicSpline
                              : Interpolation::Linear;

        int trackIdx = getTrackFor(nodeIndex);
        std::vector<float> raw = getFloats(JSON, bufferData, JSON["accessors"][outputAcc]);

        if (path_ == "translation" || path_ == "scale")
        {
            std::vector<Vector3> values;
            values.reserve(raw.size() / 3);
            for (size_t i = 0; i + 2 < raw.size(); i += 3)
                values.emplace_back(raw[i], raw[i + 1], raw[i + 2]);

            int channelIdx = (path_ == "translation") ? 0 : 2;
            auto& vc = std::get<Vec3Channel>(clip.tracks[trackIdx].channels[channelIdx]);
            vc.times = times;
            vc.values = std::move(values);
            vc.interp = interp;
        }
        else if (path_ == "rotation")
        {
            std::vector<Quat> values;
            values.reserve(raw.size() / 4);
            for (size_t i = 0; i + 3 < raw.size(); i += 4)
                values.emplace_back(raw[i], raw[i + 1], raw[i + 2], raw[i + 3]); // x,y,z,w — matches ModelLoader's Quat ctor order

            auto& qc = std::get<QuatChannel>(clip.tracks[trackIdx].channels[1]);
            qc.times = times;
            qc.values = std::move(values);
            qc.interp = interp;
        }
    }

    clip.RecalculateDuration();

    std::cout << "Parsed animation clip: " << clip.name
               << " with " << clip.tracks.size() << " tracks, duration "
               << clip.duration << "s\n";

    return clip;
}

AnimationClip AnimationManager::ParseAnimationEntry(const json& JSON, const std::vector<unsigned char>& bufferData,
                                                      const json& anim, const std::string&, const std::string& clipName)
{
    AnimationClip clip;
    clip.name = clipName;

    if (!anim.contains("channels") || !anim.contains("samplers"))
        return clip;

    const json& samplers = anim["samplers"];
    std::unordered_map<unsigned int, int> nodeToTrackIdx;

    auto getTrackFor = [&](unsigned int nodeIndex) -> int
    {
        auto it = nodeToTrackIdx.find(nodeIndex);
        if (it != nodeToTrackIdx.end())
            return it->second;

        int idx = clip.AddTrack();
        clip.tracks[idx].AddChannel(ChannelType::Vec3);
        clip.tracks[idx].AddChannel(ChannelType::Quat);
        clip.tracks[idx].AddChannel(ChannelType::Vec3);

        nodeToTrackIdx[nodeIndex] = idx;

        std::string nodeName = "node_" + std::to_string(nodeIndex);
        if (JSON["nodes"][nodeIndex].contains("name"))
            nodeName = JSON["nodes"][nodeIndex]["name"];
        clip.trackSourceNames[idx] = nodeName;

        return idx;
    };

    for (const auto& ch : anim["channels"])
    {
        const json& target = ch["target"];
        if (!target.contains("node")) continue;

        unsigned int nodeIndex = target["node"];
        std::string path_ = target["path"];
        if (path_ == "weights") continue;

        unsigned int samplerIndex = ch["sampler"];
        const json& sampler = samplers[samplerIndex];

        std::vector<float> times = getFloats(JSON, bufferData, JSON["accessors"][(unsigned int)sampler["input"]]);

        std::string interpStr = sampler.value("interpolation", "LINEAR");
        Interpolation interp = interpStr == "STEP" ? Interpolation::Step
                              : interpStr == "CUBICSPLINE" ? Interpolation::CubicSpline
                              : Interpolation::Linear;

        int trackIdx = getTrackFor(nodeIndex);
        std::vector<float> raw = getFloats(JSON, bufferData, JSON["accessors"][(unsigned int)sampler["output"]]);

        if (path_ == "translation" || path_ == "scale")
        {
            std::vector<Vector3> values;
            values.reserve(raw.size() / 3);
            for (size_t i = 0; i + 2 < raw.size(); i += 3)
                values.emplace_back(raw[i], raw[i + 1], raw[i + 2]);

            int channelIdx = (path_ == "translation") ? 0 : 2;
            auto& vc = std::get<Vec3Channel>(clip.tracks[trackIdx].channels[channelIdx]);
            vc.times = times; vc.values = std::move(values); vc.interp = interp;
        }
        else if (path_ == "rotation")
        {
            std::vector<Quat> values;
            values.reserve(raw.size() / 4);
            for (size_t i = 0; i + 3 < raw.size(); i += 4)
                values.emplace_back(raw[i], raw[i + 1], raw[i + 2], raw[i + 3]);

            auto& qc = std::get<QuatChannel>(clip.tracks[trackIdx].channels[1]);
            qc.times = times; qc.values = std::move(values); qc.interp = interp;
        }
    }

    clip.RecalculateDuration();
    return clip;
}

AnimationID AnimationManager::Load(const std::string& path)
{
    std::string directory = path.substr(0, path.find_last_of('/') + 1);
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Failed to open animation file: " + path);
    json JSON; file >> JSON; file.close();

    std::vector<unsigned char> bufferData = loadBufferData(directory, JSON["buffers"][0]["uri"]);

    if (!JSON.contains("animations") || JSON["animations"].empty())
    {
        std::cout << "Warning: no animations found in " << path << "\n";
        return pool_.insert(AnimationClip{});
    }

    AnimationClip clip = ParseAnimationEntry(JSON, bufferData, JSON["animations"][0], path, path);
    return pool_.insert(std::move(clip));
}