#pragma once
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <optional>
#include <glad/glad.h>
#include <json/json.h>
#include "../../math_custom/Mat4.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Vector2.h"
#include "../../math_custom/Quat.h"

#include "../../resources/data/ModelData.h"

using json = nlohmann::json;

struct StaticVertex;
struct SkinnedVertex;

class ModelLoader {
public:
    explicit ModelLoader(const std::string& filePath);
    bool isValid() const { return hasParsed; }
    const ModelData& getModelData() const { return model; }

private:

    std::unordered_map<unsigned int, unsigned int> nodeParent;

    void buildNodeParentMap();
    struct ParsedMeshRef {
        MeshKind meshKind;
        uint32_t meshIndex; 
    };

    std::string filePath;
    std::string directory;

    json JSON;
    std::vector<unsigned char> bufferData;
    ModelData model;
    bool hasParsed = true;
    std::unordered_map<unsigned int, uint32_t> jointNodeToBone;

    void parseGLTF();
    void buildJointNodeMap();

    void traverseNode(
        unsigned int nodeIndex,
        const Vector3& accumPos,
        const Quat& accumRot,
        const Vector3& accumScale,
        uint32_t parentSubmeshIndex,
        std::optional<uint32_t> parentBoneIndex
    );

    std::vector<ParsedMeshRef> parseMesh(unsigned int meshIndex);
    void parseMaterial(unsigned int materialIndex);

    std::vector<StaticVertex> assembleStaticVertices(
        const std::vector<Vector3>& positions,
        const std::vector<Vector3>& normals,
        const std::vector<Vector2>& uvs
    );

    std::vector<SkinnedVertex> assembleSkinnedVertices(
        const std::vector<Vector3>& positions,
        const std::vector<Vector3>& normals,
        const std::vector<Vector2>& uvs,
        const std::vector<std::array<uint32_t, 4>>& joints,
        const std::vector<std::array<float, 4>>& weights
    );

    void parseSkeleton();
    std::vector<Mat4>    getMat4s(const json& accessor);
    std::vector<Vector3> groupVec3(const std::vector<float>& floats);
    std::vector<Vector2> groupVec2(const std::vector<float>& floats);
    std::vector<std::array<uint32_t, 4>> groupJoints(const json& accessor);
    std::vector<std::array<float, 4>>    groupWeights(const std::vector<float>& floats);

    std::vector<float>  getFloats(const json& accessor);
    std::vector<GLuint> getIndices(const json& accessor);

    std::vector<unsigned char> loadBufferData(const std::string& uri);

    static Mat4 fromArray(const json& arr);
    static void decomposeMat4(const Mat4& m, Vector3& outPos, Quat& outRot, Vector3& outScale);
};