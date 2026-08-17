#include "TextureManager.h"
#include "../../core/EventBus.h" // adjust path to your actual EventBus header
#include "../stb/stb_image.h"
#include <iostream>

TextureManager::TextureManager() {}

TextureManager::~TextureManager() = default;

TextureID TextureManager::addTexture(const std::string& path, const TextureType& type) {
    TextureKey key{ path };
    auto it = textureLookup.find(key);
    if (it != textureLookup.end()) {
        std::cout << "Texture already loaded: " << path << "\n";
        return it->second;
    }

    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << "\n";
        return TextureID{}; // caller should fall back to a default (e.g. getDefaultWhite())
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    TextureDesc desc;
    if (channels == 1) {
        desc.internalFormat = GL_R8;
        desc.format = GL_RED;
    } else if (channels == 3) {
        desc.internalFormat = isSRGB(type) ? GL_SRGB8 : GL_RGB8;
        desc.format = GL_RGB;
    } else if (channels == 4) {
        desc.internalFormat = isSRGB(type) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        desc.format = GL_RGBA;
    } else {
        std::cerr << "Unsupported channel count: " << channels << "\n";
        stbi_image_free(data);
        return TextureID{};
    }

    auto texture = std::make_unique<Texture>(width, height, data, desc);
    stbi_image_free(data);

    TextureRecord record{ std::move(texture), path, type };
    TextureID id = texturePool.insert(std::move(record));
    textureLookup[key] = id;
    return id;
}

Texture* TextureManager::getTexture(TextureID id) {
    TextureRecord* record = texturePool.get(id);
    return record ? record->texture.get() : nullptr;
}

Texture* TextureManager::getTexture(TextureID id) const {
    const TextureRecord* record = texturePool.get(id);
    return record ? record->texture.get() : nullptr;
}

void TextureManager::removeTexture(TextureID id) {
    if (TextureRecord* record = texturePool.get(id)) {
        textureLookup.erase(TextureKey{ record->path });
        texturePool.remove(id);
    }
}

TextureID TextureManager::getID(const std::string& path) const {
    auto it = textureLookup.find(TextureKey{ path });
    return it != textureLookup.end() ? it->second : TextureID{};
}


CubeMapID TextureManager::addCubeMap(std::unique_ptr<CubeMap> cubeMap, const std::string& key) {
    CubeMapRecord record{ std::move(cubeMap), key };
    CubeMapID id = cubeMapPool.insert(std::move(record));
    cubeMapLookup[TextureKey{ key }] = id;
    return id;
}

CubeMap* TextureManager::getCubeMap(CubeMapID id)  { 
    CubeMapRecord* record = cubeMapPool.get(id);
    return record ? record->cubeMap.get() : nullptr;
}

CubeMap* TextureManager::getCubeMap(CubeMapID id) const { 
    const CubeMapRecord* record = cubeMapPool.get(id);
    return record ? record->cubeMap.get() : nullptr;
}

void TextureManager::removeCubeMap(CubeMapID id) {
    if (CubeMapRecord* record = cubeMapPool.get(id)) {
        cubeMapLookup.erase(TextureKey{ record->path });
        cubeMapPool.remove(id);
    }
}

CubeMapID TextureManager::getCubeMapID(const std::string& key) const {
    auto it = cubeMapLookup.find(TextureKey{ key });
    return it != cubeMapLookup.end() ? it->second : CubeMapID{};
}


TextureID TextureManager::addGeneratedTexture(const std::string& syntheticKey, std::unique_ptr<Texture> tex, TextureType type) {
    TextureRecord record{ std::move(tex), syntheticKey, type };
    TextureID id = texturePool.insert(std::move(record));
    textureLookup[TextureKey{ syntheticKey }] = id;
    return id;
}

bool TextureManager::isSRGB(TextureType type) const {
    return type == TextureType::Albedo;
}

