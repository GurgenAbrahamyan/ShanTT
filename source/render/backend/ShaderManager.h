#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>

#include "Shader.h"
#include "../../core/EventBus.h"
#include "../data/ShaderType.h"


class InitShader;
class GetDefaultShader;



class ShaderManager {
public:
    ShaderManager(EventBus* bus);

    Shader* load(
        const std::string& name,
        const std::string& vertexPath,
        const std::string& fragmentPath,
     
        ShaderType type = ShaderType::UNKNOWN
    );

    Shader* getShader(const std::string& name);
    Shader* getDefaultShader();


    Shader* currentShader(ShaderType type);
    Shader* nextShader(ShaderType type);

    bool hasShader(const std::string& name) const;
    void clear(); // for shutdown / reload

private:
    EventBus* bus;

    // All loaded shaders by name
    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;

    // Map of shaders per type
    std::unordered_map<ShaderType, std::vector<Shader*>> shadersByType;

    // Current index per type for cycling
    std::unordered_map<ShaderType, size_t> currentShaderIndex;

    
   ShaderManager& operator=(const ShaderManager&) = delete;
};
