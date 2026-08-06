#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <algorithm>

class EventBus {
public:
    EventBus() = default;

    template<typename EventT>
    void subscribe(void* owner, std::function<void(const EventT&)> listener)
    {
        auto& vec = listeners[typeid(EventT)];
        vec.push_back({
            owner,
            [listener](const void* event) {
                listener(*static_cast<const EventT*>(event));
            }
        });
    }

    template<typename EventT>
    void unsubscribe(void* owner)
    {
        auto it = listeners.find(typeid(EventT));
        if (it == listeners.end()) return;

        auto& vec = it->second;
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [owner](const Listener& l) { return l.owner == owner; }),
            vec.end());
    }

    void unsubscribeAll(void* owner)
    {
        for (auto& [type, vec] : listeners)
        {
            vec.erase(
                std::remove_if(vec.begin(), vec.end(),
                    [owner](const Listener& l) { return l.owner == owner; }),
                vec.end());
        }
    }

    template<typename EventT>
    void publish(const EventT& event)
    {
        auto it = listeners.find(typeid(EventT));
        if (it == listeners.end()) return;

        for (size_t i = 0; i < it->second.size(); ++i)
            it->second[i].func(&event);
    }

private:
    struct Listener
    {
        void* owner;
        std::function<void(const void*)> func;
    };

    std::unordered_map<std::type_index, std::vector<Listener>> listeners;
};