#include "ModelLoader.h"
#include <fstream>
#include <cstring>
#include <stdexcept>
#include "../../resources/data/Vertex.h"
#include "../../resources/data/MaterialData.h"
#include "../../resources/data/MeshData.h"
#include "../../resources/data/ModelData.h"
#include "../../resources/data/SubMeshData.h"

#include "../../math_custom/Quat.h"
#include "math.h"
#include "../../math_custom/GLAdapter.h"
#include "../../resources/data/MaterialTextureInfo.h"




// ---------------- Constructor ----------------
ModelLoader::ModelLoader(const std::string& filePath)
    : filePath(filePath)
{
    directory = filePath.substr(0, filePath.find_last_of('/') + 1);
    std::ifstream file(filePath);
    if (!file.is_open())
        throw std::runtime_error("Failed to open model file");
    file >> JSON;
    file.close();

    bufferData = loadBufferData(JSON["buffers"][0]["uri"]);
    parseGLTF();
}

void ModelLoader::parseGLTF()
{
    Mat4 identityMat;
    int sceneIndex = JSON.value("scene", 0);
    const json& scene = JSON["scenes"][sceneIndex];

    std::cout << "Parsing scene: " << scene.value("name", "unnamed") << "\n";

    if (scene.contains("nodes")) {
        std::cout << "Scene has " << scene["nodes"].size() << " root nodes\n";
        for (int nodeIndex : scene["nodes"]) {
            traverseNode(nodeIndex, identityMat);
        }
    }
    else {
        std::cerr << "Warning: Scene has no nodes!\n";
    }
}

void ModelLoader::traverseNode(unsigned int nextNode, const Mat4& parentMat)
{
    json node = JSON["nodes"][nextNode];
    std::string nodeName = node.value("name", "node_" + std::to_string(nextNode));
    std::cout << "Parsing node: " << nodeName << "\n";

    Mat4 matNode;

    if (node.find("matrix") != node.end())
    {
        matNode = fromArray(node["matrix"]);
    }
    else
    {
        Vector3 localTranslation(0.0f, 0.0f, 0.0f);
        if (node.contains("translation"))
        {
            Vector3 gltf_t(node["translation"][0], node["translation"][1], node["translation"][2]);

            localTranslation.x = gltf_t.x;
            localTranslation.y = gltf_t.y;
            localTranslation.z = gltf_t.z;

            std::cout << "Local Translation: " << localTranslation.x << ", " << localTranslation.y << ", " << localTranslation.z << "\n";
        }

        Quat localRotation; // Identity
        if (node.contains("rotation"))
        {
            Quat gltf_q(node["rotation"][0], node["rotation"][1], node["rotation"][2], node["rotation"][3]);

            localRotation.x = gltf_q.x;
            localRotation.y = gltf_q.y;
            localRotation.z = gltf_q.z;
            localRotation.w = gltf_q.w;

            std::cout << "Local Rotation: " << localRotation.x << ", " << localRotation.y << ", " << localRotation.z << ", " << localRotation.w << "\n";
        }

        Vector3 localScale(1.0f, 1.0f, 1.0f);
        if (node.contains("scale"))
        {
            Vector3 gltf_s(node["scale"][0], node["scale"][1], node["scale"][2]);

            localScale.x = gltf_s.x;
            localScale.y = gltf_s.y;
            localScale.z = gltf_s.z;
            std::cout << "Local Scale: " << localScale.x << ", " << localScale.y << ", " << localScale.z << "\n";
        }

        matNode =


           
            
            
            
            Mat4::translate(localTranslation)*
            GLAdapter::toGL(Mat4::fromQuat(localRotation))*
            Mat4::scale(localScale)
            ;
    }

    Mat4 worldMat = parentMat* matNode;

    if (node.contains("mesh"))
    {
        unsigned int meshInd = node["mesh"];
        parseMesh(meshInd);

        // Parse primitives to get material info
        const json& mesh = JSON["meshes"][meshInd];
        for (size_t primIndex = 0; primIndex < mesh["primitives"].size(); primIndex++)
        {
            const json& prim = mesh["primitives"][primIndex];

            unsigned int materialIndex = 0;
            if (prim.contains("material"))
            {
                materialIndex = prim["material"];
                parseMaterial(materialIndex);
            }

            // Create SubMeshData entry
            SubMeshData subMeshData;
            subMeshData.worldTransform = worldMat;
            subMeshData.meshIndex = model.meshes.size() - mesh["primitives"].size() + primIndex;
            subMeshData.materialIndex = materialIndex;
            subMeshData.name = nodeName + "_submesh_" + std::to_string(primIndex);

            model.submeshes.push_back(subMeshData);

            std::cout << "Created submesh: " << subMeshData.name << " (mesh: " << subMeshData.meshIndex << ", material: " << subMeshData.materialIndex << ")\n";
        }
    }

    if (node.contains("children"))
    {
        for (auto& child : node["children"])
            traverseNode(child, worldMat);
    }
}

