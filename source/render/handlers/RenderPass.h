#pragma once
#include "../data/RenderContext.h"
#include "../data/BatchMap.h"
#include "../backend/containers/GlobalUniformBuffer.h"
#include "../data/GPULight.h"
#include "../../math_custom/Mat4.h"
#include "../../ecs/components/core/TransformComponent.h"
#include "../../ecs/components/core/ParentComponent.h"
#include "../../ecs/components/graphics/MeshComponent.h"
#include "../../ecs/components/graphics/MaterialComponent.h"
#include "../../ecs/components/graphics/LightComponent.h"
#include "../../ecs/components/graphics/CubeMapComponent.h"
#include "../../ecs/components/graphics/CameraComponent.h"
#include "../../ecs/components/graphics/ActiveCameraTag.h"
#include "../../ecs/components/core/WorldMatrixComponent.h"
#include "../../ecs/components/graphics/ModelComponent.h"
#include "../backend/Shader.h"
#include "../../math_custom/GLAdapter.h"

#include "../data/RenderResource.h"
#include <glad/glad.h>


class RenderPass
{
protected:
    Shader* shader;

public:
    std::vector<RenderResource*> inputs;
    std::vector<RenderResource*> outputs;

    RenderPass(Shader* s) : shader(s) {}

    virtual void execute(RenderContext& ctx) = 0;
};




class ShadowPass : public RenderPass
{
public:
    ShadowPass(Shader* s, int tileSize = 1024)
        : RenderPass(s), tileSize(tileSize)
    {
       
    }

    void execute(RenderContext& ctx) override
    {
        if (outputs.empty() || ctx.shadowCasters.empty()) return;

        int numCasters = ctx.shadowCasters.size();

        // expand framebuffer if needed
        FrameBuffer* fb = outputs[0]->framebuffer;
        if (numCasters > currentSlots)
        {
            fb->resize(tileSize * numCasters, tileSize);
            currentSlots = numCasters;
        }

        shader->Activate();
        fb->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        for (auto* light : ctx.shadowCasters)
        {
            int idx = light->shadowIndex;
            glViewport(idx * tileSize, 0, tileSize, tileSize);

            shader->setMat4("lightSpaceMatrix", light->lightSpaceMatrix); 


            for (auto& [mat, meshMap] : ctx.batches)
            {
                for (auto& [mesh, batch] : meshMap)
                {
                    if (batch.instances.empty()) continue;
                    mesh->bind();
                    mesh->setupInstanceVBO(batch.instances.size());
                    glBindBuffer(GL_ARRAY_BUFFER, mesh->getInstanceVBO());
                    glBufferSubData(GL_ARRAY_BUFFER, 0,
                        batch.instances.size() * sizeof(Mat4),
                        batch.instances.data());
                    glDrawElementsInstanced(GL_TRIANGLES,
                        mesh->indexCount(), GL_UNSIGNED_INT, 0,
                        batch.instances.size());
                }
            }
        }

        fb->unbind();
        glViewport(0, 0, ctx.windowWidth, ctx.windowHeight);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    float getTileWidth() const { return atlasTileWidth; }
    

  
private:
  
	int currentSlots = 0;
    int tileSize;
    float atlasTileWidth = 0;
    int shadowCount = 0;



   
};
class GeometryPass : public RenderPass {
public:
    GeometryPass(Shader* s) : RenderPass(s) {}

    void execute(RenderContext& ctx) override
    {

        if (outputs.empty()) return; // single output
        FrameBuffer* fb = outputs[0]->framebuffer;
        fb->bind();
		glDisable(GL_CULL_FACE);
        shader->Activate();
        uploadLights(&ctx);

       
        shader->setMat4("view", ctx.camera->viewMatrix);
        shader->setMat4("projection", ctx.camera->projectionMatrix);
        shader->setVec3("cameraPos", GLAdapter::toGL(ctx.cameraTransform->position));

        
        if (!inputs.empty()) {

           
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, inputs[0]->framebuffer->getDepthAttachment());
            shader->setInt("shadowMap", 7);
		 	shader->setInt("shadowCasterCount", (int)ctx.shadowCasters.size());
        }
       
        
      
        
        for (auto& [mat, meshMap] : ctx.batches)
        {
            mat->Bind(shader);

            

           for (auto& [mesh, batch] : meshMap)
            {
                if (batch.instances.empty()) continue;

                mesh->bind();
                mesh->setupInstanceVBO(batch.instances.size());
                glBindBuffer(GL_ARRAY_BUFFER, mesh->getInstanceVBO());
                glBufferSubData(GL_ARRAY_BUFFER, 0, batch.instances.size() * sizeof(Mat4),
                    batch.instances.data());

                glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount(),
                    GL_UNSIGNED_INT, 0, batch.instances.size());
            }
        }

        fb->unbind();
    }

