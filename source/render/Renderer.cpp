#include "Renderer.h"
#include "RenderGraph.h"

Renderer::Renderer()
    :
    shaderManager(nullptr),
    m_ResourceAllocator(),
    m_RenderGraph(m_ResourceAllocator)
{
    shaderManager = new ShaderManager();
}

void Renderer::Init(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDepthFunc(GL_LESS);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}
Renderer::~Renderer() {
    delete shaderManager;
}



void Renderer::render(const FrameRenderData& frameData) const 
{
    m_RenderGraph.execute(frameData, debugData );
}