#pragma once
#include <memory>
#include <string>
#include "TextureHandleTypes.h"
#include "EnvironmentMap.h"
class Shader;
class TextureManager;

class EnvironmentBaker {
public:
    EnvironmentBaker();
    ~EnvironmentBaker();

    EnvironmentMap loadCubeMapHDR(TextureManager& tm, const std::string& hdrPath);
   
    TextureID getBRDF(TextureManager& tm);

private:
    CubeMapID equirectToCubemap   (TextureManager& tm, const std::string& hdrPath, const std::string& baseKey);
    CubeMapID generateIrradiance  (TextureManager& tm, CubeMapID source, const std::string& baseKey);
    CubeMapID generatePrefiltered (TextureManager& tm, CubeMapID source, const std::string& baseKey);
    TextureID generateBRDF        (TextureManager& tm);

    
    void renderCube();
    void renderQuad();
    void initGeometry();

    std::unique_ptr<Shader> equirectShader;
    std::unique_ptr<Shader> irradianceShader;
    std::unique_ptr<Shader> prefilterShader;
    std::unique_ptr<Shader> brdfShader;

    unsigned int cubeVAO = 0, cubeVBO = 0;
    unsigned int quadVAO = 0, quadVBO = 0;

    TextureID cachedBrdfLUT; // set on first getBRDF() call
};