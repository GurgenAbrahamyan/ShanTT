#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

#include "../assets/RenderMesh.h"

#include "../data/Vertex.h"

class EventBus;
using MeshID = int;

class MeshManager {
public:
    MeshManager(EventBus* bus);

    MeshID addMesh(
        const std::string& name,
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        bool dynamic = false
    );

    RenderMesh* getMesh(MeshID id);
    MeshID getMeshID(const std::string& name) const;
    RenderMesh* getRectangleMesh();

    int getMeshCount() const { return static_cast<int>(meshes.size()); }

private:
    std::unordered_map<MeshID, std::unique_ptr<RenderMesh>> meshes;
    std::unordered_map<std::string, MeshID> nameToID;

    MeshID nextID = 0;

    [[maybe_unused]] EventBus* bus;
};
