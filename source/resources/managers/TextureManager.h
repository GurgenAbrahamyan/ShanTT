#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>
#include "ResourcePool.h"
#include "TextureHandleTypes.h"
#include "../../resources/assets/Texture.h"
#include "resources/assets/CubeMap.h"
#include "../data/TextureType.h"

class EventBus;

struct TextureKey {
    std::string path;
    bool operator==(const TextureKey& other) const { return path == other.path; }
};
struct TextureKeyHash {
    std::size_t operator()(const TextureKey& key) const { return std::hash<std::string>()(key.path); }
};

struct TextureRecord {
    std::unique_ptr<Texture> texture;
    std::string path;
    TextureType type = TextureType::Albedo;
};

struct CubeMapRecord {
    std::unique_ptr<CubeMap> cubeMap;
    std::string path;
};

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    
    TextureID addTexture    (const std::string& path, const TextureType& type);
    Texture*  getTexture    (TextureID id);
    Texture*  getTexture    (TextureID id) const;
    void      removeTexture (TextureID id);
    TextureID getID         (const std::string& path) const;

    
    CubeMapID addCubeMap    (std::unique_ptr<CubeMap> cubeMap, const std::string& key);
    CubeMap*  getCubeMap    (CubeMapID id);
    CubeMap*  getCubeMap    (CubeMapID id) const;
    void      removeCubeMap (CubeMapID id);
    CubeMapID getCubeMapID  (const std::string& key) const;

    TextureID addGeneratedTexture (const std::string& syntheticKey, std::unique_ptr<Texture> tex, TextureType type);

    size_t getTextureCount() const { return texturePool.size(); }
    bool isSRGB(TextureType type) const;

    
    void initDefaults();
    TextureID getDefaultWhite()  const { return defaultWhite; }
    TextureID getDefaultAlbedo() const { return defaultWhite; }
    TextureID getDefaultNormal() const { return defaultFlatNormal; }
    TextureID getDefaultBlack()  const { return defaultBlack; }

    
    TextureID loadARM(const std::string& aoPath, const std::string& armPath);

    void NextTexture(EventBus* bus);

private:
    std::unique_ptr<Texture> createSinglePixel(unsigned char* data, int channels, GLenum internalFormat);

    ResourcePool<TextureRecord, TextureTag> texturePool;
    ResourcePool<CubeMapRecord, CubeMapTag> cubeMapPool;
    std::unordered_map<TextureKey, TextureID, TextureKeyHash> textureLookup;
    std::unordered_map<TextureKey, CubeMapID, TextureKeyHash> cubeMapLookup;

    TextureID defaultWhite;
    TextureID defaultFlatNormal;
    TextureID defaultBlack;

    uint32_t debugCycleIndex = 0;
};