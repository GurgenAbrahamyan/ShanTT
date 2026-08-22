#pragma once
#include <vector>

#include "Bone.h"

struct Skeleton{
    std::string name;
    std::vector<Bone> bones;
};