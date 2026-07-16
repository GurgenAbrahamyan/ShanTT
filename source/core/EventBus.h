#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>


class EventBus {
public:
    EventBus() = default;

   
    template<typename T>
    void subscribe(std::function<void(T&)> listener) {
        auto& vec = listeners[typeid(T)];
        vec.push_back([listener](void* event) {
            listener(*static_cast<T*>(event));
            });
    }

    template<typename T>
    void publish(T& event) {
        auto it = listeners.find(typeid(T));
        if (it != listeners.end()) {
            for (auto& func : it->second) {
                func(&event); 
            }
        }
    }

private:
    
    std::unordered_map<std::type_index, std::vector<std::function<void(void*)>>> listeners;
};


