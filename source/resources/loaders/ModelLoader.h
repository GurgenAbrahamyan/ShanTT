#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <json/json.h>
#include "../../math_custom/Mat4.h"
#include "../../math_custom/Vector3.h"
#include "../../math_custom/Vector2.h"
#include "../../math_custom/Quat.h"
#include "../../resources/data/ModelData.h"


using json = nlohmann::json;

struct Vertex;
struct MeshData;
struct MaterialData;
struct SubMeshData;


class ModelLoader {
public:
    explicit ModelLoader(const std::string& filePath);
	bool isValid() const { return hasParsed; }
    const ModelData& getModelData() const { return model; }

private:
    std::string filePath;
    std::string directory;

    json JSON;
    std::vector<unsigned char> bufferData;
    ModelData model;
    bool hasParsed = true;


    void parseGLTF();
    void traverseNode(unsigned int nextNode, const Mat4& parentMat);
    void parseMesh(unsigned int meshIndex);
    void parseMaterial(unsigned int materialIndex);
    std::vector<Vertex> assembleVertices(
        const std::vector<Vector3>& positions,
        const std::vector<Vector3>& normals,
        const std::vector<Vector2>& uvs
    );

    //  void bakeTransformToMesh(MeshData& meshData, const Mat4& transform);

    std::vector<Vector3> groupVec3(const std::vector<float>& floats);
    std::vector<Vector2> groupVec2(const std::vector<float>& floats);

    std::vector<float> getFloats(const json& accessor);
    std::vector<GLuint> getIndices(const json& accessor);
    //   std::vector<std::string> getTexturesForMesh(unsigned int meshIndex);

    std::vector<unsigned char> loadBufferData(const std::string& uri);


    static Mat4 fromArray(const json& arr);
};