#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <algorithm>
#include <string>
#include <cctype>

#include "../core/EventBus.h"
#include "../core/Event.h"
#include "../render/data/RenderContext.h"

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
#include "../ecs/components/graphics/MeshComponent.h"
#include "../ecs/components/graphics/ModelComponent.h"
#include "../ecs/components/physics/CollisionShapeComponent.h"
#include "../ecs/components/physics/RigidBodyComponent.h"
#include "../ecs/components/physics/SoftBodyComponent.h"


#include "../resources/managers/ModelManager.h"

using ComponentTypes = std::tuple<
    TagComponent,
    ParentComponent,
    TransformComponent,
    WorldMatrixComponent,
    ActiveCameraTag,
    CameraComponent,
    CubeMapComponent,
    LightComponent,
    MaterialComponent,
    MeshComponent,
    ModelComponent,
    CollisionShapeComponent,
    RigidBodyComponent,
    SoftBodyComponent
>;

struct DebugWindow {
    std::string title;
    std::function<void(bool& open)> drawFunc;
    bool open = true;
};

// ─── Helpers ──────────────────────────────────────────────────────────────────

static bool BeginComponentHeader(const char* label, bool& deleteRequested, bool defaultOpen = true)
{
    ImGuiTreeNodeFlags flags = (defaultOpen ? ImGuiTreeNodeFlags_DefaultOpen : 0)
        | ImGuiTreeNodeFlags_Framed
        | ImGuiTreeNodeFlags_SpanAvailWidth
        | ImGuiTreeNodeFlags_AllowOverlap;

    bool open = ImGui::TreeNodeEx(label, flags);

    float btnSize = ImGui::GetFrameHeight() * 0.85f;
    float rightEdge = ImGui::GetWindowContentRegionMax().x;
    ImGui::SameLine(rightEdge - btnSize);

    ImGui::PushID(label);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.08f, 0.08f, 0.75f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.78f, 0.18f, 0.18f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.95f, 0.08f, 0.08f, 1.00f));
    if (ImGui::Button("×", ImVec2(btnSize, btnSize)))
        deleteRequested = true;
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    return open;
}

static void EndComponentHeader() { ImGui::TreePop(); }

static void DragVec3(const char* label, Vector3& v, float speed = 0.05f)
{
    float buf[3] = { v.x, v.y, v.z };
    ImGui::PushID(label);
    float labelW = ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2.f;
    ImGui::SetNextItemWidth(-labelW);
    if (ImGui::DragFloat3("##v", buf, speed))
        v = { buf[0], buf[1], buf[2] };
    ImGui::SameLine();
    ImGui::TextUnformatted(label);
    ImGui::PopID();
}

static void DragQuat(const char* label, Quat& q)
{
    float buf[4] = { q.x, q.y, q.z, q.w };
    ImGui::PushID(label);
    float labelW = ImGui::CalcTextSize(label).x + ImGui::GetStyle().ItemSpacing.x * 2.f;
    ImGui::SetNextItemWidth(-labelW);
    if (ImGui::DragFloat4("##q", buf, 0.01f))
        q = { buf[0], buf[1], buf[2], buf[3] };
    ImGui::SameLine();
    ImGui::TextUnformatted(label);
    ImGui::PopID();
}

static bool StrContainsCI(const std::string& haystack, const char* needle)
{
    if (!needle || needle[0] == '\0') return true;
    std::string h = haystack, n = needle;
    std::transform(h.begin(), h.end(), h.begin(), ::tolower);
    std::transform(n.begin(), n.end(), n.begin(), ::tolower);
    return h.find(n) != std::string::npos;
}

