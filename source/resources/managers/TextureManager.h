#pragma once
#include <../glad/glad.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "../../resources/assets/Texture.h"
#include "../../resources/assets/CubeMap.h"
#include "../data/TextureType.h"
using TextureID = uint32_t;

class EventBus;

struct TextureKey {
    std::string path;
    bool operator==(const TextureKey& other) const {
        return path == other.path;
    }
};

struct TextureKeyHash {
    std::size_t operator()(const TextureKey& key) const {
        return std::hash<std::string>()(key.path);
    }
};

struct TextureRecord {
    std::unique_ptr<Texture> texture;
    std::string path;
    TextureType type;

};

class TextureManager {
public:
    TextureManager();

    TextureID addTexture(const std::string& path,const TextureType& type);
    Texture* getTexture(TextureID id);

    TextureID getID(const std::string& path) const;
    void NextTexture(EventBus* bus);

    size_t getTextureCount() const { return textures.size(); }

    CubeMap* loadCubeMap(std::string filePath);
    CubeMap* loadCubeMapArray(std::vector<std::string> filepaths);
    CubeMap* loadCubeMapDebug();
	bool isSRGB(TextureType type) const;

    // Default texture accessors
    Texture* getDefaultWhite()   const { return defaultWhite.get(); }
    Texture* getDefaultAlbedo()  const { return defaultWhite.get(); }
    Texture* getDefaultNormal()  const { return defaultFlatNormal.get(); }
    Texture* getDefaultBlack()   const { return defaultBlack.get(); }

    Texture* loadARM(const std::string& aoPath, const std::string& armPath);
    void initDefaults();
private:
    std::vector<TextureRecord> textures;
    std::unordered_map<TextureKey, TextureID, TextureKeyHash> lookup;
	std::unique_ptr<CubeMap> cubeMap;

    std::unique_ptr<Texture> defaultWhite;
    std::unique_ptr<Texture> defaultFlatNormal;
    std::unique_ptr<Texture> defaultBlack;

    Texture* createSinglePixel(unsigned char* data, int channels, GLenum internal);

};