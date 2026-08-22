#include "ModelLoader.h"
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <iostream>

#include "../../resources/data/StaticVertex.h"
#include "../../resources/data/SkinnedVertex.h"
#include "../../resources/data/MaterialData.h"
#include "../../resources/data/StaticMeshData.h"
#include "../../resources/data/SkinnedMeshData.h"
#include "../../resources/data/ModelData.h"
#include "../../resources/data/SubMeshData.h"
#include "../../resources/data/SkeletonData.h"
#include "../../resources/data/BoneData.h"

// ---------------- Constructor ----------------
ModelLoader::ModelLoader(const std::string& filePath)
    : filePath(filePath)
{
    directory = filePath.substr(0, filePath.find_last_of('/') + 1);
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Failed to open model file";
        hasParsed = false;
        return;
    }
    file >> JSON;
    file.close();

    bufferData = loadBufferData(JSON["buffers"][0]["uri"]);
    parseGLTF();
}

void ModelLoader::parseGLTF()
{
    buildNodeParentMap();
    parseSkeleton();
    buildJointNodeMap();

    int sceneIndex = JSON.value("scene", 0);
    const json& scene = JSON["scenes"][sceneIndex];

    std::cout << "Parsing scene: " << scene.value("name", "unnamed") << "\n";

    if (scene.contains("nodes")) {
        std::cout << "Scene has " << scene["nodes"].size() << " root nodes\n";
        for (int nodeIndex : scene["nodes"]) {
            traverseNode(
                nodeIndex,
                Vector3(0.0f, 0.0f, 0.0f),
                Quat(),
                Vector3(1.0f, 1.0f, 1.0f),
                UINT32_MAX,
                std::nullopt
            );
        }
    } else {
        std::cerr << "Warning: Scene has no nodes!\n";
    }
}

void ModelLoader::buildJointNodeMap()
{
    if (!model.skeleton.has_value()) return;
    if (!JSON.contains("skins") || JSON["skins"].empty()) return;

    const json& skin = JSON["skins"][0];
    const std::vector<int> jointNodes = skin["joints"].get<std::vector<int>>();

    for (size_t i = 0; i < jointNodes.size(); ++i) {
        jointNodeToBone[jointNodes[i]] = static_cast<uint32_t>(i);
    }
}

