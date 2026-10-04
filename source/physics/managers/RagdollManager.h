#pragma once

#include <filesystem>
#include <unordered_map>

#include "RagdollID.h"
#include "physics/data/RagdollData.h"


class RagdollManager
{
public:
    RagdollManager() = default;

    RagdollAssetID Load(const std::filesystem::path& path);

    RagdollAsset* Get(RagdollAssetID id);
    const RagdollAsset* Get(RagdollAssetID id) const;

    void Unload(RagdollAssetID id);
    void Clear();

private:
    RagdollAsset LoadFromFile(
        const std::filesystem::path& path);

private:
    std::vector<RagdollAsset> m_Assets;

    std::unordered_map<std::string, RagdollAssetID> m_PathToID;
};