#pragma once
#include "resources/managers/MeshHandleTypes.h"
#include "resources/managers/MaterialHandleTypes.h"

struct RenderableComponent {
    MeshRef mesh;         
    MaterialID material;
    bool visible = true;
};