void ModelLoader::traverseNode(
    unsigned int nodeIndex,
    const Vector3& accumPos,
    const Quat& accumRot,
    const Vector3& accumScale,
    uint32_t parentSubmeshIndex,
    std::optional<uint32_t> parentBoneIndex)
{
    json node = JSON["nodes"][nodeIndex];
    std::string nodeName = node.value("name", "node_" + std::to_string(nodeIndex));
    std::cout << "Parsing node: " << nodeName << "\n";

    Vector3 t(0.0f, 0.0f, 0.0f);
    Quat    r;
    Vector3 s(1.0f, 1.0f, 1.0f);

    if (node.find("matrix") != node.end())
    {
        // Rare exporter path — decompose once here so everything downstream
        // of this point stays TRS.
        Mat4 m = fromArray(node["matrix"]);
        decomposeMat4(m, t, r, s);
    }
    else
    {
        if (node.contains("translation")) {
            t = Vector3(node["translation"][0], node["translation"][1], node["translation"][2]);
        }
        if (node.contains("rotation")) {
            r = Quat(node["rotation"][0], node["rotation"][1], node["rotation"][2], node["rotation"][3]);
        }
        if (node.contains("scale")) {
            s = Vector3(node["scale"][0], node["scale"][1], node["scale"][2]);
        }
    }

    // Fold this node's TRS into the running accumulation (relative to the
    // nearest ancestor submesh/root). Standard TRS composition:
    //   combinedPos   = accumPos + accumRot * (accumScale * t)
    //   combinedRot   = accumRot * r
    //   combinedScale = accumScale * s   (component-wise; assumes no
    //                                      rotated-non-uniform-scale chains)
    Vector3 combinedPos   = accumPos + (accumRot.rotate(accumScale * t));
    Quat    combinedRot   = accumRot * r;
    Vector3 combinedScale = accumScale * s;

    // Is this node itself a skeleton joint? If so, mesh-less children
    // beneath it (until a mesh node is hit) should end up bone-attached.
    std::optional<uint32_t> boneIndexForChildren = parentBoneIndex;
    if (auto it = jointNodeToBone.find(nodeIndex); it != jointNodeToBone.end()) {
        boneIndexForChildren = it->second;
    }

    uint32_t submeshIndexForChildren = parentSubmeshIndex;
    Vector3  posForChildren   = combinedPos;
    Quat     rotForChildren   = combinedRot;
    Vector3  scaleForChildren = combinedScale;

    if (node.contains("mesh"))
    {
        unsigned int meshInd = node["mesh"];
        std::vector<ParsedMeshRef> parsedRefs = parseMesh(meshInd);

        const json& mesh = JSON["meshes"][meshInd];
        for (size_t primIndex = 0; primIndex < mesh["primitives"].size(); primIndex++)
        {
            const json& prim = mesh["primitives"][primIndex];

            uint32_t materialIndex = 0;
            if (prim.contains("material")) {
                materialIndex = prim["material"];
                parseMaterial(materialIndex);
            }

            SubMeshData subMeshData;
            subMeshData.name = nodeName + "_submesh_" + std::to_string(primIndex);
            subMeshData.meshKind = parsedRefs[primIndex].meshKind;
            subMeshData.meshIndex = parsedRefs[primIndex].meshIndex;
            subMeshData.materialIndex = materialIndex;
            subMeshData.parentSubmeshIndex = parentSubmeshIndex;
            subMeshData.localPosition = combinedPos;
            subMeshData.localRotation = combinedRot;
            subMeshData.localScale = combinedScale;
            subMeshData.attachBoneIndex = parentBoneIndex;

            model.submeshes.push_back(subMeshData);

            std::cout << "Created submesh: " << subMeshData.name
                       << " (kind: " << (subMeshData.meshKind == MeshKind::Static ? "static" : "skinned")
                       << ", material: " << subMeshData.materialIndex << ")\n";

            // Children of this node become relative to THIS submesh now.
            // The accumulation resets to identity, and bone-attachment
            // chaining stops here — a part parented under another mesh part
            // is a plain parent relationship, not a bone attachment.
            submeshIndexForChildren = static_cast<uint32_t>(model.submeshes.size() - 1);
            posForChildren   = Vector3(0.0f, 0.0f, 0.0f);
            rotForChildren   = Quat();
            scaleForChildren = Vector3(1.0f, 1.0f, 1.0f);
            boneIndexForChildren = std::nullopt;
        }
    }

    if (node.contains("children"))
    {
        for (auto& child : node["children"]) {
            traverseNode(
                child,
                posForChildren, rotForChildren, scaleForChildren,
                submeshIndexForChildren,
                boneIndexForChildren
            );
        }
    }
}

