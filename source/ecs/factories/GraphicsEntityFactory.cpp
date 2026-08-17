#include "GraphicsEntityFactory.h"
#include "../components/core/TransformComponent.h"
#include "../components/core/TagComponent.h"
#include "../components/graphics/LightComponent.h"
#include "../components/graphics/CubeMapComponent.h"
#include "../components/graphics/CameraComponent.h"
#include "../components/graphics/ActiveCameraTag.h"
namespace GraphicsEntityFactory {

    entt::entity createLight(
        entt::registry& registry,
		Vector3 position,
        Quat rotation,
        LightComponent ligthComp,
        const std::string& tag)
    {
        auto entity = registry.create();
        registry.emplace<TagComponent>(entity, tag);
        registry.emplace<TransformComponent>(entity, position, rotation, Vector3(1, 1, 1));
        registry.emplace<LightComponent>(entity, ligthComp);
        return entity;
    }

    entt::entity createSkybox(
        entt::registry& registry,
        EnvironmentMap& map)
    {
        auto entity = registry.create();
        registry.emplace<TagComponent>(entity, "skybox");
        registry.emplace<CubeMapComponent>(entity, map);
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