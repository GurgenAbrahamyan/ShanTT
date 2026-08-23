#pragma once
#include <variant>
#include "ResourcePool.h"

struct StaticMeshTag {};
struct SkinnedMeshTag {};

using StaticMeshID  = Handle<StaticMeshTag>;
using SkinnedMeshID = Handle<SkinnedMeshTag>;

using MeshRef = std::variant<StaticMeshID, SkinnedMeshID>;