std::vector<ModelLoader::ParsedMeshRef> ModelLoader::parseMesh(unsigned int meshIndex)
{
    const json& mesh = JSON["meshes"][meshIndex];
    std::vector<ParsedMeshRef> results;

    for (auto& prim : mesh["primitives"])
    {
        const auto& attrs = prim["attributes"];

        unsigned int posAcc  = attrs["POSITION"];
        unsigned int normAcc = attrs["NORMAL"];
        unsigned int uvAcc{};
        if(attrs.contains("TEXCOORD_0"))
            uvAcc   = attrs["TEXCOORD_0"];
        unsigned int indAcc  = prim["indices"];

        auto positions = groupVec3(getFloats(JSON["accessors"][posAcc]));
        auto normals   = groupVec3(getFloats(JSON["accessors"][normAcc]));
        auto uvs       = groupVec2(getFloats(JSON["accessors"][uvAcc]));
        auto indices   = getIndices(JSON["accessors"][indAcc]);

        std::string meshName = mesh.value("name", "mesh_" + std::to_string(meshIndex));

        bool isSkinned = attrs.contains("JOINTS_0") && attrs.contains("WEIGHTS_0");

        if (isSkinned)
        {
            unsigned int jointsAcc  = attrs["JOINTS_0"];
            unsigned int weightsAcc = attrs["WEIGHTS_0"];

            auto joints  = groupJoints(JSON["accessors"][jointsAcc]);
            auto weights = groupWeights(getFloats(JSON["accessors"][weightsAcc]));

            SkinnedMeshData meshData;
            meshData.name = meshName;
            meshData.indices.assign(indices.begin(), indices.end());
            meshData.vertices = assembleSkinnedVertices(positions, normals, uvs, joints, weights);

            model.skinnedMeshes.push_back(std::move(meshData));
            results.push_back({ MeshKind::Skinned, static_cast<uint32_t>(model.skinnedMeshes.size() - 1) });

            std::cout << "Parsed skinned mesh: " << meshName
                       << " with " << positions.size() << " vertices\n";
        }
        else
        {
            StaticMeshData meshData;
            meshData.name = meshName;
            meshData.indices.assign(indices.begin(), indices.end());
            meshData.vertices = assembleStaticVertices(positions, normals, uvs);

            model.staticMeshes.push_back(std::move(meshData));
            results.push_back({ MeshKind::Static, static_cast<uint32_t>(model.staticMeshes.size() - 1) });

            std::cout << "Parsed static mesh: " << meshName
                       << " with " << positions.size() << " vertices\n";
        }
    }

    return results;
}

void ModelLoader::parseMaterial(unsigned int materialIndex)
{
    if (materialIndex < model.materials.size())
        return;

    MaterialData mat;
    const json& matJSON = JSON["materials"][materialIndex];

    mat.name = matJSON.value("name", "material_" + std::to_string(materialIndex));

    std::cout << "Parsing material: " << mat.name << "\n";

    if (matJSON.contains("pbrMetallicRoughness"))
    {
        const json& pbr = matJSON["pbrMetallicRoughness"];
        mat.metallic = pbr.value("metallicFactor", 1.0f);
        mat.roughness = pbr.value("roughnessFactor", 0.0f);
        if (pbr.contains("baseColorFactor")) {
            auto& c = pbr["baseColorFactor"];
            mat.baseColorFactor = { c[0], c[1], c[2], c[3] };
        }

        if (pbr.contains("baseColorTexture"))
        {
            unsigned int texIndex = pbr["baseColorTexture"]["index"];
            unsigned int imgIndex = JSON["textures"][texIndex]["source"];
            std::string uri = JSON["images"][imgIndex]["uri"];
            mat.textureInfo.push_back({ directory + uri, TextureType::Albedo });
        }

        if (pbr.contains("metallicRoughnessTexture"))
        {
            unsigned int texIndex = pbr["metallicRoughnessTexture"]["index"];
            unsigned int imgIndex = JSON["textures"][texIndex]["source"];
            std::string uri = JSON["images"][imgIndex]["uri"];
            mat.textureInfo.push_back({ directory + uri, TextureType::ORM });
        }
    }

    if (matJSON.contains("normalTexture"))
    {
        unsigned int texIndex = matJSON["normalTexture"]["index"];
        unsigned int imgIndex = JSON["textures"][texIndex]["source"];
        std::string uri = JSON["images"][imgIndex]["uri"];
        mat.textureInfo.push_back({ directory + uri, TextureType::Normal });
    }

    if (matJSON.contains("occlusionTexture"))
    {
        unsigned int texIndex = matJSON["occlusionTexture"]["index"];
        unsigned int imgIndex = JSON["textures"][texIndex]["source"];
        std::string uri = JSON["images"][imgIndex]["uri"];
        mat.textureInfo.push_back({ directory + uri, TextureType::AO });
    }

    if (matJSON.contains("emissiveFactor")) {
        auto& e = matJSON["emissiveFactor"];
        mat.emissiveFactor = { e[0], e[1], e[2] };
    }

    if (matJSON.contains("emissiveTexture"))
    {
        unsigned int texIndex = matJSON["emissiveTexture"]["index"];
        unsigned int imgIndex = JSON["textures"][texIndex]["source"];
        std::string uri = JSON["images"][imgIndex]["uri"];
        mat.textureInfo.push_back({ directory + uri, TextureType::Emissive });
    }

    if (matJSON.contains("extras"))
    {
        const json& extras = matJSON["extras"];

        if (extras.contains("heightTexture"))
        {
            unsigned int texIndex = extras["heightTexture"]["index"];
            unsigned int imgIndex = JSON["textures"][texIndex]["source"];
            std::string uri = JSON["images"][imgIndex]["uri"];
            mat.textureInfo.push_back({ directory + uri, TextureType::Height });
        }

        if (extras.contains("heightScale"))
        {
            mat.heightScale = extras["heightScale"].get<float>();
        }
    }

    model.materials.push_back(mat);
}

