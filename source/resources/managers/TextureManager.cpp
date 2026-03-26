#include "TextureManager.h"
#include "../stb/stb_image.h"
#include <iostream>
#include "../../core/Event.h"
#include "../../core/EventBus.h"
#include <memory>


TextureManager::TextureManager() {} // empty, no GL calls

void TextureManager::initDefaults()
{
    unsigned char white[] = { 255, 255, 255 };
    unsigned char flatN[] = { 128, 128, 255 };
    unsigned char black[] = { 0,   0,   0 };

    defaultWhite = std::unique_ptr<Texture>(createSinglePixel(white, 3, GL_SRGB8));
    defaultFlatNormal = std::unique_ptr<Texture>(createSinglePixel(flatN, 3, GL_RGB8));
    defaultBlack = std::unique_ptr<Texture>(createSinglePixel(black, 3, GL_RGB8));

}

Texture* TextureManager::createSinglePixel(unsigned char* data, int channels, GLenum internal)
{
    GLenum format = (channels == 1) ? GL_RED : (channels == 3) ? GL_RGB : GL_RGBA;
    TextureDesc desc;
    desc.internalFormat = internal;
    desc.format = format;
    return new Texture(1, 1, data, desc);
}

TextureID TextureManager::addTexture(const std::string& path,const TextureType& type)
{
    TextureKey key{ path };

    
    if (auto it = lookup.find(key); it != lookup.end()) {
        std::cout << "Texture already loaded: " << path << " (ID: " << it->second << ")\n";
        return it->second;
    }

    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!data)
    {
        std::cerr << "Failed to load texture: " << path << "\n";
        return UINT32_MAX;
    }

    std::cout << "Loading texture: " << path << " (" << width << "x" << height << ", " << channels << " channels)\n";

    // Prevent alignment issues
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Choose correct format based on actual channels
    GLenum format = GL_RGBA;
    GLenum internal = GL_RGBA8;
  

    if (channels == 3)
    {
        format = GL_RGB;
        internal = isSRGB(type) ? GL_SRGB8 : GL_RGB8;
    }
    else if (channels == 4)
    {
        format = GL_RGBA;
        internal = isSRGB(type) ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    }
    else if (channels == 1)
    {
        format = GL_RED;
        internal = GL_R8;
    }
    else
    {
        std::cerr << "Unsupported channel count: " << channels << "\n";
        stbi_image_free(data);
        return UINT32_MAX;
    }


    TextureDesc desc;
    desc.internalFormat = internal;
    desc.format = format;
    

    auto tex = std::make_unique<Texture>(width, height, data, desc);
    stbi_image_free(data);


    TextureID id = static_cast<TextureID>(textures.size());
    textures.push_back({ std::move(tex), path, type });
    lookup[key] = id;

    std::cout << "Texture loaded successfully (ID: " << id << ")\n";
    return id;
}


Texture* TextureManager::loadARM(const std::string& aoPath, const std::string& armPath)
{
    int w = 0, h = 0, ch = 0;
    unsigned char* armData = nullptr;

    stbi_set_flip_vertically_on_load(false);

    // 1. Load ARM if present
    if (!armPath.empty()) {
        armData = stbi_load(armPath.c_str(), &w, &h, &ch, 4); // force RGBA
        if (!armData) {
            std::cerr << "Failed to load ARM texture: " << armPath << "\n";
        }
    }

    // 2. If ARM not loaded, but AO exists, need dimensions
    if (!armData && !aoPath.empty()) {
        int aoW, aoH, aoCh;
        unsigned char* aoDataTmp = stbi_load(aoPath.c_str(), &aoW, &aoH, &aoCh, 1);
        if (!aoDataTmp) {
            std::cerr << "Failed to load AO texture: " << aoPath << "\n";
        }
        else {
            w = aoW;
            h = aoH;
            stbi_image_free(aoDataTmp); // will reload later
        }
    }

    // 3. If still no ARM and no AO, create 1x1 default
    if (!armData && w == 0) w = 1;
    if (!armData && h == 0) h = 1;

    // 4. Allocate ARM buffer
    if (!armData) {
        armData = new unsigned char[w * h * 4];
        for (int i = 0; i < w * h; i++) {
            armData[i * 4 + 0] = 255; // R default
            armData[i * 4 + 1] = 128; // G default (roughness)
            armData[i * 4 + 2] = 0;   // B default (metallic)
            armData[i * 4 + 3] = 255; // A default
        }
    }
    else {
        // Ensure R is at least 255 if AO missing
        for (int i = 0; i < w * h; i++)
            if (armData[i * 4 + 0] == 0) armData[i * 4 + 0] = 255;
    }

    // 5. Bake AO if exists
    if (!aoPath.empty()) {
        int aoW, aoH, aoCh;
        unsigned char* aoData = stbi_load(aoPath.c_str(), &aoW, &aoH, &aoCh, 1);
        if (aoData) {
            if (aoW != w || aoH != h) {
                std::cerr << "AO texture size mismatch, resizing not implemented\n";
            }
            else {
                for (int i = 0; i < w * h; i++)
                    armData[i * 4 + 0] = aoData[i]; // overwrite R with AO
            }
            stbi_image_free(aoData);
            std::cout << "AO baked into ARM R channel: " << aoPath << "\n";
        }
        else {
            std::cerr << "Failed to load AO texture: " << aoPath << "\n";
        }
    }

    TextureDesc desc;
    desc.internalFormat = GL_RGBA8;
    desc.format = GL_RGBA;

    auto tex = std::make_unique<Texture>(w, h, armData, desc);

    // Clean up
    if (!armPath.empty()) stbi_image_free(armData);
    else delete[] armData;

    Texture* raw = tex.get();
    textures.push_back({ std::move(tex), armPath.empty() ? aoPath : armPath, TextureType::ORM });
    lookup[TextureKey{ armPath.empty() ? aoPath : armPath }] = static_cast<TextureID>(textures.size() - 1);

    std::cout << "ARM texture loaded and packed: " << (armPath.empty() ? aoPath : armPath) << "\n";
    return raw;
}

