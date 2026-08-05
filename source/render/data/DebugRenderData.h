#pragma once

#include <vector>
#include <string>


struct DebugTexture {
    std::string name;
    unsigned int textureID {};
};


struct DebugRenderData {
    std::vector<DebugTexture> debugTextures;
};