void ModelLoader::parseMesh(unsigned int meshIndex)
{
    const json& mesh = JSON["meshes"][meshIndex];

    for (auto& prim : mesh["primitives"])
    {
        unsigned int posAcc = prim["attributes"]["POSITION"];


        unsigned int normAcc = prim["attributes"]["NORMAL"];
        unsigned int uvAcc = prim["attributes"]["TEXCOORD_0"];
        unsigned int indAcc = prim["indices"];

        auto positions = groupVec3(getFloats(JSON["accessors"][posAcc]));
        auto normals = groupVec3(getFloats(JSON["accessors"][normAcc]));
        auto uvs = groupVec2(getFloats(JSON["accessors"][uvAcc]));

        MeshData meshData;
        meshData.vertices = assembleVertices(positions, normals, uvs);
        meshData.indices = getIndices(JSON["accessors"][indAcc]);
        meshData.name = mesh.value("name", "mesh_" + std::to_string(meshIndex));

        std::cout << "Parsed mesh: " << meshData.name << " with " << meshData.vertices.size() << " vertices and " << meshData.indices.size() << " indices.\n";

        model.meshes.push_back(meshData);
    }
}

void ModelLoader::parseMaterial(unsigned int materialIndex)
{
    if (materialIndex < model.materials.size())
        return;

    MaterialData mat;
    const json& matJSON = JSON["materials"][materialIndex];

    mat.name = matJSON.value("name", "material_" + std::to_string(materialIndex));

    std::cout << "Parsing material: " << mat.name << "\n";

    // Parse PBR metallic roughness properties
    if (matJSON.contains("pbrMetallicRoughness"))
    {
        const json& pbr = matJSON["pbrMetallicRoughness"];
        mat.metallic = pbr.value("metallicFactor", 1.0f);
        mat.roughness = pbr.value("roughnessFactor", 1.0f);
        if (pbr.contains("baseColorFactor")) {
            auto& c = pbr["baseColorFactor"];
            mat.baseColorFactor = { c[0], c[1], c[2], c[3] };
        }
        std::cout << "  Metallic: " << mat.metallic << ", Roughness: " << mat.roughness << "\n";

        // Parse base color texture
        if (pbr.contains("baseColorTexture"))
        {
            unsigned int texIndex = pbr["baseColorTexture"]["index"];
            unsigned int imgIndex = JSON["textures"][texIndex]["source"];
            std::string uri = JSON["images"][imgIndex]["uri"];
            std::cout << "  Base Color Texture: " << uri << "\n";
            mat.textureInfo.push_back({ directory + uri, TextureType::Albedo });
        }

        // Parse metallic/roughness texture (ARM - Ambient Occlusion, Roughness, Metallic)
        if (pbr.contains("metallicRoughnessTexture"))
        {
            unsigned int texIndex = pbr["metallicRoughnessTexture"]["index"];
            unsigned int imgIndex = JSON["textures"][texIndex]["source"];
            std::string uri = JSON["images"][imgIndex]["uri"];
            std::cout << "  Metallic/Roughness Texture (ARM): " << uri << "\n";
            mat.textureInfo.push_back({ directory + uri, TextureType::ORM });
        }
    }

    // Parse normal map (can be outside of PBR block)
    if (matJSON.contains("normalTexture"))
    {
        unsigned int texIndex = matJSON["normalTexture"]["index"];
        unsigned int imgIndex = JSON["textures"][texIndex]["source"];
        std::string uri = JSON["images"][imgIndex]["uri"];
        std::cout << "  Normal Map: " << uri << "\n";
        mat.textureInfo.push_back({ directory + uri, TextureType::Normal });
    }

    // Optional: Parse occlusion texture
    if (matJSON.contains("occlusionTexture"))
    {
        unsigned int texIndex = matJSON["occlusionTexture"]["index"];
        unsigned int imgIndex = JSON["textures"][texIndex]["source"];
        std::string uri = JSON["images"][imgIndex]["uri"];
        std::cout << "  Occlusion Texture: " << uri << "\n";
        mat.textureInfo.push_back({ directory + uri, TextureType::AO });
    }


    if (matJSON.contains("emissiveFactor")) {
        auto& e = matJSON["emissiveFactor"];
        mat.emissiveFactor = { e[0], e[1], e[2] };
    }
    // Optional: Parse emissive texture
    if (matJSON.contains("emissiveTexture"))

        
    {
        
        unsigned int texIndex = matJSON["emissiveTexture"]["index"];
        unsigned int imgIndex = JSON["textures"][texIndex]["source"];
        std::string uri = JSON["images"][imgIndex]["uri"];
        std::cout << "  Emissive Texture: " << uri << "\n";
        mat.textureInfo.push_back({ directory + uri, TextureType::Emissive });
    }

    // Optional: Parse height/displacement texture from extras
    if (matJSON.contains("extras"))
    {
        const json& extras = matJSON["extras"];

        if (extras.contains("heightTexture"))
        {
            unsigned int texIndex = extras["heightTexture"]["index"];
            unsigned int imgIndex = JSON["textures"][texIndex]["source"];
            std::string uri = JSON["images"][imgIndex]["uri"];
            std::cout << "  Height Texture: " << uri << "\n";
            mat.textureInfo.push_back({ directory + uri, TextureType::Height });
        }

        if (extras.contains("heightScale"))
        {
            mat.heightScale = extras["heightScale"].get<float>();
        }
    }

    model.materials.push_back(mat);
}

