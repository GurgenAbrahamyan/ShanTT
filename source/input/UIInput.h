#pragma once

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <algorithm>
#include <string>
#include <cctype>

#include "../render/RenderGraph.h"
#include "../core/EventBus.h"
#include "../render/data/DebugRenderData.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "../ecs/components/core/TagComponent.h"
#include "../ecs/components/core/ParentComponent.h"
#include "../ecs/components/core/TransformComponent.h"
#include "../ecs/components/core/WorldMatrixComponent.h"
#include "../ecs/components/graphics/ActiveCameraTag.h"
#include "../ecs/components/graphics/CameraComponent.h"
#include "../ecs/components/graphics/CubeMapComponent.h"
#include "../ecs/components/graphics/LightComponent.h"
#include "../ecs/components/graphics/MaterialComponent.h"
#include "../ecs/components/physics/CollisionShapeComponent.h"
#include "../ecs/components/physics/RigidBodyComponent.h"
#include "../ecs/components/physics/SoftBodyComponent.h"
#include "ecs/components/graphics/Renderable.h"
#include "ecs/components/graphics/SkeletonComponent.h"

#include "platform/IPlatform.h"

#include "resources/managers/AssetManager.h"

using ComponentTypes = std::tuple<
    TagComponent,
    ParentComponent,
    TransformComponent,
    WorldMatrixComponent,
    ActiveCameraTag,
    CameraComponent,
    CubeMapComponent,
    RenderableComponent,
    SkeletonComponent,
    LightComponent,
    MaterialComponent,
    CollisionShapeComponent,
    RigidBodyComponent,
    SoftBodyComponent
>;

struct DebugWindow
{
    std::string title;

    std::function<void(bool& open)> drawFunc;

    bool open = true;
};

[[maybe_unused]] static bool BeginComponentHeader(
    const char* label,
    bool& deleteRequested,
    bool defaultOpen = true)
{
    ImGuiTreeNodeFlags flags =
        (defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0)
        | ImGuiTreeNodeFlags_Framed
        | ImGuiTreeNodeFlags_SpanAvailWidth
        | ImGuiTreeNodeFlags_AllowOverlap;

    bool open = ImGui::TreeNodeEx(label, flags);

    float btnSize = ImGui::GetFrameHeight() * 0.85f;
    float rightEdge = ImGui::GetWindowContentRegionMax().x;

    ImGui::SameLine(rightEdge - btnSize);

    ImGui::PushID(label);

    ImGui::PushStyleColor(
        ImGuiCol_Button,
        ImVec4(0.50f, 0.08f, 0.08f, 0.75f)
    );

    ImGui::PushStyleColor(
        ImGuiCol_ButtonHovered,
        ImVec4(0.78f, 0.18f, 0.18f, 1.00f)
    );

    ImGui::PushStyleColor(
        ImGuiCol_ButtonActive,
        ImVec4(0.95f, 0.08f, 0.08f, 1.00f)
    );

    if (ImGui::Button("×", ImVec2(btnSize, btnSize)))
        deleteRequested = true;

    ImGui::PopStyleColor(3);
    ImGui::PopID();

    return open;
}

[[maybe_unused]] static void EndComponentHeader()
{
    ImGui::TreePop();
}

[[maybe_unused]] static void DragVec3(
    const char* label,
    Vector3& v,
    float speed = 0.05f)
{
    float buf[3] = { v.x, v.y, v.z };

    ImGui::PushID(label);

    float labelW =
        ImGui::CalcTextSize(label).x
        + ImGui::GetStyle().ItemSpacing.x * 2.f;

    ImGui::SetNextItemWidth(-labelW);

    if (ImGui::DragFloat3("##v", buf, speed))
        v = { buf[0], buf[1], buf[2] };

    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    ImGui::PopID();
}

[[maybe_unused]] static void DragQuat(
    const char* label,
    Quat& q)
{
    float buf[4] = { q.x, q.y, q.z, q.w };

    ImGui::PushID(label);

    float labelW =
        ImGui::CalcTextSize(label).x
        + ImGui::GetStyle().ItemSpacing.x * 2.f;

    ImGui::SetNextItemWidth(-labelW);

    if (ImGui::DragFloat4("##q", buf, 0.01f))
        q = { buf[0], buf[1], buf[2], buf[3] };

    ImGui::SameLine();
    ImGui::TextUnformatted(label);

    ImGui::PopID();
}

[[maybe_unused]] static bool StrContainsCI(
    const std::string& haystack,
    const char* needle)
{
    if (!needle || needle[0] == '\0')
        return true;

    std::string h = haystack;
    std::string n = needle;

    std::transform(
        h.begin(),
        h.end(),
        h.begin(),
        ::tolower
    );

    std::transform(
        n.begin(),
        n.end(),
        n.begin(),
        ::tolower
    );

    return h.find(n) != std::string::npos;
}

