#include "GraphicsEntityFactory.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/TagComponent.h"
#include "../components/graphics/LightComponent.h"
#include "../components/graphics/CubeMapComponent.h"
#include "../components/graphics/MeshComponent.h"
#include "../../resources/managers/MeshManager.h"
#include "../../resources/managers/MaterialManager.h"
#include "../components/graphics/CameraComponent.h"
#include "../components/graphics/ActiveCameraTag.h"
#include "../components/graphics/ModelComponent.h"
#include "../../resources/data/ModelAsset.h"
namespace GraphicsEntityFactory {

    entt::entity createRectangle(
        entt::registry& registry,
        MeshManager& meshManager,
        MaterialManager& materialManager,
        const std::string& tag,
        Vector3 position,
        Vector3 scale
        )
    {
        auto entity = registry.create();
        registry.emplace<TagComponent>(entity, tag);
      //  registry.emplace<TransformComponent>(entity, position, Quat(), scale);
        ModelComponent component;
        
        ModelAsset* asset = new ModelAsset();
        asset->meshes.push_back({ meshManager.getRectangleMesh(),materialManager.getRectangleMaterial(), Mat4()});
        component.asset = asset;
        registry.emplace<ModelComponent>(entity, component);
        registry.emplace<TransformComponent>(entity,  position, Quat(), scale );
        return entity;
    }

    entt::entity createLight(
        entt::registry& registry,
		Vector3 position,
        LightComponent ligthComp,
        const std::string& tag)
    {
        auto entity = registry.create();
        registry.emplace<TagComponent>(entity, tag);
        registry.emplace<TransformComponent>(entity, position, Quat(), Vector3(1, 1, 1));
        registry.emplace<LightComponent>(entity, ligthComp);
        return entity;
    }

    entt::entity createSkybox(
        entt::registry& registry,
        CubeMap* cubeMap)
    {
        auto view = registry.view<CubeMapComponent>();
        assert(view.size() == 0 && "Only one skybox allowed!");
        auto entity = registry.create();
        registry.emplace<TagComponent>(entity, "skybox");
        registry.emplace<CubeMapComponent>(entity, cubeMap);
        return entity;
    }

    entt::entity createCamera(
        entt::registry& registry,
        Vector3 position,
		CameraComponent cameraComp,
        bool makeActive,
        const std::string& tag)
    {
        auto entity = registry.create();
        registry.emplace<TagComponent>(entity, tag);
        registry.emplace<TransformComponent>(entity, position, Quat(),Vector3(1, 1, 1));
        registry.emplace<CameraComponent>(entity, cameraComp);

        if (makeActive) {
            registry.clear<ActiveCameraTag>();
            registry.emplace<ActiveCameraTag>(entity);
        }

        return entity;
    }
}