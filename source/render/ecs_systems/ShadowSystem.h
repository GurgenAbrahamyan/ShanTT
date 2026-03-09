#pragma once
#include "../data/RenderContext.h"
#include "../data/GPULight.h"
#include "../../ecs/components/graphics/LightComponent.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../../math_custom/Mat4.h"
#include "../../math_custom/GLAdapter.h"

struct ShadowSettings {
    float nearPlane = 0.1f;
    float farPlane = 50.0f;
    float orthoWidth = 30.0f;
    float orthoHeight = 30.0f;
};

class ShadowSystem {
public:
    ShadowSettings settings;

    void update(RenderContext* ctx) {
        ctx->shadowCasters.clear();
       
        for (auto& light : ctx->lights) {
            if (light.shadowIndex == -1) continue; 

            if (light.type == 1)      // directional
                light.lightSpaceMatrix = calcDirectional(light);
            else if (light.type == 2) // spot
                light.lightSpaceMatrix = calcSpot(light);

            ctx->shadowCasters.push_back(&light);
        }
    }

private:
    Mat4 calcDirectional(const GPULight& light) {
        Vector3 lightDir = light.direction.normalized();
        Vector3 lightPos = light.position;
        Vector3 center = lightPos + lightDir * 10.0f;
        Vector3 up = (fabs(lightDir.y) > 0.99f)
            ? Vector3(0, 0, 1)
            : Vector3(0, 1, 0);

        Mat4 view = Mat4::lookAt(lightPos, center, up);
        Mat4 proj = Mat4::ortho(
            -settings.orthoWidth, settings.orthoWidth,
            -settings.orthoHeight, settings.orthoHeight,
            settings.nearPlane, settings.farPlane
        );

        return view*proj;
    }

    Mat4 calcSpot(const GPULight& light) {
        Vector3 lightDir = light.direction.normalized();
        Vector3 lightPos = light.position;
        Vector3 center = lightPos + lightDir * 10.0f;
        Vector3 up = (fabs(lightDir.y) > 0.99f)
            ? Vector3(0, 0, 1)
            : Vector3(0, 1, 0);

        Mat4 view = Mat4::lookAt(lightPos, center, up);
        float fovRad = acos(light.outerCone) * 2.0f;
        float fovDeg = fovRad * (180.0f / 3.14159f);
        Mat4 proj = Mat4::perspective(fovDeg, 1.0f, settings.nearPlane, settings.farPlane);

        return  view* proj;
    }
};