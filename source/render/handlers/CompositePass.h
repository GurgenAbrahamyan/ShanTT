#pragma once

#include "RenderPass.h"
#include "../RenderGraphBuilder.h"
#include "../PassResources.h"

#include "render/backend/Shader.h"
#include "render/backend/containers/VAO.h"
#include "render/backend/containers/VBO.h"

#include <glad/glad.h>

#include <string>
#include <vector>
#include <variant>


class CompositePass : public RenderPass
{
public:

    struct CompositeInput
    {
        ResourceId resource =
            INVALID_RESOURCE_ID;

        std::string uniformName;

        uint32_t textureUnit = 0;
    };


    struct CompositeUniform
    {
        std::string name;

        std::variant<
            float,
            int,
            bool
        > value;
    };


    struct CompositePassSettings
    {
        Shader* shader = nullptr;

        ResourceId baseInput =
            INVALID_RESOURCE_ID;

        std::vector<CompositeInput> inputs;

        std::vector<CompositeUniform> uniforms;

        uint32_t width = 0;
        uint32_t height = 0;
    };


    CompositePass(
        RenderGraphBuilder& builder,
        const CompositePassSettings& settings
    )
        : RenderPass(builder, "Composite")
        , m_Settings(settings)
        , m_Shader(settings.shader)
        , m_BaseInput(
            builder.read(settings.baseInput)
        )
        , m_QuadVBO(
            quadVertices,
            sizeof(quadVertices),
            false
        )
    {

        m_InputHandles.reserve(
            settings.inputs.size()
        );

        for (const auto& input : settings.inputs)
        {
            m_InputHandles.push_back(
                builder.read(input.resource)
            );
        }

        m_Output_Color =
            builder.create(
                "CompositePass.Color",
                {
                    TextureResourceDesc{
                        settings.width,
                        settings.height,
                        { .internalFormat = GL_RGBA16F}
                    }
                }
            );

        m_Output =
            builder.create(
                "CompositePass.Buffer",
                FrameBufferResourceDesc{
                    {m_Output_Color}
                }
            );


        m_QuadVAO.Bind();
        m_QuadVBO.Bind();

        m_QuadVAO.LinkAttrib(
            m_QuadVBO,
            0,
            2,
            GL_FLOAT,
            4 * sizeof(float),
            (void*)0
        );

        m_QuadVAO.LinkAttrib(
            m_QuadVBO,
            1,
            2,
            GL_FLOAT,
            4 * sizeof(float),
            (void*)(2 * sizeof(float))
        );

        m_QuadVAO.Unbind();
        m_QuadVBO.Unbind();
    }


    void execute(
        const FrameRenderData&,
        PassResources& resources,
        const DebugRenderData&
    ) override
    {
        if (!m_Shader)
            return;

        if (m_BaseInput == INVALID_RESOURCE_ID ||
            m_Output == INVALID_RESOURCE_ID)
        {
            return;
        }


        Texture* baseInput =
            resources.get<Texture>(
                m_BaseInput
            );

        FrameBuffer* output =
            resources.get<FrameBuffer>(
                m_Output
            );

        if (!baseInput || !output)
            return;


        output->bind();

        glViewport(
            0,
            0,
            output->getWidth(),
            output->getHeight()
        );


        m_Shader->Activate();

        m_Shader->setInt(
            "uBaseInput",
            0
        );

        baseInput->Bind(0);

        // m_InputHandles[i] <-> m_Settings.inputs[i], same index
        for (size_t i = 0;
             i < m_Settings.inputs.size();
             ++i)
        {
            const ResourceId handle = m_InputHandles[i];

            if (handle == INVALID_RESOURCE_ID)
                continue;

            Texture* resourceInput =
                resources.get<Texture>(handle);

            if (!resourceInput)
                continue;

            const auto& inputSettings =
                m_Settings.inputs[i];

            m_Shader->setInt(
                inputSettings.uniformName.c_str(),
                inputSettings.textureUnit
            );

            resourceInput->Bind(
                inputSettings.textureUnit
            );
        }

        for (const auto& uniform : m_Settings.uniforms)
        {
            std::visit(
                [&](const auto& value)
                {
                    using T =
                        std::decay_t<decltype(value)>;

                    if constexpr (
                        std::is_same_v<T, float>
                    )
                    {
                        m_Shader->setFloat(
                            uniform.name.c_str(),
                            value
                        );
                    }
                    else if constexpr (
                        std::is_same_v<T, int>
                    )
                    {
                        m_Shader->setInt(
                            uniform.name.c_str(),
                            value
                        );
                    }
                    else if constexpr (
                        std::is_same_v<T, bool>
                    )
                    {
                        m_Shader->setInt(
                            uniform.name.c_str(),
                            value
                        );
                    }
                },
                uniform.value
            );
        }


        m_QuadVAO.Bind();

        glDrawArrays(
            GL_TRIANGLE_STRIP,
            0,
            4
        );

        m_QuadVAO.Unbind();

        output->unbind();
    }


    ResourceId output()
    {
        return m_Output_Color;
    }

    CompositePassSettings& getSettings(){
        return m_Settings;
    }

private:

    CompositePassSettings m_Settings;

    Shader* m_Shader = nullptr;

    ResourceId m_BaseInput =
        INVALID_RESOURCE_ID;

    std::vector<ResourceId> m_InputHandles;

    ResourceId m_Output =
        INVALID_RESOURCE_ID;

    ResourceId m_Output_Color =
        INVALID_RESOURCE_ID;


    VAO m_QuadVAO;

    VBO m_QuadVBO;


    static constexpr float
        quadVertices[16] =
    {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };
};