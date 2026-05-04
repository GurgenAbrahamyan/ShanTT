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
#include "../../render/backend/Shader.h"
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

    
    CubeMap* loadCubeMapArray(std::vector<std::string> filepaths);
    
	bool isSRGB(TextureType type) const;

   
    Texture* getDefaultWhite()   const { return defaultWhite.get(); }
    Texture* getDefaultAlbedo()  const { return defaultWhite.get(); }
    Texture* getDefaultNormal()  const { return defaultFlatNormal.get(); }
    Texture* getDefaultBlack()   const { return defaultBlack.get(); }

    Texture* loadARM(const std::string& aoPath, const std::string& armPath);
    void initDefaults();
    CubeMap* loadCubeMapHDR(std::string filepath);
    Texture* getBRDF();

private:

    void generateBRDF();
    void renderCube();
    void renderQuad();
	void  equirectToCubemap(const std::string& hdrPath);
    
    std::vector<TextureRecord> textures;
    std::unordered_map<TextureKey, TextureID, TextureKeyHash> lookup;

	std::unique_ptr<CubeMap> cubeMap;
    std::unique_ptr<Texture> brdfTexture;

    std::unique_ptr<Texture> defaultWhite;
    std::unique_ptr<Texture> defaultFlatNormal;
    std::unique_ptr<Texture> defaultBlack;

    std::unique_ptr<Shader> equirectShader;
    std::unique_ptr<Shader> irradianceShader;
    std::unique_ptr<Shader> prefilterShader;

    std::unique_ptr<Shader> brdfShader;

    std::unique_ptr<Texture> createSinglePixel(unsigned char* data, int channels, GLenum internal);

};