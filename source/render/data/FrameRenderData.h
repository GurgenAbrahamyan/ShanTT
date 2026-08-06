#pragma once

#include <any>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <stdexcept>


class FrameRenderData
{
public:

    template<typename T>
    T& Emplace()
    {
        auto [it, inserted] =
            m_Data.emplace(
                std::type_index(typeid(T)),
                T{}
            );

        if (!inserted)
            throw std::runtime_error("FrameData: type already exists");

        return std::any_cast<T&>(it->second);
    }


    template<typename T, typename... Args>
        T& Emplace(Args&&... args)
    {
        auto [it, inserted] = m_Data.try_emplace(
            std::type_index(typeid(T)),
            std::in_place_type<T>,
            std::forward<Args>(args)...
        );

        if (!inserted)
            throw std::runtime_error("FrameData: type already exists");

        return std::any_cast<T&>(it->second);
    }
    
    template<typename T>
    T& Get()
    {
        return std::any_cast<T&>(
            m_Data.at(std::type_index(typeid(T)))
        );
    }


    template<typename T>
    const T& Get() const
    {
        return std::any_cast<const T&>(
            m_Data.at(std::type_index(typeid(T)))
        );
    }


    template<typename T>
    bool Has() const
    {
        return m_Data.contains(
            std::type_index(typeid(T))
        );
    }


    void Clear() {
        m_Data.clear();
    }


private:

    std::unordered_map<std::type_index, std::any> m_Data;
};