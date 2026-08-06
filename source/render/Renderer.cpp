#include "Renderer.h"


Renderer::Renderer( EventBus* bus, EngineResources* ctx) :
    bus(bus),
    shaderManager(nullptr),
    ctx(ctx),
    graph(nullptr)
{

    shaderManager = new ShaderManager(bus);


    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

   
}

Renderer::~Renderer() {

    delete shaderManager;
}



void Renderer::render(const FrameRenderData& frameData) const
{
    clearFramebuffers();

    graph->execute(frameData, *ctx , debugData );

}


void Renderer::clearFramebuffers() const
{

    if (m_MainFrameBuffer) {
        m_MainFrameBuffer->bind();
        glViewport(0, 0, m_MainFrameBuffer->getWidth(), m_MainFrameBuffer->getHeight());
        glClearColor(0.67f, 0.67f, 0.67f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_MainFrameBuffer->unbind();
    }


    if (m_BlurFrameBuffer) {
        m_BlurFrameBuffer->bind();
        glViewport(0, 0, m_BlurFrameBuffer->getWidth(), m_BlurFrameBuffer->getHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_BlurFrameBuffer->unbind();
    }


    if (m_ShadowFrameBuffer) {
        m_ShadowFrameBuffer->bind();
        glViewport(0, 0, m_ShadowFrameBuffer->getWidth(), m_ShadowFrameBuffer->getHeight());
        glClearDepth(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, ctx->windowSize.x, ctx->windowSize.y);
        m_ShadowFrameBuffer->unbind();
    }

    if (m_LightFrameBuffer) {

        m_LightFrameBuffer->bind();
        glViewport(0, 0, m_LightFrameBuffer->getWidth(), m_LightFrameBuffer->getHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        m_LightFrameBuffer->unbind();


    }



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.67f, 0.67f, 0.67f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}