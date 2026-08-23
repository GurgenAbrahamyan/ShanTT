#pragma once

#include <vector>
#include <variant>

#include "math_custom/Vector3.h"
#include "math_custom/Quat.h"

enum class Interpolation { Linear, Step, CubicSpline };

struct Vec3Channel {
    std::vector<float> times;
    std::vector<Vector3> values;
    Interpolation interp = Interpolation::Linear;
};

struct QuatChannel {
    std::vector<float> times;
    std::vector<Quat> values;
    Interpolation interp = Interpolation::Linear;
};

struct FloatChannel {
    std::vector<float> times;
    std::vector<float> values;
    Interpolation interp = Interpolation::Linear;
};

using Channel = std::variant<Vec3Channel, QuatChannel, FloatChannel>;
using SampledValue = std::variant<Vector3, Quat, float>;

enum class ChannelType { Vec3, Quat, Float };

struct Track {
    std::vector<Channel> channels;

    int AddChannel(ChannelType type) {
        switch (type) {
            case ChannelType::Vec3:  channels.emplace_back(Vec3Channel{});  break;
            case ChannelType::Quat:  channels.emplace_back(QuatChannel{});  break;
            case ChannelType::Float: channels.emplace_back(FloatChannel{}); break;
        }
        return static_cast<int>(channels.size()) - 1;
    }

    void RemoveChannel(int channelIndex) {
        if (channelIndex >= 0 && channelIndex < (int)channels.size())
            channels.erase(channels.begin() + channelIndex);
    }
};