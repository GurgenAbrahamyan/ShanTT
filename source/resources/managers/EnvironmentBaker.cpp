#include "EnvironmentBaker.h"
#include "TextureManager.h"
#include "resources/assets/CubeMap.h"
#include "../../render/backend/Shader.h"
#include "../../math_custom/Mat4.h"
#include "../stb/stb_image.h"
#include <iostream>
#include <cmath>

EnvironmentBaker::EnvironmentBaker() {
    equirectShader = std::make_unique<Shader>(
        "source\\shaders\\texture_generation\\hdr_to_cubemap\\equirect.vert",
        "source\\shaders\\texture_generation\\hdr_to_cubemap\\equirect.frag");

    irradianceShader = std::make_unique<Shader>(
        "source\\shaders\\texture_generation\\cubemap_convultion\\convultion.vert",
        "source\\shaders\\texture_generation\\cubemap_convultion\\convultion.frag");

    prefilterShader = std::make_unique<Shader>(
        "source\\shaders\\texture_generation\\prefilter_convultion\\prefilter.vert",
        "source\\shaders\\texture_generation\\prefilter_convultion\\prefilter.frag");

    brdfShader = std::make_unique<Shader>(
        "source\\shaders\\texture_generation\\brdf\\brdf.vert",
        "source\\shaders\\texture_generation\\brdf\\brdf.frag");

    initGeometry();
}

EnvironmentBaker::~EnvironmentBaker() = default;

void EnvironmentBaker::initGeometry() {
    // Cube
    float cubeVertices[] = {
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

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Quad
    float quadVertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void EnvironmentBaker::renderCube() {
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void EnvironmentBaker::renderQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

EnvironmentMap EnvironmentBaker::loadCubeMapHDR(TextureManager& tm, const std::string& hdrPath) {
    EnvironmentMap result;
    result.env         = equirectToCubemap(tm, hdrPath, hdrPath);
    result.irradiance  = generateIrradiance(tm, result.env, hdrPath);
    result.prefiltered = generatePrefiltered(tm, result.env, hdrPath);
    return result;
}

CubeMapID EnvironmentBaker::equirectToCubemap(TextureManager& tm, const std::string& hdrPath, const std::string& baseKey) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(hdrPath.c_str(), &width, &height, &nrComponents, 0);
    if (!data) {
        std::cerr << "Failed to load HDR image: " << hdrPath << "\n";
        return {};
    }

    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);

    CubeMap envCubemap(512, GL_RGB16F, 1); // constructor allocates all 6 faces

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    Mat4 captureProjection = Mat4::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    Mat4 captureViews[] = {
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
    };

    equirectShader->Activate();
    equirectShader->setInt("equirectangularMap", 0);
    equirectShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, 512, 512);
    for (unsigned int i = 0; i < 6; ++i) {
        equirectShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap.getID(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteTextures(1, &hdrTexture);

    std::cout << "Baked env cubemap: " << hdrPath << "\n";
    return tm.addCubeMap(std::make_unique<CubeMap>(std::move(envCubemap)), baseKey);
}

CubeMapID EnvironmentBaker::generateIrradiance(TextureManager& tm, CubeMapID source, const std::string& baseKey) {
    CubeMap* src = tm.getCubeMap(source);
    if (!src) {
        std::cerr << "generateIrradiance: invalid source cubemap\n";
        return {};
    }

    CubeMap irradianceMap(32, GL_RGB16F, 1);

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    Mat4 captureProjection = Mat4::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    Mat4 captureViews[] = {
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
    };

    irradianceShader->Activate();
    irradianceShader->setInt("environmentMap", 0);
    irradianceShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, src->getID());

    glViewport(0, 0, 32, 32);
    for (unsigned int i = 0; i < 6; ++i) {
        irradianceShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap.getID(), 0);
        glClear(GL_COLOR_BUFFER_BIT);
        renderCube();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);

    std::cout << "Baked irradiance map: " << baseKey << "\n";
    return tm.addCubeMap(std::make_unique<CubeMap>(std::move(irradianceMap)), baseKey + "_irr");
}

CubeMapID EnvironmentBaker::generatePrefiltered(TextureManager& tm, CubeMapID source, const std::string& baseKey) {
    CubeMap* src = tm.getCubeMap(source);
    if (!src) {
        std::cerr << "generatePrefiltered: invalid source cubemap\n";
        return {};
    }

    constexpr unsigned int kBaseSize = 256;
    constexpr unsigned int kMaxMipLevels = 10; 

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    CubeMap prefilterMap(kBaseSize, GL_RGB16F, kMaxMipLevels);

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    Mat4 captureProjection = Mat4::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    Mat4 captureViews[] = {
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
        Mat4::lookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3( 0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
    };

    prefilterShader->Activate();
    prefilterShader->setInt("environmentMap", 0);
    prefilterShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, src->getID());

    for (unsigned int mip = 0; mip < kMaxMipLevels; ++mip) {
        unsigned int mipWidth  = static_cast<unsigned int>(kBaseSize * std::pow(0.5, mip));
        unsigned int mipHeight = mipWidth;
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(kMaxMipLevels - 1);
        prefilterShader->setFloat("roughness", roughness);

        for (unsigned int i = 0; i < 6; ++i) {
            prefilterShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap.getID(), mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);

    std::cout << "Baked prefiltered map: " << baseKey << "\n";
    return tm.addCubeMap(std::make_unique<CubeMap>(std::move(prefilterMap)), baseKey + "_pref");
}

TextureID EnvironmentBaker::getBRDF(TextureManager& tm) {
    if (!cachedBrdfLUT.isValid()) {
        cachedBrdfLUT = generateBRDF(tm);
    }
    return cachedBrdfLUT;
}

TextureID EnvironmentBaker::generateBRDF(TextureManager& tm) {
    TextureDesc desc;
    desc.internalFormat = GL_RG16F;
    desc.format = GL_RG;
    desc.type = GL_FLOAT;
    desc.minFilter = GL_LINEAR;
    desc.magFilter = GL_LINEAR;
    desc.wrapS = GL_CLAMP_TO_EDGE;
    desc.wrapT = GL_CLAMP_TO_EDGE;
    desc.generateMipmaps = false;

    auto brdfTexture = std::make_unique<Texture>(512, 512, nullptr, desc);

    unsigned int captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfTexture->getID(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "BRDF FBO not complete!\n";

    glViewport(0, 0, 512, 512);
    brdfShader->Activate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);

    std::cout << "BRDF LUT generated successfully (512x512)\n";

    return tm.addGeneratedTexture("__brdf_lut", std::move(brdfTexture), TextureType::Albedo);
}