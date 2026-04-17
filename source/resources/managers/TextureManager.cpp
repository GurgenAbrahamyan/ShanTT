#include "TextureManager.h"
#include "../stb/stb_image.h"
#include <iostream>
#include "../../core/Event.h"
#include "../../core/EventBus.h"
#include "../../math_custom/Mat4.h"

#include "../../render/backend/Shader.h"
#include "../../render/backend/containers/FrameBuffer.h"
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

    equirectShader = std::make_unique<Shader>(
        "resource\\shaders\\texture_generation\\hdr_to_cubemap\\equirect.vert", 
        "resource\\shaders\\texture_generation\\hdr_to_cubemap\\equirect.frag", "  ");

    irradianceShader = std::make_unique<Shader>(
        "resource\\shaders\\texture_generation\\cubemap_convultion\\convultion.vert",
        "resource\\shaders\\texture_generation\\cubemap_convultion\\convultion.frag", "  ");

    prefilterShader = std::make_unique<Shader>(
        "resource\\shaders\\texture_generation\\prefilter_convultion\\prefilter.vert",
        "resource\\shaders\\texture_generation\\prefilter_convultion\\prefilter.frag", "  ");
    brdfShader = std::make_unique<Shader>(
        "resource\\shaders\\texture_generation\\brdf\\brdf.vert",
        "resource\\shaders\\texture_generation\\brdf\\brdf.frag", "  ");
       
    generateBRDF();

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
    cubeMap->setEnvTexture(texID);

    return cubeMap.get();
}



CubeMap* TextureManager::loadCubeMapHDR(std::string filepath) {
    equirectToCubemap(filepath);
    return cubeMap.get();
}

void TextureManager::renderCube()
{
    static unsigned int VAO = 0;
    static unsigned int VBO = 0;

    if (VAO == 0)
    {
        float vertices[] = {
            -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f,
             1.0f, 1.0f,-1.0f, -1.0f, 1.0f,-1.0f, -1.0f,-1.0f,-1.0f,

            -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f,-1.0f, 1.0f,

            -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f, -1.0f,-1.0f,-1.0f,
            -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

             1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f,
             1.0f,-1.0f,-1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f,

            -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,  1.0f,-1.0f, 1.0f,
             1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f, -1.0f,-1.0f,-1.0f,

            -1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void TextureManager::renderQuad()
{
    static unsigned int VAO = 0;
    static unsigned int VBO = 0;

    if (VAO == 0)
    {
        float vertices[] = {
            
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,

             1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); 
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); 
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void TextureManager::generateBRDF() {

    // --- 1. Create BRDF 2D texture ---
    TextureDesc textureDesc;
    textureDesc.internalFormat = GL_RG16F;  // 16-bit float for accuracy
    textureDesc.format = GL_RG;
    textureDesc.type = GL_FLOAT;
    textureDesc.minFilter = GL_LINEAR;
    textureDesc.magFilter = GL_LINEAR;
    textureDesc.wrapS = GL_CLAMP_TO_EDGE;
    textureDesc.wrapT = GL_CLAMP_TO_EDGE;
    textureDesc.generateMipmaps = false;

   
    brdfTexture = std::make_unique<Texture>(512, 512, nullptr, textureDesc);

 
    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfTexture->getID(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "BRDF FBO not complete!" << std::endl;

    glViewport(0, 0, 512, 512);

   
    brdfShader->Activate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad(); 

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);

    std::cout << "BRDF LUT generated successfully (512x512)\n";

}

void TextureManager::equirectToCubemap(const std::string& hdrPath)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(hdrPath.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
 
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    

  

    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(envCubemap);
    Mat4 captureProjection = Mat4::perspective(90.0f, 1.0f, 0.1f, 10.0f);
     Mat4 captureViews[] =
    {
       Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
       Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f,  0.0f,  0.0f),Vector3(0.0f, -1.0f,  0.0f)),
       Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
       Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
       Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
       Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    equirectShader->Activate();
    equirectShader->setInt("equirectangularMap", 0);
    equirectShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube(); // renders a 1x1 cube
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    cubeMap = std::make_unique<CubeMap>();
    cubeMap->setEnvTexture(envCubemap);


    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
            GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    irradianceShader->Activate();
    irradianceShader->setInt("environmentMap", 0);
    irradianceShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32 ); 
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    cubeMap->setIrrTexture(irradianceMap);


    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 256, 256, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    prefilterShader->Activate();
    prefilterShader->setInt("environmentMap", 0);
    prefilterShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 10;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = 256 * std::pow(0.5, mip);
        unsigned int mipHeight = 256 * std::pow(0.5, mip);
      
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader->setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    cubeMap->setPreFilterTexture(prefilterMap);
}

Texture* TextureManager::getBRDF() {
    if (!brdfTexture)
        generateBRDF();
    return brdfTexture.get();
   
}