std::vector<StaticVertex> ModelLoader::assembleStaticVertices(
    const std::vector<Vector3>& positions,
    const std::vector<Vector3>& normals,
    const std::vector<Vector2>& uvs)
{
    std::vector<StaticVertex> vertices;
    vertices.reserve(positions.size());
    for (size_t i = 0; i < positions.size(); i++)
    {
        StaticVertex v;
        v.pos = positions[i];
        v.normal = normals[i];
        v.uv = uvs[i];
        vertices.push_back(v);
    }
    return vertices;
}

std::vector<SkinnedVertex> ModelLoader::assembleSkinnedVertices(
    const std::vector<Vector3>& positions,
    const std::vector<Vector3>& normals,
    const std::vector<Vector2>& uvs,
    const std::vector<std::array<uint32_t, 4>>& joints,
    const std::vector<std::array<float, 4>>& weights)
{
    std::vector<SkinnedVertex> vertices;
    vertices.reserve(positions.size());
    for (size_t i = 0; i < positions.size(); i++)
    {
        SkinnedVertex v;
        v.pos = positions[i];
        v.normal = normals[i];
        v.uv = uvs[i];

        for (int j = 0; j < 4; ++j) {
            v.boneIds[j] = joints[i][j];
            v.weights[j] = weights[i][j];
        }

        vertices.push_back(v);
    }
    return vertices;
}

std::vector<Vector3> ModelLoader::groupVec3(const std::vector<float>& floats) {
    std::vector<Vector3> out;
    out.reserve(floats.size() / 3);
    for (size_t i = 0; i + 2 < floats.size(); i += 3) {
        out.emplace_back(floats[i], floats[i + 1], floats[i + 2]);
    }
    return out;
}

std::vector<Vector2> ModelLoader::groupVec2(const std::vector<float>& floats)
{
    std::vector<Vector2> out;
    out.reserve(floats.size() / 2);
    for (size_t i = 0; i + 1 < floats.size(); i += 2)
        out.emplace_back(floats[i], floats[i + 1]);
    return out;
}

std::vector<std::array<uint32_t, 4>> ModelLoader::groupJoints(const json& accessor)
{
    // JOINTS_0 is UNSIGNED_BYTE (5121) or UNSIGNED_SHORT (5123) — raw
    // integer data, never float, so this reads bytes directly rather than
    // going through getFloats.
    std::vector<std::array<uint32_t, 4>> out;

    unsigned int bufferViewIndex = accessor["bufferView"];
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];
    const json& bufferView = JSON["bufferViews"][bufferViewIndex];
    unsigned int byteOffset = bufferView.value("byteOffset", 0);
    unsigned int start = byteOffset + accByteOffset;

    unsigned int compSize = (componentType == 5121) ? 1 : 2; // UNSIGNED_BYTE : UNSIGNED_SHORT
    unsigned int stride = 4 * compSize;

    out.reserve(count);
    for (unsigned int i = 0; i < count; ++i)
    {
        std::array<uint32_t, 4> ids{};
        for (int j = 0; j < 4; ++j)
        {
            unsigned int off = start + i * stride + j * compSize;
            if (compSize == 1) {
                ids[j] = bufferData[off];
            } else {
                uint16_t v;
                std::memcpy(&v, &bufferData[off], 2);
                ids[j] = v;
            }
        }
        out.push_back(ids);
    }
    return out;
}

