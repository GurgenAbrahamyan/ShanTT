#include "ShaderManager.h"
#include "../../core/Event.h"
#include "../../resources/data/ShaderData.h"

ShaderManager::ShaderManager(EventBus* bus)
    : bus(bus)
{
   
    bus->subscribe<InitShader>([this](InitShader& event) {
        std::cout << "[ShaderManager] Initializing shader: " << event.data->name << "\n";
        Shader* shader = this->load(
            event.data->name,
            event.data->vertexPath,
            event.data->fragmentPath,
            event.data->type
        );
        event.result = shader;
        });

 
    bus->subscribe<GetDefaultShader>([this](GetDefaultShader& event) {
        std::cout << "[ShaderManager] Getting default shader \n";
        event.shader = getDefaultShader();
        });
}

Shader* ShaderManager::load(
    const std::string& name,
    const std::string& vertexPath,
    const std::string& fragmentPath,
   
    ShaderType type
) {

    auto it = shaders.find(name);
    if (it != shaders.end()) {
        std::cerr << "[ShaderManager] Shader already loaded: " << name << "\n";
        return it->second.get();
    }

    std::unique_ptr<Shader> shader;

    try {
      
            
       
            shader = std::make_unique<Shader>(vertexPath.c_str(), fragmentPath.c_str());
        
    }
    catch (...) {
        std::cerr << "[ShaderManager] Failed to load shader: " << name << "\n";
        return nullptr;
    }

    Shader* rawPtr = shader.get();
    shaders.emplace(name, std::move(shader));

  
    if (type != ShaderType::UNKNOWN) {
        shadersByType[type].push_back(rawPtr);
        if (currentShaderIndex.find(type) == currentShaderIndex.end()) {
            currentShaderIndex[type] = 0; 
        }
    }

    std::cout << "[ShaderManager] Loaded shader: " << name << "\n";
    return rawPtr;
}

Shader* ShaderManager::getShader(const std::string& name) {
    auto it = shaders.find(name);
    if (it == shaders.end()) {
        std::cerr << "[ShaderManager] Shader not found: " << name << "\n";
        return nullptr;
    }
    return it->second.get();
}

Shader* ShaderManager::getDefaultShader() {
    if (shaders.empty()) return nullptr;
    return shaders.begin()->second.get();
}

Shader* ShaderManager::currentShader(ShaderType type) {
    auto& vec = shadersByType[type];
    if (vec.empty()) return getDefaultShader();
    return vec[currentShaderIndex[type]];
}

Shader* ShaderManager::nextShader(ShaderType type) {
    auto& vec = shadersByType[type];
    if (vec.empty()) return getDefaultShader();

    size_t& idx = currentShaderIndex[type];
    idx = (idx + 1) % vec.size();
    return vec[idx];
}

bool ShaderManager::hasShader(const std::string& name) const {
    return shaders.find(name) != shaders.end();
}

void ShaderManager::clear() {
    shaders.clear();
    shadersByType.clear();
    currentShaderIndex.clear();
    std::cout << "[ShaderManager] Cleared all shaders\n";
}