Texture* TextureManager::getTexture(TextureID id) {
    if (id >= textures.size()) {
        std::cerr << "Invalid texture ID: " << id << "\n";
        return nullptr;
    }
    return textures[id].texture.get();
}

TextureID TextureManager::getID(const std::string& path) const {
    TextureKey key{ path };
    if (auto it = lookup.find(key); it != lookup.end()) {
        return it->second;
    }
    return UINT32_MAX;
}

void TextureManager::NextTexture(EventBus* bus) {

}

bool TextureManager::isSRGB(TextureType type) const {

    switch (type) {
    case TextureType::Albedo:
        return true;
    default:
        return false;
    }
}

CubeMap* TextureManager::loadCubeMap(std::string filepath) {
    // TO-DO: fix 
    stbi_set_flip_vertically_on_load(false);

    int w, h, ch;
    unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &ch, 0);
    if (!data) {
        std::cerr << "Failed to load cubemap\n";
        return nullptr;
    }

    int faceSize = w / 4;
    GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;

    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    struct Face { GLenum target; int gx, gy; };
    Face faceMap[6] = {
        { GL_TEXTURE_CUBE_MAP_POSITIVE_X, 2, 1 },
        { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 1 },
        { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 1, 0 },
        { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 1, 2 },
        { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 1, 1 },
        { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 3, 1 },
    };

    // Extract each face into contiguous memory
    std::vector<unsigned char> faceData(faceSize * faceSize * ch);

    for (auto& f : faceMap) {
        // Copy row by row
        for (int y = 0; y < faceSize; y++) {
            unsigned char* srcRow = data + ((f.gy * faceSize + y) * w + f.gx * faceSize) * ch;
            unsigned char* dstRow = faceData.data() + y * faceSize * ch;
            memcpy(dstRow, srcRow, faceSize * ch);
        }

        glTexImage2D(f.target, 0, format, faceSize, faceSize, 0, format, GL_UNSIGNED_BYTE, faceData.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    stbi_image_free(data);

    cubeMap = std::make_unique<CubeMap>();
    cubeMap->setTexture(texID);

    return cubeMap.get();
}
CubeMap* TextureManager::loadCubeMapArray(std::vector<std::string> filepaths) {
    if (filepaths.size() != 6) {
        std::cerr << "CubeMap requires exactly 6 faces, got " << filepaths.size() << std::endl;
        return nullptr;
    }

    stbi_set_flip_vertically_on_load(false);

    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    for (unsigned int i = 0; i < 6; i++) {
        int width, height, channels;
        unsigned char* data = stbi_load(filepaths[i].c_str(), &width, &height, &channels, 0);

        if (!data) {
            std::cerr << "Failed to load cubemap face: " << filepaths[i] << std::endl;
            glDeleteTextures(1, &texID);
            return nullptr;
        }

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        GLenum internalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8; // linear

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            internalFormat,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

        stbi_image_free(data);

        std::cout << "Loaded cubemap face " << i << ": " << filepaths[i] << std::endl;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    cubeMap = std::make_unique<CubeMap>();
    cubeMap->setTexture(texID);

    return cubeMap.get();
}


CubeMap* TextureManager::loadCubeMapDebug() {
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    // Solid colors: RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN
    unsigned char colors[6][3] = {
        {255, 0, 0},   // +X = RED
        {0, 255, 0},   // -X = GREEN
        {0, 0, 255},   // +Y = BLUE
        {255, 255, 0}, // -Y = YELLOW
        {255, 0, 255}, // +Z = MAGENTA
        {0, 255, 255}  // -Z = CYAN
    };

    for (int i = 0; i < 6; i++) {
        unsigned char face[3];
        memcpy(face, colors[i], 3);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, face);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    cubeMap = std::make_unique<CubeMap>();
    cubeMap->setTexture(texID);
    return cubeMap.get();
}