static void DrawTextureSlot(const char* slotLabel, Texture* tex, float thumbSize = 56.f)
{
    ImGui::PushID(slotLabel);
    if (tex)
    {
        ImTextureID imID = (ImTextureID)(intptr_t)tex->getID();
        ImGui::Image(imID, ImVec2(thumbSize, thumbSize), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image(imID, ImVec2(256.f, 256.f), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Text("%d x %d  |  id %d", tex->getWidth(), tex->getHeight(), tex->getID());
            ImGui::EndTooltip();
        }
    }
    else
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + thumbSize, p.y + thumbSize), IM_COL32(60, 60, 60, 255));
        ImGui::GetWindowDrawList()->AddRect(p, ImVec2(p.x + thumbSize, p.y + thumbSize), IM_COL32(120, 120, 120, 255));
        ImGui::Dummy(ImVec2(thumbSize, thumbSize));
    }
    ImGui::SameLine();
    float centreOffsetY = (thumbSize - ImGui::GetTextLineHeight()) * 0.5f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + centreOffsetY);
    ImGui::TextUnformatted(slotLabel);
    ImGui::PopID();
}

static void DrawTextureSlot(const char* slotLabel, unsigned int tex, float thumbSize = 56.f)
{
    ImGui::PushID(slotLabel);
    if (tex)
    {
        ImTextureID imID = (ImTextureID)(uintptr_t)tex;
        ImGui::Image(imID, ImVec2(thumbSize, thumbSize), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image(imID, ImVec2(256.f, 256.f), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Text("id %d", tex);
            ImGui::EndTooltip();
        }
    }
    else
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + thumbSize, p.y + thumbSize), IM_COL32(60, 60, 60, 255));
        ImGui::GetWindowDrawList()->AddRect(p, ImVec2(p.x + thumbSize, p.y + thumbSize), IM_COL32(120, 120, 120, 255));
        ImGui::Dummy(ImVec2(thumbSize, thumbSize));
    }
    ImGui::SameLine();
    float centreOffsetY = (thumbSize - ImGui::GetTextLineHeight()) * 0.5f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + centreOffsetY);
    ImGui::TextUnformatted(slotLabel);
    ImGui::PopID();
}

// ─────────────────────────────────────────────────────────────────────────────

class UiInput {
public:
    UiInput(GLFWwindow* window, EventBus* bus) : window(window), bus(bus)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ApplyEditorStyle();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void startNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void buildUI(RenderContext* ctx)
    {
        auto* registry = ctx->registry;
        auto* modelMgr = ctx->modelManager; // ModelManager* added to RenderContext

        const float pad = 10.f;
        const float panelW = (float)ctx->windowWidth / 5.f;
        const float inspW = (float)ctx->windowWidth / 5.f;
        const float gbufW = (float)ctx->windowWidth / 5.f;
        const float usableW = panelW - 2.f * pad;
        const float usableH = (float)ctx->windowHeight - 2.f * pad;

        // ── LEFT PANEL — 3 columns: [Comp Adder | Entity List | Systems] ─────────
        ImGui::SetNextWindowSize(ImVec2(usableW, usableH), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(pad, pad), ImGuiCond_FirstUseEver);
        ImGui::Begin("Scene", nullptr, 0);

        ImGui::Columns(3, "scene_cols", true);

        // ── Column 0: Component adder ─────────────────────────────────────────────
        ImGui::BeginChild("##compadder", ImVec2(0.f, 0.f), true);
        if (registry->valid(activeInspectorEntity))
        {
            std::string activeName = "Entity " + std::to_string((uint32_t)activeInspectorEntity);
            if (registry->all_of<TagComponent>(activeInspectorEntity))
                activeName = registry->get<TagComponent>(activeInspectorEntity).tag;
            ImGui::TextColored(ImVec4(0.40f, 0.80f, 1.f, 1.f), "%s %s",
                reinterpret_cast<const char*>(u8"\u2605"), activeName.c_str());
        }
        else ImGui::TextDisabled("No entity selected");

        ImGui::Separator();
        static char compFilter[128] = {};
        ImGui::SetNextItemWidth(-1.f);
        ImGui::InputTextWithHint("##cfilter", "Search components...", compFilter, sizeof(compFilter));
        ImGui::Spacing();

        if (registry->valid(activeInspectorEntity))
            for (auto& [name, adderFn] : componentAdderMap)
            {
                if (!StrContainsCI(name, compFilter)) continue;
                ImGui::PushID(name.c_str());
                if (ImGui::Selectable(("  + " + name).c_str())) adderFn(*registry, activeInspectorEntity);
                ImGui::PopID();
            }
        ImGui::EndChild();

        ImGui::NextColumn();

        // ── Column 1: Entity list ─────────────────────────────────────────────────
        ImGui::BeginChild("##entitylist", ImVec2(0.f, 0.f), true);
        if (ImGui::Button(" + ")) ImGui::OpenPopup("NewEntityPopup");
        ImGui::SameLine();
        static char entityFilter[128] = {};
        ImGui::SetNextItemWidth(-1.f);
        ImGui::InputTextWithHint("##efilter", "Filter entities...", entityFilter, sizeof(entityFilter));
        ImGui::Separator();

        entt::entity toDestroy = entt::null;
        auto view = registry->view<entt::entity>();
        for (auto entity : view)
        {
            if (!registry->valid(entity)) continue;
            std::string displayName = "Entity " + std::to_string((uint32_t)entity);
            if (registry->all_of<TagComponent>(entity))
                displayName = registry->get<TagComponent>(entity).tag;
            if (!StrContainsCI(displayName, entityFilter)) continue;

            bool isActive = (entity == activeInspectorEntity);

            if (isActive) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.80f, 1.00f, 1.f));

