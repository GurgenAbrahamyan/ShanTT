#pragma once

#include "TextureManager.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "render/backend/ShaderManager.h"
#include "SkeletonManager.h"
#include "animation/AnimationManager.h"
#include "physics/managers/RagdollManager.h"

class AssetManager
{
public:

    TextureManager&   textures()  { return m_textures;  }
    MeshManager&      meshes()    { return m_meshes;    }
    MaterialManager&  materials() { return m_materials; }
    ModelManager&     models()    { return m_models;    }
    ShaderManager&    shaders()   { return m_shaders;   }
    SkeletonManager&  skeletons() { return m_skeletons; }
    AnimationManager& animations(){ return m_animations;}
    RagdollManager&    ragdolls()  { return m_ragdolls;}
private:
    TextureManager   m_textures;
    MeshManager      m_meshes;
    MaterialManager  m_materials;
    SkeletonManager  m_skeletons;
    ModelManager     m_models{&m_meshes, &m_materials, &m_textures, &m_skeletons};
    ShaderManager    m_shaders;
    AnimationManager m_animations;
    RagdollManager   m_ragdolls;
};