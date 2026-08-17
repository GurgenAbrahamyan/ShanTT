#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ResourcePool.h"
#include "MeshHandleTypes.h"

#include "../assets/RenderMesh.h"
#include "../data/Vertex.h"

class MeshManager {
public:
    MeshManager() = default;

    MeshID addMesh(
        const std::string& name,
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices
    );

    RenderMesh* getMesh(MeshID id);
    RenderMesh* getMesh(MeshID id) const ;
    
    void removeMesh(MeshID id);

    MeshID getMeshID(const std::string& name) const;

    RenderMesh* getRectangleMesh();

    size_t getMeshCount() const {
        return pool.size();
    }

private:
    struct MeshRecord {
        std::unique_ptr<RenderMesh> mesh;
        std::string name;
    };

    ResourcePool<MeshRecord, MeshTag> pool;
    std::unordered_map<std::string, MeshID> lookup;
};