#pragma once
#include "../data/RenderContext.h"
#include "../data/GPULight.h"
#include "../../ecs/components/graphics/LightComponent.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../../math_custom/Mat4.h"

#include "ShadowAtlas.h"

struct ShadowSettings {
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float orthoWidth = 15.0f;
    float orthoHeight = 15.0f;
};

class ShadowSystem {
public:
    
    ShadowSettings settings;
    ShadowAtlas atlas;

    ShadowSystem() {}

    void update(RenderContext* ctx) {
        ctx->shadowCasters.clear();
        ctx->shadowData.clear(); 

        atlas.freeAll();

        for (auto& light : ctx->lights) {
            if (light.shadowIndex == -1) continue;

            if (light.type == 1) { 
                int tileIdx = atlas.allocateBestFit();
                if (tileIdx < 0) continue;

                light.shadowIndex = tileIdx;
                ShadowTile& tile = atlas.getTile(tileIdx);
                Mat4 matrix = calcDirectional(light);

                
                ShadowData data;
                data.lightMatrix = matrix;
                data.uvMin = tile.uvMin;
                data.uvMax = tile.uvMax;

                ctx->shadowData.push_back(data);
                ctx->shadowCasters.push_back(&light);
            }
            else if (light.type == 2) { // spot
                int tileIdx = atlas.allocateBestFit();
                if (tileIdx < 0) continue;

                light.shadowIndex = tileIdx;
                ShadowTile& tile = atlas.getTile(tileIdx);
                Mat4 matrix = calcSpot(light);

                ShadowData data;
                data.lightMatrix = matrix;
                data.uvMin = tile.uvMin;
                data.uvMax = tile.uvMax;

                ctx->shadowData.push_back(data);
                ctx->shadowCasters.push_back(&light);
            }
            else if (light.type == 0) { 
                int tileIdx = atlas.allocateBestFitConsecutive(6);
                if (tileIdx < 0) continue;

                light.shadowIndex = tileIdx;
                std::vector<Mat4> faces = calcPoint(light);

                for (int f = 0; f < 6; f++) {
                    ShadowTile& tile = atlas.getTile(tileIdx + f);

                    ShadowData data;
                    data.lightMatrix = faces[f];
                    data.uvMin = tile.uvMin;
                    data.uvMax = tile.uvMax;

                    ctx->shadowData.push_back(data);
                }
                ctx->shadowCasters.push_back(&light);
            }
        }
    }
private:
    Mat4 calcDirectional(const GPULight& light) {
        Vector3 lightDir = light.direction.normalized();
        Vector3 lightPos = light.position;
        Vector3 center = lightPos + lightDir * 10.0f;
        Vector3 up = (fabs(lightDir.y) > 0.99f)
            ? Vector3(0, 0, 1) : Vector3(0, 1, 0);
        Mat4 view = Mat4::lookAt(lightPos, center, up);
        Mat4 proj = Mat4::ortho(
            -settings.orthoWidth, settings.orthoWidth,
            -settings.orthoHeight, settings.orthoHeight,
            settings.nearPlane, settings.farPlane);
        return proj*view;
    }

    Mat4 calcSpot(const GPULight& light) {
        Vector3 lightDir = light.direction.normalized();
        Vector3 lightPos = light.position;
        Vector3 center = lightPos + lightDir * 10.0f;

        Vector3 up = (fabs(lightDir.y) > 0.99f)
            ? Vector3(0, 0, 1) : Vector3(0, 1, 0);
        Mat4 view = Mat4::lookAt(lightPos, center, up);
        float fovRad = acos(light.innerCone) * 2.0f; 
        float fovDeg = fovRad * (180.0f / 3.14159f);


        Mat4 proj = Mat4::perspective(fovDeg, 1.0f, settings.nearPlane, settings.farPlane);
        /*Mat4 proj = Mat4::ortho(
            -settings.orthoWidth, settings.orthoWidth,
            -settings.orthoHeight, settings.orthoHeight,
            settings.nearPlane, settings.farPlane);*/
        return proj*view;
    }

    std::vector<Mat4> calcPoint(const GPULight& light) {
        std::vector<Mat4> all;
        all.push_back(calcPointFace(Vector3(1, 0, 0), Vector3(0, -1, 0), light.position));
        all.push_back(calcPointFace(Vector3(-1, 0, 0), Vector3(0, -1, 0), light.position));
        all.push_back(calcPointFace(Vector3(0, 1, 0), Vector3(0, 0, 1), light.position));
        all.push_back(calcPointFace(Vector3(0, -1, 0), Vector3(0, 0, -1), light.position));
        all.push_back(calcPointFace(Vector3(0, 0, 1), Vector3(0, -1, 0), light.position));
        all.push_back(calcPointFace(Vector3(0, 0, -1), Vector3(0, -1, 0), light.position));
        return all;
    }

    Mat4 calcPointFace(Vector3 dir, Vector3 up, Vector3 pos) {
        Mat4 view = Mat4::lookAt(pos, pos + dir, up);
        Mat4 proj = Mat4::perspective(90.3f, 1.0f, settings.nearPlane, settings.farPlane);
        return proj*view;
    }
};