private:
    UniformBuffer* lightsUBO = new UniformBuffer(
        sizeof(GPULight) * 32 + sizeof(int) + 12, 1
    );

    void uploadLights(RenderContext* ctx)
    {
        auto& gpuLights = ctx->lights;

        lightsUBO->bind();
		
        if (!gpuLights.empty())
        {
            lightsUBO->update(
                gpuLights.data(),
                gpuLights.size() * sizeof(GPULight),
                0
            );
            
        }

        int lightCount = (int)gpuLights.size();
        lightsUBO->update(&lightCount, sizeof(int), sizeof(GPULight) * 32);
    }
};



class CubeMapPass : public RenderPass {
public:
    CubeMapPass(Shader* s) : RenderPass(s) {
		
    }

    void execute(RenderContext& context) override {

      
        FrameBuffer* fb = outputs[0]->framebuffer;
        fb->bind();

     //   glDisable(GL_CULL_FACE);
    
        auto& registry = *context.registry;
    
      

        auto* cam = context.camera;
        auto* camTrans = context.cameraTransform;
        if (!cam || !camTrans) return;

        auto skyView = registry.view<CubeMapComponent>();
        if (skyView.empty()) return;
        CubeMapComponent& cmc = registry.get<CubeMapComponent>(skyView.front());
        if (!cmc.cubeMap) return;

        shader->Activate();

        glDepthFunc(GL_LEQUAL);
      

        Mat4 view_mat = cam->viewMatrix;
        view_mat.data[12] = 0.0f;
        view_mat.data[13] = 0.0f;
        view_mat.data[14] = 0.0f;

        shader->setInt("skybox", 0);
        shader->setMat4("projection", cam->projectionMatrix);
        shader->setMat4("view", view_mat);

        cmc.cubeMap->bind();
        cmc.cubeMap->bindTexture();
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthFunc(GL_LESS);
       // glEnable(GL_CULL_FACE);
        fb->unbind();
    }
};

class BlurPass : public RenderPass {
public:
    BlurPass(Shader* s)
        : RenderPass(s),
        quadVBO(quadVertices, sizeof(quadVertices), false)
    {

        quadVAO.Bind();
        quadVBO.Bind();

        quadVAO.LinkAttrib(quadVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
        quadVAO.LinkAttrib(quadVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        quadVAO.Unbind();
        quadVBO.Unbind();
    }

    void execute(RenderContext& context) override {
       
     //   if (!output || inputs.empty()) return;

        FrameBuffer* inputFB = inputs[0]->framebuffer;
        FrameBuffer* outputFB = outputs[0]->framebuffer;

		outputFB->bind();
		auto* cam = context.camera;
        auto* camTrans = context.cameraTransform;
        if (!cam || !camTrans) return;

      
        shader->Activate();

       
        shader->setInt("uSceneTexture", 0);
        shader->setInt("uDepthTexture", 1);

	
        shader->setFloat("uFocusDistance",  cam->focusDistance);

 
        shader->setFloat("uAperture", cam->aperture); 

     
        shader->setFloat("uFocalLength", cam->focalLength);

		shader->setFloat("uBlurScale", cam->blurScale);
       
        shader->setFloat("uNearPlane", cam->nearPlane);
        shader->setFloat("uFarPlane", cam->farPlane);

       
       

       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputFB->getColorAttachment());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, inputFB->getDepthAttachment());

      
        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();
        outputFB->unbind();
    }
private:
    VAO quadVAO;
    VBO quadVBO;

    static constexpr float quadVertices[16] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    };
};

class FinalBlitPass : public RenderPass{

public:
    FinalBlitPass(Shader* shader)
        : RenderPass(shader),
        quadVBO(quadVertices, sizeof(quadVertices), false)
    {
	
        quadVAO.Bind();
        quadVBO.Bind();

        quadVAO.LinkAttrib(quadVBO, 0, 2, GL_FLOAT,
            4 * sizeof(float), (void*)0);

        quadVAO.LinkAttrib(quadVBO, 1, 2, GL_FLOAT,
            4 * sizeof(float),
            (void*)(2 * sizeof(float)));

        quadVAO.Unbind();
        quadVBO.Unbind();
    }

    void execute(RenderContext& context) override
    {
      //  if (inputs.empty()) return;

        FrameBuffer* inputFB = inputs[0]->framebuffer;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
     

        shader->Activate();
        shader->setInt("screenTexture", 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputFB->getColorAttachment());

        quadVAO.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        quadVAO.Unbind();

        glEnable(GL_DEPTH_TEST);
    }

private:
    VAO quadVAO;
    VBO quadVBO;

    static constexpr float quadVertices[16] = {
     -1.0f,  1.0f,  0.0f, 1.0f,
     -1.0f, -1.0f,  0.0f, 0.0f,
      1.0f,  1.0f,  1.0f, 1.0f,
      1.0f, -1.0f,  1.0f, 0.0f,
    };
};
