#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <vector>

#include "IExtractionSystem.h"
#include "render/data/FrameRenderData.h"

class SceneExtractor
{
public:

    using ExtractionFn = std::function<void(entt::registry&, FrameRenderData&)>;

    SceneExtractor() = default;
    ~SceneExtractor() { Shutdown(); }

    SceneExtractor(const SceneExtractor&) = delete;
    SceneExtractor& operator=(const SceneExtractor&) = delete;

    SceneExtractor(SceneExtractor&&) = default;
    SceneExtractor& operator=(SceneExtractor&&) = default;

    void OnCreate(entt::registry& registry){
        for(auto& system : m_Systems)
            system->onInit(registry);
    }

    template<typename T, typename... Args>
    T* addSystem(Args&&... args)
    {
        static_assert(
            std::is_base_of_v<IExtractionSystem, T>,
            "T must derive from IExtractionSystem"
        );

        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = system.get();

        m_Systems.push_back(std::move(system));
        return ptr;
    }

    void addFunction(ExtractionFn fn)
    {
        m_Functions.push_back(std::move(fn));
    }

    void extract(entt::registry& registry, FrameRenderData& out)
    {
        for (auto& system : m_Systems)
            system->extract(registry, out);

        for (auto& fn : m_Functions)
            fn(registry, out);
    }

    void Shutdown()
    {
        for (auto& system : m_Systems)
            system->onShutdown();

        m_Systems.clear();
        m_Functions.clear();
    }

    size_t SystemCount() const { return m_Systems.size(); }

private:

    std::vector<std::unique_ptr<IExtractionSystem>> m_Systems;
    std::vector<ExtractionFn> m_Functions;
};