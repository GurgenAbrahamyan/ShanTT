#pragma once
#include "ResourcePool.h"

struct TextureTag {};
struct CubeMapTag {};
using TextureID = Handle<TextureTag>;
using CubeMapID = Handle<CubeMapTag>;