std::vector<std::array<float, 4>> ModelLoader::groupWeights(const std::vector<float>& floats)
{
    std::vector<std::array<float, 4>> out;
    out.reserve(floats.size() / 4);
    for (size_t i = 0; i + 3 < floats.size(); i += 4) {
        out.push_back({ floats[i], floats[i + 1], floats[i + 2], floats[i + 3] });
    }
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
    for (size_t row = 0; row < 4; ++row)
        for (size_t col = 0; col < 4; ++col)
            m(row, col) = arr[col * 4 + row];
    return m;
}

void ModelLoader::decomposeMat4(const Mat4& m, Vector3& outPos, Quat& outRot, Vector3& outScale)
{
    // Translation is the last column (assuming column-major, row/col access
    // matching fromArray's m(row, col) convention above).
    outPos = Vector3(m(0, 3), m(1, 3), m(2, 3));

    Vector3 col0(m(0, 0), m(1, 0), m(2, 0));
    Vector3 col1(m(0, 1), m(1, 1), m(2, 1));
    Vector3 col2(m(0, 2), m(1, 2), m(2, 2));

    outScale = Vector3(col0.length(), col1.length(), col2.length());

    Vector3 c0 = outScale.x > 1e-8f ? col0 * (1.0f / outScale.x) : Vector3(1, 0, 0);
    Vector3 c1 = outScale.y > 1e-8f ? col1 * (1.0f / outScale.y) : Vector3(0, 1, 0);
    Vector3 c2 = outScale.z > 1e-8f ? col2 * (1.0f / outScale.z) : Vector3(0, 0, 1);

    Mat4 rotOnly;
    rotOnly(0,0) = c0.x; rotOnly(1,0) = c0.y; rotOnly(2,0) = c0.z;
    rotOnly(0,1) = c1.x; rotOnly(1,1) = c1.y; rotOnly(2,1) = c1.z;
    rotOnly(0,2) = c2.x; rotOnly(1,2) = c2.y; rotOnly(2,2) = c2.z;

    outRot = Quat::fromMat4(rotOnly);
}