            if (ImGui::Selectable(displayName.c_str(), isActive))
                activeInspectorEntity = entity; // ← single window: just set active

            if (isActive) ImGui::PopStyleColor();

            if (ImGui::BeginPopupContextItem())
            {
                ImGui::TextDisabled("%s", displayName.c_str());
                ImGui::Separator();
                if (ImGui::MenuItem("Delete Entity")) toDestroy = entity;
                ImGui::EndPopup();
            }
        }

        if (registry->valid(toDestroy))
        {
            if (activeInspectorEntity == toDestroy) activeInspectorEntity = entt::null;
            registry->destroy(toDestroy);
        }

        if (ImGui::BeginPopup("NewEntityPopup"))
        {
            ImGui::Text("New Entity Name");
            ImGui::Separator();
            ImGui::SetNextItemWidth(220.f);
            if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
            bool confirm = ImGui::InputText("##newname", newEntityNameBuf, sizeof(newEntityNameBuf),
                ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            confirm |= ImGui::Button("Create");
            if (confirm && newEntityNameBuf[0] != '\0')
            {
                auto e = registry->create();
                registry->emplace<TagComponent>(e, std::string(newEntityNameBuf));
                activeInspectorEntity = e;
                newEntityNameBuf[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) { newEntityNameBuf[0] = '\0'; ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::EndChild();

        ImGui::NextColumn();

        // ── Column 2: Systems / Managers ─────────────────────────────────────────
        ImGui::BeginChild("##systems", ImVec2(0.f, 0.f), true);
        ImGui::TextColored(ImVec4(0.75f, 0.55f, 1.00f, 1.f), "Systems");
        ImGui::Separator();
        ImGui::Spacing();

        // Each entry: name → bool open flag
        // Expand this list as you add more managers/systems
        struct SystemEntry { const char* label; bool* openFlag; };
        SystemEntry systemEntries[] = {
            { "Model Manager",   &modelManagerOpen   },
            // { "Texture Manager", &textureManagerOpen },
            // { "Physics System",  &physicsSystemOpen  },
        };

        for (auto& entry : systemEntries)
        {
            bool isOpen = *entry.openFlag;
            if (isOpen) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.80f, 1.00f, 1.f));
            if (ImGui::Selectable(entry.label, isOpen))
                *entry.openFlag = !(*entry.openFlag);
            if (isOpen) ImGui::PopStyleColor();
        }

        ImGui::EndChild();

        ImGui::Columns(1);
        ImGui::End();

        // ── SINGLE ENTITY INSPECTOR WINDOW ────────────────────────────────────────
        if (registry->valid(activeInspectorEntity))
        {
            std::string entityName = "Entity " + std::to_string((uint32_t)activeInspectorEntity);
            if (registry->all_of<TagComponent>(activeInspectorEntity))
                entityName = registry->get<TagComponent>(activeInspectorEntity).tag;

            std::string title = std::string(reinterpret_cast<const char*>(u8"\u2605 "))
                + entityName + "##insp";

            ImGui::SetNextWindowSize(ImVec2(inspW, usableH/2), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2((float)ctx->windowWidth - inspW - pad, pad), ImGuiCond_FirstUseEver);

            bool windowOpen = true;
            ImGui::Begin(title.c_str(), &windowOpen,0);

            ImGui::Text("ID: %u", (uint32_t)activeInspectorEntity);
            ImGui::Separator();
            renderComponents<ComponentTypes>(*registry, activeInspectorEntity, componentRenderMap);

            ImGui::End();

            if (!windowOpen)
                activeInspectorEntity = entt::null;
        }

        if (modelManagerOpen && modelMgr) {
         DrawModelManagerWindow(modelMgr, registry, ctx);
    }
      //  // ── GBUFFER / RENDER PASS VIEWER ─────────────────────────────────────────
        const float gbufX = (float)ctx->windowWidth -  gbufW;
        float gbufH = (float)ctx->windowHeight / 2.f;
        ImGui::SetNextWindowSize(ImVec2(gbufW - pad, gbufH), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2((float)ctx->windowWidth - gbufW - pad, (float)ctx->windowHeight - gbufH - pad), ImGuiCond_FirstUseEver);
        ImGui::Begin("Render Passes", nullptr,
             ImGuiWindowFlags_HorizontalScrollbar);

        const float thumbW = ImGui::GetContentRegionAvail().x;
        const float thumbH = thumbW * (9.f / 16.f);

        for (auto& dt : ctx->debugTextures)
        {
            if (dt.textureID == 0) continue;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.85f, 1.00f, 1.f));
            ImGui::TextUnformatted(dt.name.c_str());
            ImGui::PopStyleColor();
            ImGui::SameLine(thumbW - 60.f);
            ImGui::TextDisabled("id %u", dt.textureID);
            ImTextureID imID = (ImTextureID)(intptr_t)dt.textureID;
            ImGui::Image(imID, ImVec2(thumbW, thumbH), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Image(imID, ImVec2(512.f, 512.f * (9.f / 16.f)), ImVec2(0, 1), ImVec2(1, 0));
                ImGui::TextDisabled("%s  |  id %u", dt.name.c_str(), dt.textureID);
                ImGui::EndTooltip();
            }
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        }
        ImGui::End();

        // ── Misc debug windows ────────────────────────────────────────────────────
        for (auto it = windows.begin(); it != windows.end();)
        {
            ImGui::Begin(it->title.c_str(), &it->open);
            it->drawFunc(it->open);
            ImGui::End();
            if (!it->open) it = windows.erase(it);
            else           ++it;
        }
    }

    void render()
    {
        ImGui::Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    template<typename Tuple, std::size_t Index = 0>
    void renderComponents(entt::registry& registry, entt::entity entity,
        const std::unordered_map<std::type_index,
        std::function<void(entt::registry&, entt::entity, bool&)>>&renderMap)
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
                    it->second(registry, entity, shouldDelete);
                    if (shouldDelete) registry.remove<Comp>(entity);
                }
            }
            renderComponents<Tuple, Index + 1>(registry, entity, renderMap);
        }
    }

