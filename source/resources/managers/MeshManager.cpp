#include "MeshManager.h"

StaticMeshID MeshManager::addStaticMesh(
    const std::string& name,
    const std::vector<StaticVertex>& vertices,
    const std::vector<unsigned int>& indices)
{
    MeshRecord<StaticVertex> record;
    record.mesh = std::make_unique<RenderMesh<StaticVertex>>(vertices, indices);
    record.name = name;

    StaticMeshID id = staticPool.insert(std::move(record));
    staticLookup[name] = id;
    return id;
}

RenderMesh<StaticVertex>* MeshManager::getStaticMesh(StaticMeshID id)
{
    auto* record = staticPool.get(id);
    return record ? record->mesh.get() : nullptr;
}

RenderMesh<StaticVertex>* MeshManager::getStaticMesh(StaticMeshID id) const
{
    auto* record = staticPool.get(id);
    return record ? record->mesh.get() : nullptr;
}

void MeshManager::removeStaticMesh(StaticMeshID id)
{
    if (auto* record = staticPool.get(id)) {
        staticLookup.erase(record->name);
    }
    staticPool.remove(id);
}

StaticMeshID MeshManager::getStaticMeshID(const std::string& name) const
{
    auto it = staticLookup.find(name);
    return (it != staticLookup.end()) ? it->second : StaticMeshID{};
}

SkinnedMeshID MeshManager::addSkinnedMesh(
    const std::string& name,
    const std::vector<SkinnedVertex>& vertices,
    const std::vector<unsigned int>& indices)
{
    MeshRecord<SkinnedVertex> record;
    record.mesh = std::make_unique<RenderMesh<SkinnedVertex>>(vertices, indices);
    record.name = name;

    SkinnedMeshID id = skinnedPool.insert(std::move(record));
    skinnedLookup[name] = id;
    std::cout << "Added Skinned Mesh: " << name << " With " << vertices.size() 
              << "vertices and " << indices.size() << "indicies \n";
    return id;
}

RenderMesh<SkinnedVertex>* MeshManager::getSkinnedMesh(SkinnedMeshID id)
{
    auto* record = skinnedPool.get(id);
    return record ? record->mesh.get() : nullptr;
}

RenderMesh<SkinnedVertex>* MeshManager::getSkinnedMesh(SkinnedMeshID id) const
{
    auto* record = skinnedPool.get(id);
    return record ? record->mesh.get() : nullptr;
}

void MeshManager::removeSkinnedMesh(SkinnedMeshID id)
{
    if (auto* record = skinnedPool.get(id)) {
        skinnedLookup.erase(record->name);
    }
    skinnedPool.remove(id);
}

SkinnedMeshID MeshManager::getSkinnedMeshID(const std::string& name) const
{
    auto it = skinnedLookup.find(name);
    return (it != skinnedLookup.end()) ? it->second : SkinnedMeshID{};
}