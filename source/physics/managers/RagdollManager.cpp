#include "RagdollManager.h"

#include <fstream>
#include <stdexcept>

#include <json.h>

using json = nlohmann::json;

namespace
{
    RagdollShapeType parseShapeType(
        const std::string& type)
    {
        if (type == "capsule")
            return RagdollShapeType::Capsule;

        if (type == "box")
            return RagdollShapeType::Box;

        if (type == "sphere")
            return RagdollShapeType::Sphere;

        throw std::runtime_error(
            "Unknown ragdoll shape type: " + type
        );
    }

    Vector3 parseVector3(
        const json& value,
        const std::string& fieldName)
    {
        if (!value.is_array() || value.size() != 3)
        {
            throw std::runtime_error(
                "Ragdoll field '" +
                fieldName +
                "' must contain exactly 3 values"
            );
        }

        return Vector3(
            value[0].get<float>(),
            value[1].get<float>(),
            value[2].get<float>()
        );
    }
}

RagdollAssetID RagdollManager::Load(
    const std::filesystem::path& path)
{
    const std::string key =
        std::filesystem::weakly_canonical(path).string();

    auto existing = m_PathToID.find(key);

    if (existing != m_PathToID.end())
        return existing->second;

    RagdollAsset asset = LoadFromFile(path);

    RagdollAssetID id;
    id.value = static_cast<uint32_t>(m_Assets.size());

    m_Assets.push_back(std::move(asset));

    m_PathToID.emplace(key, id);

    return id;
}

RagdollAsset RagdollManager::LoadFromFile(
    const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open ragdoll asset: " +
            path.string()
        );
    }

    json root;

    try
    {
        file >> root;
    }
    catch (const json::parse_error& e)
    {
        throw std::runtime_error(
            "Failed to parse ragdoll asset '" +
            path.string() +
            "': " +
            e.what()
        );
    }

    if (!root.contains("asset"))
    {
        throw std::runtime_error(
            "Ragdoll asset is missing 'asset': " +
            path.string()
        );
    }

    if (!root.contains("version"))
    {
        throw std::runtime_error(
            "Ragdoll asset is missing 'version': " +
            path.string()
        );
    }

    const int version =
        root.at("version").get<int>();

    if (version != 1)
    {
        throw std::runtime_error(
            "Unsupported ragdoll asset version " +
            std::to_string(version) +
            " in " +
            path.string()
        );
    }

    if (!root.contains("bodies") ||
        !root.at("bodies").is_array())
    {
        throw std::runtime_error(
            "Ragdoll asset must contain a 'bodies' array: " +
            path.string()
        );
    }

    RagdollAsset asset;

    asset.name =
        root.at("asset").get<std::string>();

    for (size_t i = 0;
         i < root.at("bodies").size();
         ++i)
    {
        const json& bodyJson =
            root.at("bodies")[i];

        if (!bodyJson.contains("bone"))
        {
            throw std::runtime_error(
                "Ragdoll body " +
                std::to_string(i) +
                " is missing 'bone'"
            );
        }

        if (!bodyJson.contains("shape"))
        {
            throw std::runtime_error(
                "Ragdoll body '" +
                bodyJson.at("bone").get<std::string>() +
                "' is missing 'shape'"
            );
        }

        

        RagdollBodyDef body;

        body.boneName =
            bodyJson.at("bone").get<std::string>();

        body.mass =
            bodyJson.value("mass", 1.0f);

        body.friction =
            bodyJson.value("friction", 0.5f);

        body.restitution =
            bodyJson.value("restitution", 0.0f);

        if (body.mass <= 0.0f)
        {
            throw std::runtime_error(
                "Ragdoll body '" +
                body.boneName +
                "' has invalid mass"
            );
        }

        body.localOffset = bodyJson.contains("localOffset")
            ? parseVector3(bodyJson.at("localOffset"), "localOffset")
            : Vector3{};

        if (bodyJson.contains("localRotationOffset"))
        {
            const json& q = bodyJson.at("localRotationOffset");
            body.localRotationOffset = Quat(
                q[0].get<float>(), q[1].get<float>(),
                q[2].get<float>(), q[3].get<float>());
        }

        if (bodyJson.contains("joint"))
        {
            const json& j = bodyJson.at("joint");
            body.joint.motorFrequency = j.value("motorFrequency", 6.0f);
            body.joint.motorDamping = j.value("motorDamping", 1.0f);
            body.joint.maxMotorTorque = j.value("maxMotorTorque", 800.0f);
            body.joint.breakForceThreshold = j.value("breakForceThreshold", 4000.0f);
            body.joint.health = j.value("health", 100.0f);
        }

        const json& shapeJson =
            bodyJson.at("shape");

        if (!shapeJson.contains("type"))
        {
            throw std::runtime_error(
                "Ragdoll body '" +
                body.boneName +
                "' shape is missing 'type'"
            );
        }

        const std::string shapeType =
            shapeJson.at("type").get<std::string>();

        body.shape.type =
            parseShapeType(shapeType);

        switch (body.shape.type)
        {
        case RagdollShapeType::Capsule:
        {
            if (!shapeJson.contains("radius") ||
                !shapeJson.contains("halfHeight"))
            {
                throw std::runtime_error(
                    "Capsule body '" +
                    body.boneName +
                    "' requires radius and halfHeight"
                );
            }

            body.shape.radius =
                shapeJson.at("radius").get<float>();

            body.shape.halfHeight =
                shapeJson.at("halfHeight").get<float>();

            if (body.shape.radius <= 0.0f ||
                body.shape.halfHeight <= 0.0f)
            {
                throw std::runtime_error(
                    "Capsule body '" +
                    body.boneName +
                    "' has invalid dimensions"
                );
            }

            break;
        }

        case RagdollShapeType::Box:
        {
            if (!shapeJson.contains("halfExtent"))
            {
                throw std::runtime_error(
                    "Box body '" +
                    body.boneName +
                    "' requires halfExtent"
                );
            }

            body.shape.halfExtent =
                parseVector3(
                    shapeJson.at("halfExtent"),
                    "halfExtent"
                );

            break;
        }

        case RagdollShapeType::Sphere:
        {
            if (!shapeJson.contains("radius"))
            {
                throw std::runtime_error(
                    "Sphere body '" +
                    body.boneName +
                    "' requires radius"
                );
            }

            body.shape.radius =
                shapeJson.at("radius").get<float>();

            if (body.shape.radius <= 0.0f)
            {
                throw std::runtime_error(
                    "Sphere body '" +
                    body.boneName +
                    "' has invalid radius"
                );
            }

            break;
        }
        }

        asset.bodies.push_back(
            std::move(body)
        );
    }

    return asset;
}

RagdollAsset* RagdollManager::Get(
    RagdollAssetID id)
{
    if (!id.valid())
        return nullptr;

    if (id.value >= m_Assets.size())
        return nullptr;

    return &m_Assets[id.value];
}

const RagdollAsset* RagdollManager::Get(
    RagdollAssetID id) const
{
    if (!id.valid())
        return nullptr;

    if (id.value >= m_Assets.size())
        return nullptr;

    return &m_Assets[id.value];
}

void RagdollManager::Unload(
    RagdollAssetID id)
{
    if (!id.valid())
        return;

    if (id.value >= m_Assets.size())
        return;

    m_Assets[id.value] = RagdollAsset{};
}

void RagdollManager::Clear()
{
    m_Assets.clear();
    m_PathToID.clear();
}