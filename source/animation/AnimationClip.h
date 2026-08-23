#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Track.h"

struct AnimationClip {
    std::string name;
    float duration = 0.0f;
    std::vector<Track> tracks;
    uint32_t version = 0; 

    std::unordered_map<int, std::string> trackSourceNames;

    int AddTrack() {
        tracks.emplace_back();
        ++version;
        return static_cast<int>(tracks.size()) - 1;
    }

    void RemoveTrack(int trackIndex) {
        if (trackIndex >= 0 && trackIndex < (int)tracks.size()) {
            tracks.erase(tracks.begin() + trackIndex);
            ++version;
        }
    }

    void RecalculateDuration() {
        duration = 0.0f;
        for (auto& track : tracks)
            for (auto& ch : track.channels)
                std::visit([&](auto&& c) {
                    if (!c.times.empty())
                        duration = std::max(duration, c.times.back());
                }, ch);
    }
};