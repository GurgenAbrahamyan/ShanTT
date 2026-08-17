#include "MaterialManager.h"

MaterialID MaterialManager::addMaterial(Material&& material, const std::string& name)
{
    if (!name.empty())
        if (auto it = lookup.find(name); it != lookup.end())
            return it->second;


    MaterialRecord record{
        .material = std::move(material),
        .name = name
    };

    MaterialID id = pool.insert(std::move(record));

    if (!name.empty())
        lookup[name] = id;
    return id;
}

Material* MaterialManager::getMaterial(MaterialID id)
{
    MaterialRecord* record = pool.get(id);

    if (!record)
        return nullptr;

    return &record->material;

}

const Material* MaterialManager::getMaterial(MaterialID id) const
{
    const MaterialRecord* record = pool.get(id);

    if (!record)
        return nullptr;

    return &record->material;

}

void MaterialManager::removeMaterial(MaterialID id)
{
    MaterialRecord* record = pool.get(id);
    if (!record)
        return;

    if (!record->name.empty()) 
    {
        auto it = lookup.find(record->name);
        if (it != lookup.end() && it->second == id)
            lookup.erase(it);
    }
    pool.remove(id);
}

MaterialID MaterialManager::getID(const std::string& name) const
{
    auto it = lookup.find(name);

    if (it == lookup.end())
        return MaterialID{};

    return it->second;
}