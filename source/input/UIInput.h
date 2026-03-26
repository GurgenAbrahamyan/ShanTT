#pragma once
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>

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

// ─────────────────────────────────────────────────────────────────────────────

struct DebugWindow {
    std::string title;
    std::function<void(bool& open)> drawFunc;
    bool open = true;
};


static bool BeginComponentHeader(const char* label, bool defaultOpen = true)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen * defaultOpen
        | ImGuiTreeNodeFlags_Framed
        | ImGuiTreeNodeFlags_SpanAvailWidth
        | ImGuiTreeNodeFlags_AllowOverlap;
        ;
    return ImGui::TreeNodeEx(label, flags);
}
static void EndComponentHeader() { ImGui::TreePop(); }


static void DragVec3(const char* label, Vector3& v, float speed = 0.05f)
{
    float buf[3] = { v.x, v.y, v.z };
    ImGui::PushID(label);
    if (ImGui::BeginTable("##t", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("##l", ImGuiTableColumnFlags_WidthFixed, 70.f);
        ImGui::TableSetupColumn("##v", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("%s", label);  // <--
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1);
        if (ImGui::DragFloat3("##v", buf, speed))
            v = { buf[0], buf[1], buf[2] };
        ImGui::EndTable();
    }
    ImGui::PopID();
}

static void DragQuat(const char* label, Quat& q)
{
    float buf[4] = { q.x, q.y, q.z, q.w };
    ImGui::PushID(label);
    if (ImGui::BeginTable("##t", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("##l", ImGuiTableColumnFlags_WidthFixed, 70.f);
        ImGui::TableSetupColumn("##v", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::Text("%s", label);  // <--
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1);
        if (ImGui::DragFloat4("##q", buf, 0.01f))
            q = { buf[0], buf[1], buf[2], buf[3] };
        ImGui::EndTable();
    }
    ImGui::PopID();
}

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
        float windowWidthUi, windowHeightUi;
        float offsetFromBorders = 10;
        windowWidthUi = (float)ctx->windowWidth / 6 ;
        windowHeightUi = (float)ctx->windowHeight;

        ImGui::SetNextWindowSize(ImVec2(windowWidthUi-2*offsetFromBorders, windowHeightUi- 2*offsetFromBorders), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(offsetFromBorders, offsetFromBorders), ImGuiCond_FirstUseEver);
        ImGui::Begin("Entities");

      
        static char filter[128] = {};
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##filter", "Filter...", filter, sizeof(filter));
        ImGui::Separator();

        auto view = registry->view<entt::entity>();
        for (auto entity : view)
        {
            if (!registry->valid(entity)) continue;

            std::string displayName = "Entity " + std::to_string((uint32_t)entity);
            if (registry->all_of<TagComponent>(entity))
                displayName = registry->get<TagComponent>(entity).tag;

            
            if (filter[0] != '\0' &&
                displayName.find(filter) == std::string::npos)
                continue;

            bool selected = (selectedEntity == entity);
            if (ImGui::Selectable(displayName.c_str(), selected))
                selectedEntity = entity;
        }

        ImGui::End();

        if (registry->valid(selectedEntity))
        {
          
            windowWidthUi = (float)ctx->windowWidth / 7 ;
            windowHeightUi = (float)ctx->windowHeight / 3;
            ImGui::SetNextWindowSize(ImVec2(windowWidthUi-2* offsetFromBorders, windowHeightUi-2* offsetFromBorders), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(ctx->windowWidth - windowWidthUi- offsetFromBorders, offsetFromBorders), ImGuiCond_FirstUseEver);

            std::string inspectorTitle = "Inspector";
            if (registry->all_of<TagComponent>(selectedEntity))
                inspectorTitle = registry->get<TagComponent>(selectedEntity).tag;

            ImGui::Begin(inspectorTitle.c_str());
            ImGui::Text("ID: %u", (uint32_t)selectedEntity);
            ImGui::Separator();

            renderComponents<ComponentTypes>(*registry, selectedEntity, componentRenderMap);

            ImGui::End();
        }

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
        std::function<void(entt::registry&, entt::entity)>>&renderMap)
    {
        if constexpr (Index < std::tuple_size_v<Tuple>)
        {
            using Comp = std::tuple_element_t<Index, Tuple>;
            if (registry.all_of<Comp>(entity))
            {
                auto it = renderMap.find(typeid(Comp));
                if (it != renderMap.end())
                    it->second(registry, entity);
            }
            renderComponents<Tuple, Index + 1>(registry, entity, renderMap);
        }
    }

private:

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
    entt::entity selectedEntity = entt::null;

  std::unordered_map<std::type_index,
        std::function<void(entt::registry&, entt::entity)>> componentRenderMap =
    {
        { typeid(TagComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Tag")) return;
            auto& t = r.get<TagComponent>(e);
            char buf[256]; strncpy_s(buf, t.tag.c_str(), sizeof(buf));
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputText("##tag", buf, sizeof(buf)))
                t.tag = buf;
            EndComponentHeader();
        }},

         { typeid(ParentComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Parent")) return;
            auto& p = r.get<ParentComponent>(e);
            ImGui::Text("Parent ID: %s",
                p.parent == entt::null ? "none"
                : std::to_string((uint32_t)p.parent).c_str());
            EndComponentHeader();
        }},

        { typeid(TransformComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Transform")) return;
            auto& t = r.get<TransformComponent>(e);
            DragVec3("Position", t.position);
            DragQuat("Rotation", t.rotation);
            DragVec3("Scale",    t.scale, 0.01f);
            EndComponentHeader();
        }},

        { typeid(WorldMatrixComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("World Matrix", false)) return;
            auto& w = r.get<WorldMatrixComponent>(e);
        
        
            EndComponentHeader();
        }},

          { typeid(ActiveCameraTag), [](entt::registry&, entt::entity) {
            if (!BeginComponentHeader("Active Camera")) return;
            ImGui::TextDisabled("(marker — no fields)");
            EndComponentHeader();
        }},

         { typeid(CameraComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Camera")) return;
            auto& c = r.get<CameraComponent>(e);
            ImGui::DragFloat("FOV",          &c.fov,         0.5f,  1.f, 180.f);
            ImGui::DragFloat("Aspect",       &c.aspectRatio, 0.01f, 0.1f, 4.f);
            ImGui::DragFloat("Near",         &c.nearPlane,   0.001f, 0.001f, 10.f);
            ImGui::DragFloat("Far",          &c.farPlane,    1.f,   1.f, 10000.f);
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

        { typeid(CubeMapComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("CubeMap")) return;
            auto& cm = r.get<CubeMapComponent>(e);
            ImGui::Text("Ptr: %s", cm.cubeMap ? "set" : "null");
            EndComponentHeader();
        }},

         { typeid(LightComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Light")) return;
            auto& l = r.get<LightComponent>(e);

            const char* types[] = { "Point", "Directional", "Spot" };
            int typeIdx = (int)l.type;
            if (ImGui::Combo("Type", &typeIdx, types, 3))
                l.type = (LightType)typeIdx;

            float col[3] = { l.color.x, l.color.y, l.color.z };
            if (ImGui::ColorEdit3("Color", col))
                l.color = { col[0], col[1], col[2] };

            DragVec3("Direction", l.direction);
            ImGui::DragFloat("Intensity",  &l.intensity,       0.01f, 0.f, 100.f);

            if (l.type == LightType::Spot) {
                ImGui::DragFloat("Inner Cone", &l.innerConeAngle, 0.005f, 0.f, 1.f);
                ImGui::DragFloat("Outer Cone", &l.outerConeAngle, 0.005f, 0.f, 1.f);
            }

            ImGui::Checkbox("Casts Shadow", &l.castsShadow);
            EndComponentHeader();
        }},

         { typeid(MaterialComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Material")) return;
            auto& m = r.get<MaterialComponent>(e);
            ImGui::Text("Ptr: %s", m.material ? "set" : "null");
            if (m.material)
                ImGui::Text("Addr: 0x%llX", (unsigned long long)m.material);
            EndComponentHeader();
        }},

        { typeid(MeshComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Mesh")) return;
            auto& m = r.get<MeshComponent>(e);
            ImGui::Text("Ptr: %s", m.mesh ? "set" : "null");
            if (m.mesh)
                ImGui::Text("Addr: 0x%llX", (unsigned long long)m.mesh);
            EndComponentHeader();
        }},

        { typeid(ModelComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Model")) return;
            auto& model = r.get<ModelComponent>(e);
            ImGui::Text("Mesh entries: %zu", model.meshes.size());
            for (size_t i = 0; i < model.meshes.size(); ++i) {
                auto& entry = model.meshes[i];
                ImGui::PushID((int)i);
                std::string label = "Entry " + std::to_string(i);
                if (ImGui::TreeNode(label.c_str())) {
                    ImGui::Text("Mesh:     %s", entry.mesh ? "set" : "null");
                    ImGui::Text("Material: %s", entry.material ? "set" : "null");
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            EndComponentHeader();
        }},

        { typeid(CollisionShapeComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Collision Shape")) return;
            auto& cs = r.get<CollisionShapeComponent>(e);
            ImGui::Text("Vertices: %zu", cs.vertices.size());
            ImGui::Text("Indices:  %zu", cs.indices.size());
            DragVec3("Local Pos",   cs.localPosition);
            DragQuat("Local Rot",   cs.localRotation);
            DragVec3("Local Scale", cs.localScale, 0.01f);
            EndComponentHeader();
        }},

        { typeid(RigidBodyComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Rigid Body")) return;
            auto& rb = r.get<RigidBodyComponent>(e);
            ImGui::DragFloat("Mass",    &rb.mass,    0.1f, 0.f, 10000.f);
            ImGui::DragFloat("Inv Mass",&rb.invmass, 0.001f);
            ImGui::Separator();
            DragVec3("Lin Velocity", rb.linearVelocity);
            DragVec3("Ang Velocity", rb.angularVelocity);
            ImGui::Separator();
            DragVec3("Force Accum",  rb.forceAccum);
            DragVec3("Torque Accum", rb.torqueAccum);
            EndComponentHeader();
        }},

        { typeid(SoftBodyComponent), [](entt::registry& r, entt::entity e) {
            if (!BeginComponentHeader("Soft Body")) return;
            auto& sb = r.get<SoftBodyComponent>(e);
            ImGui::Text("Particles: %s", sb.particles ? "set" : "null");
            ImGui::DragFloat("Stiffness", &sb.stiffness, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("Damping",   &sb.damping,   0.001f, 0.f, 1.f);
            EndComponentHeader();
        }},
    };
};