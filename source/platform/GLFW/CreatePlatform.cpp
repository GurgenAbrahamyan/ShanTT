// source/platform/GLFW/CreatePlatform.cpp
#include "platform/IPlatform.h"
#include "PlatformGLFW.h"

std::unique_ptr<IPlatform> CreatePlatform()
{
    return std::make_unique<PlatformGLFW>();
}