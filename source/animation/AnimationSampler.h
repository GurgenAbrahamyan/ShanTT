#pragma once
#include "AnimationClip.h"
#include "math_custom/Interpolation.h"
#include <vector>

template <typename T>
T SampleKeyframes(const std::vector<float>& times, const std::vector<T>& values,
                   Interpolation interp, float t, size_t& cacheIdx,
                   T(*lerpFn)(const T&, const T&, float))
{
    if (times.empty()) return T{};
    if (times.size() == 1 || t <= times.front()) return values.front();
    if (t >= times.back()) return values.back();

    if (cacheIdx >= times.size() - 1 || times[cacheIdx] > t) cacheIdx = 0;
    while (cacheIdx + 1 < times.size() && times[cacheIdx + 1] < t) ++cacheIdx;

    float t0 = times[cacheIdx], t1 = times[cacheIdx + 1];
    float alpha = (t - t0) / std::max(t1 - t0, 1e-6f);

    if (interp == Interpolation::Step) return values[cacheIdx];
    return lerpFn(values[cacheIdx], values[cacheIdx + 1], alpha);
}

inline SampledValue SampleChannel(const Channel& ch, float t, size_t& cacheIdx)
{
    return std::visit([&](auto&& c) -> SampledValue {
        using T = std::decay_t<decltype(c)>;
        if constexpr (std::is_same_v<T, Vec3Channel>)
            return SampleKeyframes(c.times, c.values, c.interp, t, cacheIdx, Math::Lerp);
        else if constexpr (std::is_same_v<T, QuatChannel>)
            return SampleKeyframes(c.times, c.values, c.interp, t, cacheIdx, Math::Nlerp);
        else
            return SampleKeyframes(c.times, c.values, c.interp, t, cacheIdx, Math::LerpFloat);
    }, ch);
}

inline std::vector<SampledValue> SampleTrack(const Track& track, float t, std::vector<size_t>& cacheIndices)
{
    if (cacheIndices.size() != track.channels.size())
        cacheIndices.assign(track.channels.size(), 0);

    std::vector<SampledValue> out;
    out.reserve(track.channels.size());
    for (size_t i = 0; i < track.channels.size(); ++i)
        out.push_back(SampleChannel(track.channels[i], t, cacheIndices[i]));
    return out;
}