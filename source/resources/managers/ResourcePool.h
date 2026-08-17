#pragma once
#include <vector>
#include <cstdint>
#include <utility>


template<typename Tag>
struct Handle {
    uint32_t index = 0;
    uint32_t generation = 0; // 0 == never-assigned / invalid

    bool isValid() const { return generation != 0; }
    bool operator==(const Handle&) const = default;
};


template<typename T, typename Tag>
class ResourcePool {
public:
    using HandleType = Handle<Tag>;

    HandleType insert(T&& value) {
        if (!freeList.empty()) {
            uint32_t idx = freeList.back();
            freeList.pop_back();
            Slot& s = slots[idx];
            s.value = std::move(value);
            s.alive = true;
            return HandleType{ idx, s.generation };
        }
        slots.push_back(Slot{ std::move(value), 1, true });
        return HandleType{ static_cast<uint32_t>(slots.size() - 1), 1 };
    }

    T* get(HandleType h) {
        if (!h.isValid() || h.index >= slots.size()) return nullptr;
        Slot& s = slots[h.index];
        if (!s.alive || s.generation != h.generation) return nullptr;
        return &s.value;
    }

    const T* get(HandleType h) const {
        if (!h.isValid() || h.index >= slots.size()) return nullptr;
        const Slot& s = slots[h.index];
        if (!s.alive || s.generation != h.generation) return nullptr;
        return &s.value;
    }

    void remove(HandleType h) {
        if (!h.isValid() || h.index >= slots.size()) return;
        Slot& s = slots[h.index];
        if (!s.alive || s.generation != h.generation) return;
        s.alive = false;
        s.generation++;
        s.value = T{};
        freeList.push_back(h.index);
    }

    size_t size() const { return slots.size(); }

    template<typename Fn>
    void forEachAlive(Fn&& fn) {
        for (uint32_t i = 0; i < slots.size(); ++i) {
            if (slots[i].alive) fn(HandleType{ i, slots[i].generation }, slots[i].value);
        }
    }

private:
    struct Slot {
        T value;
        uint32_t generation = 0;
        bool alive = false;
    };
    std::vector<Slot> slots;
    std::vector<uint32_t> freeList;
};