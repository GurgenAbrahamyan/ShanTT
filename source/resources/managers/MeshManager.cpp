#include "MeshManager.h"
#include "../assets/StaticMesh.h"
#include "../../core/Event.h"
#include "../../core/EventBus.h"
#include "../data/MeshData.h"
MeshManager::MeshManager(EventBus* bus)
    : bus(bus)
{
    bus->subscribe<InitMesh>([this](InitMesh& event) {

        std::cout << "Initializing mesh: " << event.data->name << "\n";
        MeshID id = this->addMesh(
			
            event.data->name,
            event.data->vertices,
            event.data->indices,
            true
        );
        event.result = getMesh(id);
		});
}
MeshID MeshManager::addMesh(
    const std::string& name,
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    bool dynamic)
{
    if (auto it = nameToID.find(name); it != nameToID.end())
        return it->second;

    MeshID id = nextID++;

    std::unique_ptr<RenderMesh> mesh;
    if (dynamic)
        mesh = std::make_unique<StaticMesh>(vertices, indices);
    else
        mesh = std::make_unique<StaticMesh>(vertices, indices); // StaticMesh later

	mesh->setID(id);

    meshes[id] = std::move(mesh);
    nameToID[name] = id;

    return id;
}

RenderMesh* MeshManager::getMesh(MeshID id) {
    if (auto it = meshes.find(id); it != meshes.end())
        return it->second.get();
    return nullptr;
}

RenderMesh* MeshManager::getRectangleMesh() {
    // already created? return cached
    auto it = nameToID.find("rectangle");
    if (it != nameToID.end())
        return getMesh(it->second);

    float hw = 0.5f;
    float hd = 0.5f;
    float hh = 0.5f;

    std::vector<Vertex> vertices = {
        // Front face (+Y)
        { Vector3(-hw, hd, -hh), Vector3(1,1,1), Vector2(0,0), Vector3(0,1,0) },
        { Vector3(hw, hd, -hh), Vector3(1,1,1), Vector2(1,0), Vector3(0,1,0) },
        { Vector3(hw, hd,  hh), Vector3(1,1,1), Vector2(1,1), Vector3(0,1,0) },
        { Vector3(-hw, hd,  hh), Vector3(1,1,1), Vector2(0,1), Vector3(0,1,0) },
        // Back face (-Y)
        { Vector3(-hw,-hd, -hh), Vector3(1,1,1), Vector2(0,0), Vector3(0,-1,0) },
        { Vector3(-hw,-hd,  hh), Vector3(1,1,1), Vector2(1,0), Vector3(0,-1,0) },
        { Vector3(hw,-hd,  hh), Vector3(1,1,1), Vector2(1,1), Vector3(0,-1,0) },
        { Vector3(hw,-hd, -hh), Vector3(1,1,1), Vector2(0,1), Vector3(0,-1,0) },
        // Right face (+X)
        { Vector3(hw,-hd, -hh), Vector3(1,1,1), Vector2(0,0), Vector3(1,0,0) },
        { Vector3(hw, hd, -hh), Vector3(1,1,1), Vector2(1,0), Vector3(1,0,0) },
        { Vector3(hw, hd,  hh), Vector3(1,1,1), Vector2(1,1), Vector3(1,0,0) },
        { Vector3(hw,-hd,  hh), Vector3(1,1,1), Vector2(0,1), Vector3(1,0,0) },
        // Left face (-X)
        { Vector3(-hw,-hd, -hh), Vector3(1,1,1), Vector2(0,0), Vector3(-1,0,0) },
        { Vector3(-hw,-hd,  hh), Vector3(1,1,1), Vector2(1,0), Vector3(-1,0,0) },
        { Vector3(-hw, hd,  hh), Vector3(1,1,1), Vector2(1,1), Vector3(-1,0,0) },
        { Vector3(-hw, hd, -hh), Vector3(1,1,1), Vector2(0,1), Vector3(-1,0,0) },
        // Top face (+Z)
        { Vector3(-hw, hd,  hh), Vector3(1,1,1), Vector2(0,0), Vector3(0,0,1) },
        { Vector3(hw, hd,  hh), Vector3(1,1,1), Vector2(1,0), Vector3(0,0,1) },
        { Vector3(hw,-hd,  hh), Vector3(1,1,1), Vector2(1,1), Vector3(0,0,1) },
        { Vector3(-hw,-hd,  hh), Vector3(1,1,1), Vector2(0,1), Vector3(0,0,1) },
        // Bottom face (-Z)
        { Vector3(-hw, hd, -hh), Vector3(1,1,1), Vector2(0,0), Vector3(0,0,-1) },
        { Vector3(-hw,-hd, -hh), Vector3(1,1,1), Vector2(1,0), Vector3(0,0,-1) },
        { Vector3(hw,-hd, -hh), Vector3(1,1,1), Vector2(1,1), Vector3(0,0,-1) },
        { Vector3(hw, hd, -hh), Vector3(1,1,1), Vector2(0,1), Vector3(0,0,-1) },
    };

    std::vector<unsigned int> indices = {
        0,  1,  2,   2,  3,  0,
        4,  5,  6,   6,  7,  4,
        8,  9,  10,  10, 11,  8,
        12, 13, 14,  14, 15, 12,
        16, 17, 18,  18, 19, 16,
        20, 21, 22,  22, 23, 20,
    };

    
    MeshID id = addMesh("rectangle", vertices, indices, true);
    RenderMesh* mesh = getMesh(id);
    mesh->setupBuffers();
    return mesh;
}

MeshID MeshManager::getMeshID(const std::string& name) const {
    if (auto it = nameToID.find(name); it != nameToID.end())
        return it->second;
    return -1;
}