std::unique_ptr<Texture> TextureManager::createSinglePixel(unsigned char* data, int channels, GLenum internalFormat) {
    TextureDesc desc;
    desc.internalFormat = internalFormat;
    desc.format = (channels == 1) ? GL_RED : (channels == 3 ? GL_RGB : GL_RGBA);
    desc.generateMipmaps = false;
    desc.minFilter = GL_NEAREST;
    desc.magFilter = GL_NEAREST;
    return std::make_unique<Texture>(1, 1, data, desc);
}

void TextureManager::initDefaults() {
    unsigned char white[4] = { 255, 255, 255, 255 };
    unsigned char black[4] = { 0, 0, 0, 255 };
    unsigned char flatNormal[4] = { 128, 128, 255, 255 };

    defaultWhite      = addGeneratedTexture("__default_white",  createSinglePixel(white, 4, GL_RGBA8), TextureType::Albedo);
    defaultBlack      = addGeneratedTexture("__default_black",  createSinglePixel(black, 4, GL_RGBA8), TextureType::Albedo);
    defaultFlatNormal = addGeneratedTexture("__default_normal", createSinglePixel(flatNormal, 4, GL_RGBA8), TextureType::Normal);
}


TextureID TextureManager::loadARM(const std::string& aoPath, const std::string& armPath) {
    int w = 0, h = 0, ch = 0;
    unsigned char* armData = nullptr;
    bool fromSTBI = false;

    stbi_set_flip_vertically_on_load(false);

    if (!armPath.empty()) {
        armData = stbi_load(armPath.c_str(), &w, &h, &ch, 4);
        if (!armData) std::cerr << "Failed to load ARM texture: " << armPath << "\n";
        else fromSTBI = true;
    }

    if (!armData && !aoPath.empty()) {
        int aoW, aoH, aoCh;
        unsigned char* aoDataTmp = stbi_load(aoPath.c_str(), &aoW, &aoH, &aoCh, 1);
        if (!aoDataTmp) {
            std::cerr << "Failed to load AO texture: " << aoPath << "\n";
        } else {
            w = aoW; h = aoH;
            stbi_image_free(aoDataTmp);
        }
    }

    if (!armData && w == 0) w = 1;
    if (!armData && h == 0) h = 1;

    if (!armData) {
        armData = new unsigned char[w * h * 4];
        for (int i = 0; i < w * h; i++) {
            armData[i * 4 + 0] = 255;
            armData[i * 4 + 1] = 128;
            armData[i * 4 + 2] = 0;
            armData[i * 4 + 3] = 255;
        }
        fromSTBI = false;
    } else {
        for (int i = 0; i < w * h; i++)
            if (armData[i * 4 + 0] == 0) armData[i * 4 + 0] = 255;
    }

    if (!aoPath.empty()) {
        int aoW, aoH, aoCh;
        unsigned char* aoData = stbi_load(aoPath.c_str(), &aoW, &aoH, &aoCh, 1);
        if (aoData) {
            if (aoW != w || aoH != h) {
                std::cerr << "AO texture size mismatch, resizing not implemented\n";
            } else {
                for (int i = 0; i < w * h; i++)
                    armData[i * 4 + 0] = aoData[i];
            }
            stbi_image_free(aoData);
        } else {
            std::cerr << "Failed to load AO texture: " << aoPath << "\n";
        }
    }

    TextureDesc desc;
    desc.internalFormat = GL_RGBA8;
    desc.format = GL_RGBA;
    auto tex = std::make_unique<Texture>(w, h, armData, desc);

    if (fromSTBI) stbi_image_free(armData);
    else delete[] armData;

    std::string key = armPath.empty() ? aoPath : armPath;
    return addGeneratedTexture(key, std::move(tex), TextureType::ORM);
}

void TextureManager::NextTexture(EventBus*) {
    uint32_t target = debugCycleIndex;
    uint32_t count = 0;
    texturePool.forEachAlive([&](TextureID, TextureRecord&) {
        if (count == target) {
            // TODO
        }
        count++;
    });
    debugCycleIndex = (count > 0) ? (debugCycleIndex + 1) % count : 0;
}