#pragma once
#include "../data/RenderContext.h"
#include "../backend/Shader.h"
#include "../data/RenderResource.h"



class RenderPass
{
protected:
    Shader* shader;

public:
    std::vector<RenderResource*> inputs;
    std::vector<RenderResource*> outputs;

    RenderPass(Shader* s) : shader(s) {}

    virtual void execute(RenderContext& ctx) = 0;
    virtual const char* passName() const { return "RenderPass"; }
};