[[maybe_unused]] static void DrawTextureSlot(
    const char* slotLabel,
    Texture* tex,
    float thumbSize = 56.f)
{
    ImGui::PushID(slotLabel);

    if (tex)
    {
        ImTextureID imID =
            (ImTextureID)(intptr_t)tex->getID();

        ImGui::Image(
            imID,
            ImVec2(thumbSize, thumbSize),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();

            ImGui::Image(
                imID,
                ImVec2(256.f, 256.f),
                ImVec2(0, 1),
                ImVec2(1, 0)
            );

            ImGui::Text(
                "%d x %d | id %d",
                tex->getWidth(),
                tex->getHeight(),
                tex->getID()
            );

            ImGui::EndTooltip();
        }
    }
    else
    {
        ImVec2 p = ImGui::GetCursorScreenPos();

        ImGui::GetWindowDrawList()->AddRectFilled(
            p,
            ImVec2(p.x + thumbSize, p.y + thumbSize),
            IM_COL32(60, 60, 60, 255)
        );

        ImGui::GetWindowDrawList()->AddRect(
            p,
            ImVec2(p.x + thumbSize, p.y + thumbSize),
            IM_COL32(120, 120, 120, 255)
        );

        ImGui::Dummy(ImVec2(thumbSize, thumbSize));
    }

    ImGui::SameLine();

    ImGui::SetCursorPosY(
        ImGui::GetCursorPosY()
        + (thumbSize - ImGui::GetTextLineHeight()) * 0.5f
    );

    ImGui::TextUnformatted(slotLabel);

    ImGui::PopID();
}

[[maybe_unused]] static void DrawTextureSlot(
    const char* slotLabel,
    unsigned int tex,
    float thumbSize = 56.f)
{
    ImGui::PushID(slotLabel);

    if (tex)
    {
        ImTextureID imID =
            (ImTextureID)(uintptr_t)tex;

        ImGui::Image(
            imID,
            ImVec2(thumbSize, thumbSize),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();

            ImGui::Image(
                imID,
                ImVec2(256.f, 256.f),
                ImVec2(0, 1),
                ImVec2(1, 0)
            );

            ImGui::Text("id %d", tex);

            ImGui::EndTooltip();
        }
    }
    else
    {
        ImVec2 p = ImGui::GetCursorScreenPos();

        ImGui::GetWindowDrawList()->AddRectFilled(
            p,
            ImVec2(p.x + thumbSize, p.y + thumbSize),
            IM_COL32(60, 60, 60, 255)
        );

        ImGui::GetWindowDrawList()->AddRect(
            p,
            ImVec2(p.x + thumbSize, p.y + thumbSize),
            IM_COL32(120, 120, 120, 255)
        );

        ImGui::Dummy(ImVec2(thumbSize, thumbSize));
    }

    ImGui::SameLine();

    ImGui::SetCursorPosY(
        ImGui::GetCursorPosY()
        + (thumbSize - ImGui::GetTextLineHeight()) * 0.5f
    );

    ImGui::TextUnformatted(slotLabel);

    ImGui::PopID();
}

class UiInput
{
public:
    UiInput(
        IPlatform& platform,
        EventBus* bus,
        AssetManager& manager
    );

    ~UiInput();

    UiInput(const UiInput&) = delete;
    UiInput& operator=(const UiInput&) = delete;

    bool Initialize();

    void Shutdown();

    void startNewFrame();

    void buildUI(
        entt::registry& registry,
        Vector2 windowsize,
        const DebugRenderData& debugData,
        RenderGraph* rendergraph
    );

    void render();

    template<typename Tuple, std::size_t Index = 0>
    void renderComponents(
        entt::registry& registry,
        entt::entity entity,
        const std::unordered_map<
            std::type_index,
            std::function<
                void(entt::registry&, entt::entity, bool&)
            >
        >& renderMap)
    {
        if constexpr (Index < std::tuple_size_v<Tuple>)
        {
            using Comp = std::tuple_element_t<Index, Tuple>;

            if (registry.all_of<Comp>(entity))
            {
                auto it = renderMap.find(typeid(Comp));

                if (it != renderMap.end())
                {
                    bool shouldDelete = false;

                    it->second(
                        registry,
                        entity,
                        shouldDelete
                    );

                    if (shouldDelete)
                        registry.remove<Comp>(entity);
                }
            }

            renderComponents<Tuple, Index + 1>(
                registry,
                entity,
                renderMap
            );
        }
    }

private:
    void ApplyEditorStyle();

    void createWindow(
        const std::string& title,
        std::function<void(bool&)> drawFunc
    );

    void DrawModelManagerWindow(
        ModelManager* mgr,
        entt::registry& registry,
        Vector2 windowSizes
    );

    void DrawRenderPassWindow(
        RenderPass* pass,
        const std::function<void(RenderPass*)>& drawFn,
        Vector2 windowSizes
    );

private:

     void DrawRenderGraphWindow(
        RenderGraph* rendergraph,
        Vector2 windowSizes
    );

    void DrawFramebufferStatesWindow(
        RenderGraph* rendergraph,
        Vector2 windowSizes
    );

    bool renderGraphOpen = false;

    
    [[maybe_unused]] AssetManager& assetManager;
    [[maybe_unused]] EventBus* bus;

    IPlatform& platform;

    std::vector<DebugWindow> windows;

    entt::entity activeInspectorEntity = entt::null;

    char newEntityNameBuf[128] = {};

    bool modelManagerOpen = false;

    int activePassIndex = -1;

    std::unordered_map<
        std::type_index,
        std::function<void(RenderPass*)>
    > passRenderMap;

    std::unordered_map<
        std::string,
        std::function<void(entt::registry&, entt::entity)>
    > componentAdderMap;

    std::unordered_map<
        std::type_index,
        std::function<void(entt::registry&, entt::entity, bool&)>
    > componentRenderMap;



    bool initialized = false;
};