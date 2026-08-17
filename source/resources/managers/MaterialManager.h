#pragma once
#include <string>
#include <unordered_map>
#include "ResourcePool.h"
#include "MaterialHandleTypes.h"
#include "resources/assets/Material.h"

class MaterialManager {
public:
    MaterialManager() = default;

    MaterialID addMaterial(Material&& material, const std::string& name = "");

    Material* getMaterial(MaterialID id);
    const Material* getMaterial(MaterialID id) const;

    void removeMaterial(MaterialID id);

    MaterialID getID(const std::string& name) const;

    size_t getMaterialCount() const { return pool.size(); }

private:
    struct MaterialRecord {
        Material material;
        std::string name;
    };

    ResourcePool<MaterialRecord, MaterialTag> pool;
    std::unordered_map<std::string, MaterialID> lookup;
};