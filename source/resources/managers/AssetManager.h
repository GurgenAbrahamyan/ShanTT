#pragma once

#include "TextureManager.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "render/backend/ShaderManager.h"

class AssetManager
{
public:

    TextureManager&  textures()  { return m_textures; }
    MeshManager&     meshes()    { return m_meshes; }
    MaterialManager& materials() { return m_materials; }
    ModelManager&    models()    { return m_models; }
    ShaderManager&   shaders()   { return m_shaders; }

private:
    TextureManager  m_textures;
    MeshManager     m_meshes;
    MaterialManager m_materials;
    ModelManager    m_models{&m_meshes, &m_materials, &m_textures};
    ShaderManager   m_shaders;
};