private:

    // ── Model Manager Window ──────────────────────────────────────────────────
    void DrawModelManagerWindow(ModelManager* mgr, entt::registry* registry, RenderContext* ctx)
    {
        const float pad = 10.f;
        const float w = (float)ctx->windowWidth / 5.f;
        const float h = (float)ctx->windowHeight - 2.f * pad;

        float gbufH = (float)ctx->windowHeight / 2.f;
        ImGui::SetNextWindowPos(
            ImVec2((float)ctx->windowWidth - 2 * w - 2 * pad, (float)ctx->windowHeight - gbufH - pad),
            ImGuiCond_FirstUseEver
        );
        ImGui::SetNextWindowSize(ImVec2(w, gbufH), ImGuiCond_FirstUseEver);

        ImGui::Begin("Model Manager", nullptr, 0);

        // ─────────────────────────────────────────────
        // LOAD SECTION
        // ─────────────────────────────────────────────
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.f, 1.f), "Load Model");
        ImGui::Separator();

        static char nameBuf[128] = {};
        static char pathBuf[256] = {};

        ImGui::InputTextWithHint("##name", "Model name", nameBuf, sizeof(nameBuf));
        ImGui::InputTextWithHint("##path", "Path to model file", pathBuf, sizeof(pathBuf));

        bool canLoad = nameBuf[0] && pathBuf[0];

        if (!canLoad)
            ImGui::BeginDisabled();

        if (ImGui::Button("Load Model", ImVec2(-1, 0)))
        {
            mgr->loadModel(nameBuf, pathBuf);
            nameBuf[0] = '\0';
            pathBuf[0] = '\0';
        }

        if (!canLoad)
            ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Separator();

        // ─────────────────────────────────────────────
        // LOADED MODELS LIST
        // ─────────────────────────────────────────────
        ImGui::TextColored(ImVec4(0.75f, 0.55f, 1.f, 1.f), "Loaded Models");
        ImGui::Separator();

        const auto& models = mgr->getLoadedModels();

        static int selectedModel = -1;
        int index = 0;

        for (const auto& [name, asset] : models)
        {
            bool selected = (selectedModel == index);

            if (ImGui::Selectable(name.c_str(), selected))
                selectedModel = index;

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Path: %s", asset.path.c_str());
                ImGui::Text("Meshes: %zu", asset.meshes.size());
                ImGui::EndTooltip();
            }

            index++;
        }

        ImGui::Spacing();
        ImGui::Separator();

        
        ImGui::TextColored(ImVec4(0.6f, 1.f, 0.6f, 1.f), "Spawn");

        static char entityName[128] = "ModelEntity";

        ImGui::InputText("Entity Name", entityName, sizeof(entityName));

        if (ImGui::Button("Create Entity + Assign Model", ImVec2(-1, 0)))
        {
            if (selectedModel >= 0)
            {
                auto it = models.begin();
                std::advance(it, selectedModel);

                auto entity = registry->create();
                registry->emplace<TagComponent>(entity, std::string(entityName));

                mgr->instantiateModel(it->first, *registry, entity);

                activeInspectorEntity = entity; // optional (if you want auto focus)
            }
        }

        ImGui::End();
    }

    // ─────────────────────────────────────────────────────────────────────────

    void ApplyEditorStyle()
    {
        ImGuiStyle& s = ImGui::GetStyle();
        ImGui::StyleColorsDark();
        s.WindowRounding = 4.f;
        s.FrameRounding = 3.f;
        s.ChildRounding = 3.f;
        s.GrabRounding = 3.f;
        s.FramePadding = ImVec2(6, 3);
        s.ItemSpacing = ImVec2(6, 4);
        s.WindowBorderSize = 1.f;
        s.FrameBorderSize = 0.f;

        ImVec4* c = s.Colors;
        c[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.97f);
        c[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        c[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.28f, 1.00f);
        c[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);
        c[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.35f, 0.55f, 1.00f);
        c[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
        c[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.28f, 1.00f);
        c[ImGuiCol_Button] = ImVec4(0.22f, 0.30f, 0.50f, 1.00f);
        c[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.38f, 0.62f, 1.00f);
        c[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.55f, 0.90f, 1.00f);
        c[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.18f, 0.30f, 1.00f);
        c[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.35f, 1.00f);
    }

    void createWindow(const std::string& title, std::function<void(bool&)> drawFunc)
    {
        for (auto& w : windows)
            if (w.title == title) { w.open = true; return; }
        windows.push_back({ title, drawFunc });
    }

    EventBus* bus;
    GLFWwindow* window;
    std::vector<DebugWindow> windows;

    entt::entity activeInspectorEntity = entt::null;

    char newEntityNameBuf[128] = {};

   
    bool modelManagerOpen = false;
    // bool textureManagerOpen = false;
    // bool physicsSystemOpen  = false;

  
    char modelLoadName[128] = {};
    char modelLoadPath[256] = {};

    std::unordered_map<std::string, std::function<void(entt::registry&, entt::entity)>>
        componentAdderMap =
    {
        { "Tag",             [](entt::registry& r, entt::entity e) { r.emplace_or_replace<TagComponent>(e, "unnamed"); }},
        { "Parent",          [](entt::registry& r, entt::entity e) { r.emplace_or_replace<ParentComponent>(e); }},
        { "Transform",       [](entt::registry& r, entt::entity e) { r.emplace_or_replace<TransformComponent>(e); }},
        { "World Matrix",    [](entt::registry& r, entt::entity e) { r.emplace_or_replace<WorldMatrixComponent>(e); }},
        { "Active Camera",   [](entt::registry& r, entt::entity e) { r.emplace_or_replace<ActiveCameraTag>(e); }},
        { "Camera",          [](entt::registry& r, entt::entity e) { r.emplace_or_replace<CameraComponent>(e); }},
        { "CubeMap",         [](entt::registry& r, entt::entity e) { r.emplace_or_replace<CubeMapComponent>(e); }},
        { "Light",           [](entt::registry& r, entt::entity e) { r.emplace_or_replace<LightComponent>(e); }},
        { "Material",        [](entt::registry& r, entt::entity e) { r.emplace_or_replace<MaterialComponent>(e); }},
        { "Mesh",            [](entt::registry& r, entt::entity e) { r.emplace_or_replace<MeshComponent>(e); }},
        { "Model",           [](entt::registry& r, entt::entity e) { r.emplace_or_replace<ModelComponent>(e); }},
        { "Collision Shape", [](entt::registry& r, entt::entity e) { r.emplace_or_replace<CollisionShapeComponent>(e); }},
        { "Rigid Body",      [](entt::registry& r, entt::entity e) { r.emplace_or_replace<RigidBodyComponent>(e); }},
        { "Soft Body",       [](entt::registry& r, entt::entity e) { r.emplace_or_replace<SoftBodyComponent>(e); }},
    };

    // ── Component render map ──────────────────────────────────────────────────
    std::unordered_map<std::type_index,
        std::function<void(entt::registry&, entt::entity, bool&)>> componentRenderMap =
    {
        { typeid(TagComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Tag", del)) return;
            auto& t = r.get<TagComponent>(e);
            char buf[256]; strncpy_s(buf, t.tag.c_str(), sizeof(buf));
            ImGui::SetNextItemWidth(-1.f);
            if (ImGui::InputText("##tag", buf, sizeof(buf))) t.tag = buf;
            EndComponentHeader();
        }},

        { typeid(ParentComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Parent", del)) return;
            auto& p = r.get<ParentComponent>(e);
            ImGui::Text("Parent ID: %s",
                p.parent == entt::null ? "none" : std::to_string((uint32_t)p.parent).c_str());
            EndComponentHeader();
        }},

        { typeid(TransformComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Transform", del)) return;
            auto& t = r.get<TransformComponent>(e);
            DragVec3("Position", t.position);
            DragQuat("Rotation", t.rotation);
            DragVec3("Scale",    t.scale, 0.01f);
            EndComponentHeader();
        }},

        { typeid(WorldMatrixComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("World Matrix", del, false)) return;
            EndComponentHeader();
        }},

        { typeid(ActiveCameraTag), [](entt::registry&, entt::entity, bool& del) {
            if (!BeginComponentHeader("Active Camera", del)) return;
            ImGui::TextDisabled("(marker — no fields)");
            EndComponentHeader();
        }},

        { typeid(CameraComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Camera", del)) return;
            auto& c = r.get<CameraComponent>(e);
            ImGui::DragFloat("FOV",          &c.fov,          0.5f,   1.f,   180.f);
            ImGui::DragFloat("Aspect",       &c.aspectRatio,  0.01f,  0.1f,  4.f);
            ImGui::DragFloat("Near",         &c.nearPlane,    0.001f, 0.001f,10.f);
            ImGui::DragFloat("Far",          &c.farPlane,     1.f,    1.f,   10000.f);
            ImGui::Separator();
            ImGui::DragFloat("Pitch",        &c.pitch,  0.1f);
            ImGui::DragFloat("Yaw",          &c.yaw,    0.1f);
            ImGui::Separator();
            ImGui::Checkbox("Camera Blur",   &c.applyBlur);
            ImGui::DragFloat("Focus Dist",   &c.focusDistance, 0.05f);
            ImGui::DragFloat("Aperture",     &c.aperture,      0.01f);
            ImGui::DragFloat("Focal Length", &c.focalLength,   0.001f);
            ImGui::DragFloat("Blur Scale",   &c.blurScale,     1.f);
            EndComponentHeader();
        }},

        { typeid(CubeMapComponent), [](entt::registry& r, entt::entity e, bool& del) {
           if (!BeginComponentHeader("CubeMap", del)) return;
           auto& cm = r.get<CubeMapComponent>(e);
           ImGui::DragFloat("Environment Intensity", &cm.intensity, 0.01f, 0.0f, 1.0f);
           EndComponentHeader();
        }},

        { typeid(LightComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Light", del)) return;
            auto& l = r.get<LightComponent>(e);
            const char* types[] = { "Point", "Directional", "Spot" };
            int typeIdx = (int)l.type;
            if (ImGui::Combo("Type", &typeIdx, types, 3)) l.type = (LightType)typeIdx;
            float col[3] = { l.color.x, l.color.y, l.color.z };
            if (ImGui::ColorEdit3("Color", col)) l.color = { col[0], col[1], col[2] };
            DragVec3("Direction", l.direction);
            ImGui::DragFloat("Intensity", &l.intensity, 0.01f, 0.f, 100.f);
            if (l.type == LightType::Spot)
            {
                ImGui::DragFloat("Inner Cone", &l.innerConeAngle, 0.005f, 0.f, 1.f);
                ImGui::DragFloat("Outer Cone", &l.outerConeAngle, 0.005f, 0.f, 1.f);
            }
            ImGui::Checkbox("Casts Shadow", &l.castsShadow);
            EndComponentHeader();
        }},

        { typeid(MaterialComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Material", del)) return;
            auto& m = r.get<MaterialComponent>(e);
            ImGui::Text("Ptr: %s", m.material ? "set" : "null");
            if (m.material) ImGui::Text("Addr: 0x%llX", (unsigned long long)m.material);
            EndComponentHeader();
        }},

        { typeid(MeshComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Mesh", del)) return;
            auto& m = r.get<MeshComponent>(e);
            ImGui::Text("Ptr: %s", m.mesh ? "set" : "null");
            if (m.mesh) ImGui::Text("Addr: 0x%llX", (unsigned long long)m.mesh);
            EndComponentHeader();
        }},

        { typeid(ModelComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Model", del)) return;
            auto& model = r.get<ModelComponent>(e);
            ImGui::Text("Mesh entries: %zu", model.asset->meshes.size());
            ImGui::Spacing();

            static const char* slotLabels[] = { "Albedo", "ARM", "Normal", "Emissive" };
            static constexpr int kDisplaySlots = 4;

            for (size_t i = 0; i < model.asset->meshes.size(); ++i)
            {
                auto& entry = model.asset->meshes[i];
                ImGui::PushID((int)i);
                bool nodeOpen = ImGui::TreeNodeEx(("Entry " + std::to_string(i)).c_str(),
                    ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth);
                if (nodeOpen)
                {
                    ImGui::TextDisabled("Mesh:     %s", entry.mesh ? "set" : "null");
                    ImGui::TextDisabled("Material: %s", entry.material ? "set" : "null");
                    if (entry.material)
                    {
                        Material* mat = entry.material;
                        ImGui::Spacing();
                        ImGui::DragFloat("Metallic",  &mat->metallic,  0.01f, 0.f, 1.f);
                        ImGui::DragFloat("Roughness", &mat->roughness, 0.01f, 0.f, 1.f);
                        ImGui::DragFloat("AO",        &mat->ao,        0.01f, 0.f, 1.f);
                        float bc[4] = { mat->baseColorFactor.x, mat->baseColorFactor.y,
                                        mat->baseColorFactor.z, mat->baseColorFactor.w };
                        if (ImGui::ColorEdit4("Base Color", bc))
                            mat->baseColorFactor = { bc[0], bc[1], bc[2], bc[3] };
                        float ec[3] = { mat->emissiveFactor.x, mat->emissiveFactor.y, mat->emissiveFactor.z };
                        if (ImGui::ColorEdit3("Emissive", ec))
                            mat->emissiveFactor = { ec[0], ec[1], ec[2] };
                        ImGui::Spacing(); ImGui::Separator();
                        ImGui::TextUnformatted("Textures"); ImGui::Separator(); ImGui::Spacing();
                        for (int slot = 0; slot < kDisplaySlots; ++slot)
                        {
                            DrawTextureSlot(slotLabels[slot], mat->GetTexture(slot), 56.f);
                            ImGui::Spacing();
                        }
                    }
                    else ImGui::TextDisabled("No material attached.");
                    ImGui::TreePop();
                }
                ImGui::PopID();
                ImGui::Spacing();
            }
            EndComponentHeader();
        }},

        { typeid(CollisionShapeComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Collision Shape", del)) return;
            auto& cs = r.get<CollisionShapeComponent>(e);
            ImGui::Text("Vertices: %zu", cs.vertices.size());
            ImGui::Text("Indices:  %zu", cs.indices.size());
            DragVec3("Local Pos",   cs.localPosition);
            DragQuat("Local Rot",   cs.localRotation);
            DragVec3("Local Scale", cs.localScale, 0.01f);
            EndComponentHeader();
        }},

        { typeid(RigidBodyComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Rigid Body", del)) return;
            auto& rb = r.get<RigidBodyComponent>(e);
            ImGui::DragFloat("Mass",     &rb.mass,    0.1f,  0.f, 10000.f);
            ImGui::DragFloat("Inv Mass", &rb.invmass, 0.001f);
            ImGui::Separator();
            DragVec3("Lin Velocity", rb.linearVelocity);
            DragVec3("Ang Velocity", rb.angularVelocity);
            ImGui::Separator();
            DragVec3("Force Accum",  rb.forceAccum);
            DragVec3("Torque Accum", rb.torqueAccum);
            EndComponentHeader();
        }},

        { typeid(SoftBodyComponent), [](entt::registry& r, entt::entity e, bool& del) {
            if (!BeginComponentHeader("Soft Body", del)) return;
            auto& sb = r.get<SoftBodyComponent>(e);
            ImGui::Text("Particles: %s", sb.particles ? "set" : "null");
            ImGui::DragFloat("Stiffness", &sb.stiffness, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("Damping",   &sb.damping,   0.001f, 0.f, 1.f);
            EndComponentHeader();
        }},
    };
    
};