// MeshManager.h
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ResourcePool.h"
#include "MeshHandleTypes.h"

#include "../assets/RenderMesh.h"
#include "../data/StaticVertex.h"
#include "../data/SkinnedVertex.h"

class MeshManager {
public:
    MeshManager() = default;

    StaticMeshID addStaticMesh(
        const std::string& name,
        const std::vector<StaticVertex>& vertices,
        const std::vector<unsigned int>& indices
    );

    RenderMesh<StaticVertex>* getStaticMesh(StaticMeshID id);
    RenderMesh<StaticVertex>* getStaticMesh(StaticMeshID id) const;

    void removeStaticMesh(StaticMeshID id);
    StaticMeshID getStaticMeshID(const std::string& name) const;

    RenderMesh<StaticVertex>* getRectangleMesh(); // static-only primitive, lives here now

    SkinnedMeshID addSkinnedMesh(
        const std::string& name,
        const std::vector<SkinnedVertex>& vertices,
        const std::vector<unsigned int>& indices
    );

    RenderMesh<SkinnedVertex>* getSkinnedMesh(SkinnedMeshID id);
    RenderMesh<SkinnedVertex>* getSkinnedMesh(SkinnedMeshID id) const;

    void removeSkinnedMesh(SkinnedMeshID id);
    SkinnedMeshID getSkinnedMeshID(const std::string& name) const;

    size_t getStaticMeshCount() const  { return staticPool.size(); }
    size_t getSkinnedMeshCount() const { return skinnedPool.size(); }

    const std::string& GetStaticMeshName(StaticMeshID id) const {
        static const std::string unknown = "<unnamed>";
        const MeshRecord<StaticVertex>* rec = staticPool.get(id);
        return rec ? rec->name : unknown; 
    }

    const std::string& GetSkinnedMeshName(SkinnedMeshID id) const {
        static const std::string unknown = "<unnamed>";
        const MeshRecord<SkinnedVertex>* rec = skinnedPool.get(id);
        return rec ? rec->name : unknown;
    }

    template<typename Fn>
    void ForEachStaticMesh(Fn&& fn) const {
        staticPool.forEachAlive([&](Handle<StaticMeshTag> h, const MeshRecord<StaticVertex>& rec) { fn(h, rec.name); });
    }

    template<typename Fn>
    void ForEachSkinnedMesh(Fn&& fn) const {
        skinnedPool.forEachAlive([&](Handle<SkinnedMeshTag> h, const MeshRecord<SkinnedVertex>& rec) { fn(h, rec.name); });
    }

private:
    template <typename VertexT>
    struct MeshRecord {
        std::unique_ptr<RenderMesh<VertexT>> mesh;
        std::string name;
    };

    ResourcePool<MeshRecord<StaticVertex>,  StaticMeshTag>  staticPool;
    ResourcePool<MeshRecord<SkinnedVertex>, SkinnedMeshTag> skinnedPool;

    std::unordered_map<std::string, StaticMeshID>  staticLookup;
    std::unordered_map<std::string, SkinnedMeshID> skinnedLookup;
};