std::vector<Vertex> ModelLoader::assembleVertices(
    const std::vector<Vector3>& positions,
    const std::vector<Vector3>& normals,
    const std::vector<Vector2>& uvs)
{
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < positions.size(); i++)
    {
        Vertex v;
        v.position = positions[i];
        v.normal = normals[i];
        v.uv = uvs[i];
        v.color = Vector3(1.0f, 1.0f, 1.0f);
        vertices.push_back(v);
    }
    return vertices;
}

std::vector<Vector3> ModelLoader::groupVec3(const std::vector<float>& floats) {
    std::vector<Vector3> out;
    for (size_t i = 0; i < floats.size(); i += 3) {
        float gltf_x = floats[i + 0];
        float gltf_y = floats[i + 1];
        float gltf_z = floats[i + 2];

        Vector3 v;
        v.x = gltf_x;
        v.y = gltf_y;
        v.z = gltf_z;

        out.emplace_back(v);
    }
    return out;
}

std::vector<Vector2> ModelLoader::groupVec2(const std::vector<float>& floats)
{
    std::vector<Vector2> out;
    for (size_t i = 0; i < floats.size(); i += 2)
        out.emplace_back(floats[i], floats[i + 1]);
    return out;
}

std::vector<float> ModelLoader::getFloats(const json& accessor)
{
    std::vector<float> floatVec;
    unsigned int buffViewInd = accessor.value("bufferView", 1);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];
    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];
    unsigned int numPerVert;
    if (type == "SCALAR") numPerVert = 1;
    else if (type == "VEC2") numPerVert = 2;
    else if (type == "VEC3") numPerVert = 3;
    else if (type == "VEC4") numPerVert = 4;
    else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");
    unsigned int beginningOfData = byteOffset + accByteOffset;
    unsigned int lengthOfData = count * 4 * numPerVert;
    for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
    {
        unsigned char bytes[] = { bufferData[i], bufferData[i + 1], bufferData[i + 2], bufferData[i + 3] };
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }
    return floatVec;
}

std::vector<GLuint> ModelLoader::getIndices(const json& accessor)
{
    std::vector<GLuint> indices;
    unsigned int bufferView = accessor["bufferView"];
    unsigned int count = accessor["count"];
    unsigned int offset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];
    const json& view = JSON["bufferViews"][bufferView];
    unsigned int byteOffset = view.value("byteOffset", 0);
    unsigned int start = byteOffset + offset;
    if (componentType == 5125)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            GLuint v;
            std::memcpy(&v, &bufferData[start + i * 4], 4);
            indices.push_back(v);
        }
    }
    else if (componentType == 5123)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            unsigned short v;
            std::memcpy(&v, &bufferData[start + i * 2], 2);
            indices.push_back(v);
        }
    }
    return indices;
}

std::vector<unsigned char> ModelLoader::loadBufferData(const std::string& uri)
{
    std::ifstream file(directory + uri, std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to load buffer");
    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

Mat4 ModelLoader::fromArray(const json& arr)
{
    
        Mat4 m;
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col)
                m.data[row * 4 + col] = arr[col * 4 + row];
        return m;
    
}