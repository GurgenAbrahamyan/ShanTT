#include "MeshManager.h"

MeshID MeshManager::addMesh(
    const std::string& name,
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices
)
{
    // Don't create duplicate named meshes.
    if (!name.empty())
    {
        if (auto it = lookup.find(name); it != lookup.end())
        {
            return it->second;
        }
    }

    MeshRecord record{
        std::make_unique<RenderMesh>(vertices, indices),
        name
    };

    MeshID id = pool.insert(std::move(record));

    if (!name.empty())
    {
        lookup[name] = id;
    }

    return id;
}

RenderMesh* MeshManager::getMesh(MeshID id)
{
    MeshRecord* record = pool.get(id);

    if (!record || ! record->mesh)
    {
        return nullptr;
    }

    return record->mesh.get();
}

RenderMesh* MeshManager::getMesh(MeshID id) const 
{
    const MeshRecord* record = pool.get(id);

    if (!record || ! record->mesh)
    {
        return nullptr;
    }

    return record->mesh.get();
}

void MeshManager::removeMesh(MeshID id)
{
    MeshRecord* record = pool.get(id);

    if (!record)
    {
        return;
    }

    if (!record->name.empty())
    {
        auto it = lookup.find(record->name);

        if (it != lookup.end() && it->second == id)
        {
            lookup.erase(it);
        }
    }

    pool.remove(id);
}

MeshID MeshManager::getMeshID(const std::string& name) const
{
    auto it = lookup.find(name);

    if (it == lookup.end())
    {
        return {};
    }

    return it->second;
}

RenderMesh* MeshManager::getRectangleMesh()
{
    constexpr const char* RectangleName = "rectangle";

    // Return the already-created rectangle.
    if (auto it = lookup.find(RectangleName); it != lookup.end())
    {
        return getMesh(it->second);
    }

    constexpr float hw = 0.5f;
    constexpr float hd = 0.5f;
    constexpr float hh = 0.5f;

    std::vector<Vertex> vertices = {
        // Front face (+Y)
        { Vector3(-hw,  hd, -hh), Vector2(0, 0), Vector3(0,  1,  0) },
        { Vector3( hw,  hd, -hh), Vector2(1, 0), Vector3(0,  1,  0) },
        { Vector3( hw,  hd,  hh), Vector2(1, 1), Vector3(0,  1,  0) },
        { Vector3(-hw,  hd,  hh), Vector2(0, 1), Vector3(0,  1,  0) },

        // Back face (-Y)
        { Vector3(-hw, -hd, -hh), Vector2(0, 0), Vector3(0, -1,  0) },
        { Vector3(-hw, -hd,  hh), Vector2(1, 0), Vector3(0, -1,  0) },
        { Vector3( hw, -hd,  hh), Vector2(1, 1), Vector3(0, -1,  0) },
        { Vector3( hw, -hd, -hh), Vector2(0, 1), Vector3(0, -1,  0) },

        // Right face (+X)
        { Vector3( hw, -hd, -hh), Vector2(0, 0), Vector3(1, 0, 0) },
        { Vector3( hw,  hd, -hh), Vector2(1, 0), Vector3(1, 0, 0) },
        { Vector3( hw,  hd,  hh), Vector2(1, 1), Vector3(1, 0, 0) },
        { Vector3( hw, -hd,  hh), Vector2(0, 1), Vector3(1, 0, 0) },

        // Left face (-X)
        { Vector3(-hw, -hd, -hh), Vector2(0, 0), Vector3(-1, 0, 0) },
        { Vector3(-hw, -hd,  hh), Vector2(1, 0), Vector3(-1, 0, 0) },
        { Vector3(-hw,  hd,  hh), Vector2(1, 1), Vector3(-1, 0, 0) },
        { Vector3(-hw,  hd, -hh), Vector2(0, 1), Vector3(-1, 0, 0) },

        // Top face (+Z)
        { Vector3(-hw,  hd,  hh), Vector2(0, 0), Vector3(0, 0, 1) },
        { Vector3( hw,  hd,  hh), Vector2(1, 0), Vector3(0, 0, 1) },
        { Vector3( hw, -hd,  hh), Vector2(1, 1), Vector3(0, 0, 1) },
        { Vector3(-hw, -hd,  hh), Vector2(0, 1), Vector3(0, 0, 1) },

        // Bottom face (-Z)
        { Vector3(-hw,  hd, -hh), Vector2(0, 0), Vector3(0, 0, -1) },
        { Vector3(-hw, -hd, -hh), Vector2(1, 0), Vector3(0, 0, -1) },
        { Vector3( hw, -hd, -hh), Vector2(1, 1), Vector3(0, 0, -1) },
        { Vector3( hw,  hd, -hh), Vector2(0, 1), Vector3(0, 0, -1) },
    };

    std::vector<unsigned int> indices = {
         0,  1,  2,   2,  3,  0,
         4,  5,  6,   6,  7,  4,
         8,  9, 10,  10, 11,  8,
        12, 13, 14,  14, 15, 12,
        16, 17, 18,  18, 19, 16,
        20, 21, 22,  22, 23, 20
    };

    MeshID id = addMesh(RectangleName, vertices, indices);

    return getMesh(id);
}