void ModelLoader::parseSkeleton()
{
    if (!JSON.contains("skins") || JSON["skins"].empty())
        return;

    const json& skin = JSON["skins"][0];

    if (!skin.contains("joints") || skin["joints"].empty())
        return;

    const std::vector<int> jointNodes = skin["joints"].get<std::vector<int>>();

    SkeletonData skeleton;
    skeleton.bones.reserve(jointNodes.size());

    std::vector<Mat4> inverseBindMatrices;

    if (skin.contains("inverseBindMatrices"))
    {
        unsigned int accessorIndex = skin["inverseBindMatrices"];
        inverseBindMatrices = getMat4s(JSON["accessors"][accessorIndex]);
    }
    else
    {
        inverseBindMatrices.resize(jointNodes.size(), Mat4());
    }

    for (size_t i = 0; i < jointNodes.size(); ++i)
    {
        unsigned int nodeIndex = jointNodes[i];
        const json& node = JSON["nodes"][nodeIndex];

        BoneData bone;
        bone.name = node.value("name", "bone_" + std::to_string(i));
        bone.parentIndex = UINT32_MAX;

        for (size_t candidate = 0; candidate < jointNodes.size(); ++candidate)
        {
            unsigned int parentNode = jointNodes[candidate];
            const json& parent = JSON["nodes"][parentNode];

            if (!parent.contains("children")) continue;

            for (const auto& child : parent["children"])
            {
                if (child.get<unsigned int>() == nodeIndex)
                {
                    bone.parentIndex = static_cast<int32_t>(candidate);
                    break;
                }
            }
            if (bone.parentIndex == UINT32_MAX)
            {
                /*Vector3 accPos(0.0f, 0.0f, 0.0f);
                Quat    accRot;
                Vector3 accScale(1.0f, 1.0f, 1.0f);

                unsigned int cursor = nodeIndex;
                auto it = nodeParent.find(cursor);

                while (it != nodeParent.end())
                {
                    unsigned int ancestorIdx = it->second;
                    const json& ancestor = JSON["nodes"][ancestorIdx];

                    Vector3 at(0.0f, 0.0f, 0.0f);
                    Quat    ar;
                    Vector3 as(1.0f, 1.0f, 1.0f);

                    if (ancestor.contains("translation"))
                        at = Vector3(ancestor["translation"][0], ancestor["translation"][1], ancestor["translation"][2]);
                    if (ancestor.contains("rotation"))
                        ar = Quat(ancestor["rotation"][0], ancestor["rotation"][1], ancestor["rotation"][2], ancestor["rotation"][3]);
                    if (ancestor.contains("scale"))
                        as = Vector3(ancestor["scale"][0], ancestor["scale"][1], ancestor["scale"][2]);

                    accPos   = at + ar.rotate(as * accPos);
                    accRot   = ar * accRot;
                    accScale = as * accScale;

                    cursor = ancestorIdx;
                    it = nodeParent.find(cursor);
                }

                bone.translation = accPos + accRot.rotate(accScale * bone.translation);
                bone.rotation     = accRot * bone.rotation;
                bone.scale        = accScale * bone.scale;*/
            }
        }
        if (node.contains("translation")) {
            bone.translation = Vector3(node["translation"][0], node["translation"][1], node["translation"][2]);
        }
        if (node.contains("rotation")) {
            bone.rotation = Quat(node["rotation"][0], node["rotation"][1], node["rotation"][2], node["rotation"][3]);
        }
        if (node.contains("scale")) {
            bone.scale = Vector3(node["scale"][0], node["scale"][1], node["scale"][2]);
        }
    
        bone.inverseBindMatrix = ((i < inverseBindMatrices.size()) ? inverseBindMatrices[i] : Mat4()).transpose();

        skeleton.bones.push_back(bone);
    
}

    model.skeleton = std::move(skeleton);

    std::cout << "Parsed skeleton with " << model.skeleton->bones.size() << " bones.\n";
}

std::vector<Mat4> ModelLoader::getMat4s(const json& accessor)
{
    std::vector<Mat4> matrices;

    unsigned int bufferViewIndex = accessor["bufferView"];
    unsigned int count = accessor["count"];
    unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
    const json& bufferView = JSON["bufferViews"][bufferViewIndex];
    unsigned int bufferViewByteOffset = bufferView.value("byteOffset", 0);
    unsigned int start = bufferViewByteOffset + accessorByteOffset;

    matrices.reserve(count);

    for (unsigned int i = 0; i < count; ++i)
    {
        Mat4 matrix;
        for (unsigned int j = 0; j < 16; ++j)
        {
            float value;
            std::memcpy(&value, &bufferData[start + i * 16 * sizeof(float) + j * sizeof(float)], sizeof(float));
            matrix(j / 4, j % 4) = value;
        }
        matrices.push_back(matrix);
    }

    return matrices;
}

void ModelLoader::buildNodeParentMap()
{
    for (size_t nodeIdx = 0; nodeIdx < JSON["nodes"].size(); ++nodeIdx)
    {
        const json& node = JSON["nodes"][nodeIdx];
        if (!node.contains("children")) continue;

        for (const auto& child : node["children"])
        {
            nodeParent[child.get<unsigned int>()] =
                static_cast<unsigned int>(nodeIdx);
        }
    }
}