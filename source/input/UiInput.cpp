#include "UIInput.h"


#include "render/handlers/BloomPass.h"
#include "render/handlers/CompositePass.h"
#include "render/handlers/FXAAPass.h"
#include "render/handlers/ToneMappingPass.h"

bool UiInput::Initialize()
{
    if (initialized)
        return true;

    void* nativeHandle = platform.GetNativeWindowHandle();

    if (!nativeHandle)
    {
        std::cerr
            << "UiInput::Initialize(): "
            << "Platform native window handle is NULL\n";

        return false;
    }

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ApplyEditorStyle();

    auto* glfwWindow =
        static_cast<GLFWwindow*>(nativeHandle);

    if (!ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true))
    {
        std::cerr
            << "UiInput::Initialize(): "
            << "Failed to initialize ImGui GLFW backend\n";

        ImGui::DestroyContext();

        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330"))
    {
        std::cerr
            << "UiInput::Initialize(): "
            << "Failed to initialize ImGui OpenGL backend\n";

        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return false;
    }

    initialized = true;

    return true;
}

UiInput::UiInput(IPlatform& platform, EventBus* bus, AssetManager& assetManager) :  assetManager(assetManager),  bus(bus),  platform(platform)

{
   // ── Pass render map ───────────────────────────────────────────────────────

    passRenderMap = {

        { typeid(BloomPass), [](RenderPass* base) {

        auto* p =
            static_cast<BloomPass*>(base);

        auto& settings =
            p->getSettings();

        ImGui::DragFloat(
            "Filter Radius",
            &settings.filterRadius,
            0.0001f,
            0.f,
            0.05f
        );

        ImGui::DragInt(
            "Mip Levels",
            &settings.mipMapLength,
            1,
            1,
            12
        );
    }},


    { typeid(CompositePass), [](RenderPass* base) {

        auto* p =
            static_cast<CompositePass*>(base);

        auto& settings =
            p->getSettings();

        for (auto& uniform : settings.uniforms)
        {
            ImGui::PushID(uniform.name.c_str());

            std::visit(
                [&](auto& value)
                {
                    using T =
                        std::decay_t<decltype(value)>;

                    if constexpr (
                        std::is_same_v<T, float>
                    )
                    {
                        ImGui::DragFloat(
                            uniform.name.c_str(),
                            &value,
                            0.001f
                        );
                    }
                    else if constexpr (
                        std::is_same_v<T, int>
                    )
                    {
                        ImGui::DragInt(
                            uniform.name.c_str(),
                            &value
                        );
                    }
                    else if constexpr (
                        std::is_same_v<T, bool>
                    )
                    {
                        ImGui::Checkbox(
                            uniform.name.c_str(),
                            &value
                        );
                    }
                },
                uniform.value
            );

            ImGui::PopID();
        }
    }},


    { typeid(FXAAPass), [](RenderPass* base) {

        auto* p =
            static_cast<FXAAPass*>(base);

        auto& settings =
            p->getSettings();

        ImGui::DragFloat(
            "Edge Threshold",
            &settings.edgeThreshold,
            0.001f,
            0.f,
            1.f
        );

        ImGui::DragFloat(
            "Blend Strength",
            &settings.blendStrength,
            0.001f,
            0.f,
            1.f
        );
    }},


    { typeid(ToneMappingPass), [](RenderPass* base) {

        auto* p =
            static_cast<ToneMappingPass*>(base);

        auto& settings =
            p->getSettings();

        ImGui::DragFloat(
            "Exposure",
            &settings.exposure,
            0.05f,
            -20.f,
            20.f
        );
    }},
    };



    // ── Component adder map ───────────────────────────────────────────────────

    componentAdderMap = {

        { "Tag",             [](entt::registry& r, entt::entity e) { r.emplace_or_replace<TagComponent>(e, "unnamed"); }},

        { "Parent",          [](entt::registry& r, entt::entity e) { r.emplace_or_replace<ParentComponent>(e); }},

        { "Transform",       [](entt::registry& r, entt::entity e) { r.emplace_or_replace<TransformComponent>(e); }},

        { "World Matrix",    [](entt::registry& r, entt::entity e) { r.emplace_or_replace<WorldMatrixComponent>(e); }},

        { "Active Camera",   [](entt::registry& r, entt::entity e) { r.emplace_or_replace<ActiveCameraTag>(e); }},

        { "Camera",          [](entt::registry& r, entt::entity e) { r.emplace_or_replace<CameraComponent>(e); }},

        { "CubeMap",         [](entt::registry& r, entt::entity e) { r.emplace_or_replace<CubeMapComponent>(e); }},

        { "Light",           [](entt::registry& r, entt::entity e) { r.emplace_or_replace<LightComponent>(e); }},

        { "Renderable",      [](entt::registry& r, entt::entity e) { r.emplace_or_replace<RenderableComponent>(e); }},

        { "Collision Shape", [](entt::registry& r, entt::entity e) { r.emplace_or_replace<CollisionShapeComponent>(e); }},

        { "Rigid Body",      [](entt::registry& r, entt::entity e) { r.emplace_or_replace<RigidBodyComponent>(e); }},

        { "Soft Body",       [](entt::registry& r, entt::entity e) { r.emplace_or_replace<SoftBodyComponent>(e); }},

    };


    componentRenderMap = {

        { typeid(TagComponent), [](entt::registry& r, entt::entity e, bool& del) {

            if (!BeginComponentHeader("Tag", del)) return;

            auto& t = r.get<TagComponent>(e);

            char buf[256];

            strncpy_s(buf, t.tag.c_str(), sizeof(buf));

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

            static std::unordered_map<uint32_t, bool>    eulerMode;

            static std::unordered_map<uint32_t, Vector3> eulerCache;

            static std::unordered_map<uint32_t, Quat>    lastQuat;

            uint32_t id = (uint32_t)e;

            auto [modeIt,  modeInserted]  = eulerMode.emplace(id, true);

            auto [cacheIt, cacheInserted] = eulerCache.emplace(id, t.rotation.toEulerDeg());

            auto [lqIt,    lqInserted]    = lastQuat.emplace(id, t.rotation);

            bool& showEuler = modeIt->second;

            Vector3& euler  = cacheIt->second;

            Quat& prevQuat  = lqIt->second;

            bool externalChange =

                prevQuat.x != t.rotation.x || prevQuat.y != t.rotation.y ||

                prevQuat.z != t.rotation.z || prevQuat.w != t.rotation.w;

            if (showEuler) {

                if (externalChange && !ImGui::IsItemActive())

                    euler = t.rotation.toEulerDeg();

                if (ImGui::DragFloat3("Rotation", &euler.x, 0.5f))

                    t.rotation = Quat::fromEulerDeg(euler).normalized();

            } else {

                float buf[4] = { t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w };

                if (ImGui::DragFloat4("Rotation", buf, 0.001f))

                    t.rotation = Quat(buf[0], buf[1], buf[2], buf[3]).normalized();

            }

            ImGui::SameLine();

            if (ImGui::SmallButton(showEuler ? "E" : "Q")) {

                showEuler = !showEuler;

                if (showEuler) euler = t.rotation.toEulerDeg();

            }

            prevQuat = t.rotation;

            DragVec3("Scale", t.scale, 0.01f);

            EndComponentHeader();

        }},

        { typeid(WorldMatrixComponent), [](entt::registry&, entt::entity, bool& del) {

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

            ImGui::DragFloat("Near",         &c.nearPlane,    0.001f, 0.001f, 10.f);

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

            ImGui::DragFloat("Intensity", &l.intensity, 0.01f, 0.f, 100.f);

            if (l.type == LightType::Spot) {

                ImGui::DragFloat("Inner Cone", &l.innerConeAngle, 0.005f, 0.f, 1.f);

                ImGui::DragFloat("Outer Cone", &l.outerConeAngle, 0.005f, 0.f, 1.f);

            }

            ImGui::Checkbox("Casts Shadow", &l.castsShadow);

            ImGui::DragFloat("Shadow near plane", &l.shadowNearPlane);
            ImGui::DragFloat("Shadow far plane", &l.shadowFarPlane);

            if(l.castsShadow || l.type == LightType::Directional)
                ImGui::DragFloat("Ortho Size", &l.shadowOrthoSize);
            EndComponentHeader();

        }},

        { typeid(RenderableComponent), [&](entt::registry& r, entt::entity e, bool& del)
        {
            if (!BeginComponentHeader("Renderable", del))
                return;

            auto& renderer =
                r.get<RenderableComponent>(e);

            ImGui::Checkbox(
                "Visible",
                &renderer.visible
            );

            ImGui::Separator();

            ImGui::TextUnformatted("Mesh");

            if (!renderer.mesh.valueless_by_exception())
            {
                std::visit(
                    [&](const auto& meshID)
                    {
                        using ID = std::decay_t<decltype(meshID)>;

                        if (!meshID.isValid())
                        {
                            ImGui::TextDisabled("Invalid");
                            return;
                        }

                        if constexpr (
                            std::is_same_v<ID, StaticMeshID>
                        )
                        {
                            ImGui::TextDisabled("Type: Static");
                        }
                        else if constexpr (
                            std::is_same_v<ID, SkinnedMeshID>
                        )
                        {
                            ImGui::TextDisabled("Type: Skinned");
                        }
                    },
                    renderer.mesh
                );
            }
            else
            {
                ImGui::TextDisabled("No mesh assigned");
            }

            ImGui::Separator();

            // --------------------------------------------------------
            // Material
            // --------------------------------------------------------

            ImGui::TextUnformatted("Material");

            if (!renderer.material.isValid())
            {
                ImGui::TextDisabled("No material assigned");
            }
            else
            {
                ImGui::TextDisabled("Assigned");

                if (auto* material =
                    assetManager.materials().getMaterial(
                        renderer.material))
                {
                    ImGui::Spacing();

                    ImGui::DragFloat(
                        "Metallic",
                        &material->metallic,
                        0.01f,
                        0.f,
                        1.f
                    );

                    ImGui::DragFloat(
                        "Roughness",
                        &material->roughness,
                        0.01f,
                        0.f,
                        1.f
                    );

                    ImGui::DragFloat(
                        "AO",
                        &material->ao,
                        0.01f,
                        0.f,
                        1.f
                    );

                    float bc[4] =
                    {
                        material->baseColorFactor.x,
                        material->baseColorFactor.y,
                        material->baseColorFactor.z,
                        material->baseColorFactor.w
                    };

                    if (ImGui::ColorEdit4(
                        "Base Color",
                        bc
                    ))
                    {
                        material->baseColorFactor =
                        {
                            bc[0],
                            bc[1],
                            bc[2],
                            bc[3]
                        };
                    }

                    float ec[3] =
                    {
                        material->emissiveFactor.x,
                        material->emissiveFactor.y,
                        material->emissiveFactor.z
                    };

                    if (ImGui::ColorEdit3(
                        "Emissive",
                        ec
                    ))
                    {
                        material->emissiveFactor =
                        {
                            ec[0],
                            ec[1],
                            ec[2]
                        };
                    }

                    ImGui::Spacing();
                    ImGui::Separator();

                    ImGui::TextUnformatted("Textures");
                    ImGui::Separator();
                    ImGui::Spacing();

                    static const char* slotLabels[] =
                    {
                        "Albedo",
                        "ARM",
                        "Normal",
                        "Emissive"
                    };

                    constexpr int kDisplaySlots = 4;

                    for (int slot = 0;
                        slot < kDisplaySlots;
                        ++slot)
                    {
                        DrawTextureSlot(
                            slotLabels[slot],
                            assetManager.textures().getTexture(
                                material->GetTexture(
                                    static_cast<MaterialSlot>(slot)
                                )
                            ),
                            56.f
                        );

                        ImGui::Spacing();
                    }
                }
                else
                {
                    ImGui::TextDisabled(
                        "Material handle is valid, but resource is missing."
                    );
                }
            }

            EndComponentHeader();
        }
    },

        { typeid(SkeletonComponent), [&assetManager](entt::registry& r, entt::entity e, bool& del) {

            if (!BeginComponentHeader("Skeleton", del))
                return;

            auto skeleton = assetManager.skeletons().getSkeleton(r.get<SkeletonComponent>(e).skeleton);

            ImGui::Text("Bones: %zu", skeleton->bones.size());

            ImGui::Separator();

            for (size_t i = 0; i < skeleton->bones.size(); ++i)
            {
                auto& bone = skeleton->bones[i];

                if (ImGui::TreeNode(
                    ("Bone " + std::to_string(i)).c_str()))
                {
                    // Adjust these field names to whatever SkeletonComponent actually has.
                    ImGui::Text("Name: %s", bone.name.c_str());

                    ImGui::Text("Parent: %d", bone.parentId);

                    ImGui::Separator();

                    ImGui::Text("Local Transform");

                    DragVec3("Position", bone.pos);
                    DragQuat("Rotation", bone.rot);
                    DragVec3("Scale", bone.scale, 0.01f);

                    ImGui::TreePop();
                }
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

        { typeid(AnimationState), [&assetManager](entt::registry& r, entt::entity e, bool& del) {

        if (!BeginComponentHeader("Animation State", del))
            return;

        auto& state = r.get<AnimationState>(e);

        const AnimationClip* clip = nullptr;
        if (state.clip.isValid())
            clip = &assetManager.animations().Get(state.clip);

        if (clip)
        {
            ImGui::Text("Clip: %s", clip->name.c_str());
            ImGui::Text("Duration: %.2fs", clip->duration);
            ImGui::Text("Tracks: %zu", clip->tracks.size());
        }
        else
        {
            ImGui::TextDisabled("No clip assigned / invalid handle");
        }

        ImGui::Separator();

        ImGui::Checkbox("Playing", &state.playing);
        ImGui::SameLine();
        ImGui::Checkbox("Looping", &state.looping);

        if (clip && clip->duration > 0.0f)
        {
            ImGui::SliderFloat("Time", &state.time, 0.0f, clip->duration);
        }
        else
        {
            ImGui::DragFloat("Time", &state.time, 0.01f, 0.0f, 0.0f);
        }

        ImGui::DragFloat("Speed", &state.speed, 0.01f, -4.0f, 4.0f);

        ImGui::Separator();
        ImGui::TextDisabled("Cache version: %u", state.lastSeenClipVersion);

        if (ImGui::SmallButton("Restart"))
            state.time = 0.0f;
        ImGui::SameLine();
        if (ImGui::SmallButton(state.playing ? "Pause" : "Play"))
            state.playing = !state.playing;

        EndComponentHeader();

    }},

        { typeid(SkeletalAnimationTarget), [&assetManager](entt::registry& r, entt::entity e, bool& del) {

            if (!BeginComponentHeader("Skeletal Animation Target", del))
                return;

            auto& target = r.get<SkeletalAnimationTarget>(e);

            auto* skeleton = assetManager.skeletons().getSkeleton(target.skeleton);

            if (!skeleton)
            {
                ImGui::TextDisabled("Skeleton handle invalid or unresolved");
                EndComponentHeader();
                return;
            }

            int mapped = 0;
            for (int b : target.trackToJoint)
                if (b >= 0) ++mapped;

            ImGui::Text("Bound skeleton bones: %zu", skeleton->bones.size());
            ImGui::Text("Mapped tracks: %d / %zu", mapped, target.trackToJoint.size());

            if (mapped < (int)target.trackToJoint.size())
                ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.2f, 1.f), "Some tracks are unmapped!");

            ImGui::Separator();

            if (ImGui::TreeNode("Track -> Bone Mapping"))
            {
                for (size_t t = 0; t < target.trackToJoint.size(); ++t)
                {
                    int boneIdx = target.trackToJoint[t];

                    if (boneIdx < 0)
                    {
                        ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.4f, 1.f),
                            "Track %zu -> (unmapped)", t);
                    }
                    else if (boneIdx < (int)skeleton->bones.size())
                    {
                        ImGui::Text("Track %zu -> Bone %d (%s)",
                            t, boneIdx, skeleton->bones[boneIdx].name.c_str());
                    }
                    else
                    {
                        ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.4f, 1.f),
                            "Track %zu -> Bone %d (OUT OF RANGE)", t, boneIdx);
                    }
                }

                ImGui::TreePop();
            }

            EndComponentHeader();

            }},
    };
}



void UiInput::startNewFrame()

{

    ImGui_ImplOpenGL3_NewFrame();

    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

}



void UiInput::render()

{

    ImGui::Render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    

    Vector2 windowSize{platform.GetFramebufferSize()};

    float w {windowSize.x}, h {windowSize.y};
    glViewport(0, 0, w, h);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}



void UiInput::ApplyEditorStyle()

{

    ImGuiStyle& s = ImGui::GetStyle();

    ImGui::StyleColorsDark();

    s.WindowRounding = 4.f; s.FrameRounding  = 3.f;

    s.ChildRounding  = 3.f; s.GrabRounding   = 3.f;

    s.FramePadding   = ImVec2(6, 3);

    s.ItemSpacing    = ImVec2(6, 4);

    s.WindowBorderSize = 1.f; s.FrameBorderSize = 0.f;

    ImVec4* c = s.Colors;

    c[ImGuiCol_WindowBg]      = ImVec4(0.10f, 0.10f, 0.12f, 0.97f);

    c[ImGuiCol_ChildBg]       = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);

    c[ImGuiCol_Header]        = ImVec4(0.22f, 0.22f, 0.28f, 1.00f);

    c[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.38f, 1.00f);

    c[ImGuiCol_HeaderActive]  = ImVec4(0.25f, 0.35f, 0.55f, 1.00f);

    c[ImGuiCol_FrameBg]       = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);

    c[ImGuiCol_FrameBgHovered]= ImVec4(0.22f, 0.22f, 0.28f, 1.00f);

    c[ImGuiCol_Button]        = ImVec4(0.22f, 0.30f, 0.50f, 1.00f);

    c[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.38f, 0.62f, 1.00f);

    c[ImGuiCol_SliderGrab]    = ImVec4(0.35f, 0.55f, 0.90f, 1.00f);

    c[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.18f, 0.30f, 1.00f);

    c[ImGuiCol_Separator]     = ImVec4(0.28f, 0.28f, 0.35f, 1.00f);

}



void UiInput::createWindow(const std::string& title, std::function<void(bool&)> drawFunc)

{

    for (auto& w : windows)

        if (w.title == title) { w.open = true; return; }

    windows.push_back({ title, drawFunc });

}



void UiInput::DrawRenderPassWindow(RenderPass* pass,

    const std::function<void(RenderPass*)>& drawFn, Vector2 windowSizes)

{

    const float windowWidth = windowSizes.x;

    const float pad = 10.f;

    const float w = windowWidth / 5.f;



    ImGui::SetNextWindowSize(ImVec2(w, 0), ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowPos(

        ImVec2( windowWidth - 2.f * w - 2.f * pad, pad),

        ImGuiCond_FirstUseEver);

    bool open = true;

    std::string title = std::string(pass->passName()) + " Settings";

    ImGui::Begin(title.c_str(), &open, 0);

    drawFn(pass);

    ImGui::End();

    if (!open) activePassIndex = -1;

}



void UiInput::DrawModelManagerWindow(ModelManager*, entt::registry&, Vector2)

{
 /*   const float windowWidth = windowSizes.x;
    const float windowHeight = windowSizes.y;

    const float pad = 10.f;

    const float w = windowWidth / 5.f;

    float gbufH = windowHeight / 2.f;

    ImGui::SetNextWindowPos(

        ImVec2(windowWidth - 2 * w - 2 * pad, windowHeight - gbufH - pad),

        ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowSize(ImVec2(w, gbufH), ImGuiCond_FirstUseEver);

    ImGui::Begin("Model Manager", nullptr, 0);



    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.f, 1.f), "Load Model");

    ImGui::Separator();

    static char nameBuf[128] = {};

    static char pathBuf[256] = {};

    ImGui::InputTextWithHint("##name", "Model name", nameBuf, sizeof(nameBuf));

    ImGui::InputTextWithHint("##path", "Path to model file", pathBuf, sizeof(pathBuf));

    bool canLoad = nameBuf[0] && pathBuf[0];

    if (!canLoad) ImGui::BeginDisabled();

    if (ImGui::Button("Load Model", ImVec2(-1, 0))) {

        mgr->loadModel(nameBuf, pathBuf);

        nameBuf[0] = '\0'; pathBuf[0] = '\0';

    }

    if (!canLoad) ImGui::EndDisabled();

    ImGui::Spacing(); ImGui::Separator();



    ImGui::TextColored(ImVec4(0.75f, 0.55f, 1.f, 1.f), "Loaded Models");

    ImGui::Separator();


    static int selectedModel = -1;

    int index = 0;

    for (const auto& [name, asset] : models) {

        bool selected = (selectedModel == index);

        if (ImGui::Selectable(name.c_str(), selected)) selectedModel = index;

        if (ImGui::IsItemHovered()) {

            ImGui::BeginTooltip();

            ImGui::Text("Path: %s",   asset.get()->path.c_str());

            ImGui::Text("Meshes: %zu", asset.get()->meshes.size());

            ImGui::EndTooltip();

        }

        index++;

    }

    ImGui::Spacing(); ImGui::Separator();



    ImGui::TextColored(ImVec4(0.6f, 1.f, 0.6f, 1.f), "Spawn");

    static char entityName[128] = "ModelEntity";

    ImGui::InputText("Entity Name", entityName, sizeof(entityName));

    if (ImGui::Button("Create Entity + Assign Model", ImVec2(-1, 0))) {

        if (selectedModel >= 0) {

            auto it = models.begin();

            std::advance(it, selectedModel);

            auto entity = registry.create();

            registry.emplace<TagComponent>(entity, std::string(entityName));

            activeInspectorEntity = entity;

        }

    }

    ImGui::End();*/

}

UiInput::~UiInput()
{
    Shutdown();
}

void UiInput::Shutdown()
{
    if (!initialized)
        return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    initialized = false;
}

void UiInput::buildUI(entt::registry& registry,
                       Vector2 windowSize,
                       const DebugRenderData&,
                       RenderGraph* rendergraph)
{

    ModelManager* modelMgr {&assetManager.models()};

    const float windowWidth = windowSize.x;
    const float windowHeight = windowSize.y;

    const float pad    = 10.f;

    const float panelW = windowWidth / 5.f;

    const float inspW  = windowWidth / 5.f;

    const float gbufW  = windowHeight / 5.f;

    const float usableW = panelW - 2.f * pad;

    const float usableH = windowHeight - 2.f * pad;



    ImGui::SetNextWindowSize(ImVec2(usableW, usableH), ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowPos(ImVec2(pad, pad), ImGuiCond_FirstUseEver);

    ImGui::Begin("Scene", nullptr, 0);

    ImGui::Columns(3, "scene_cols", true);



    // ── Column 0: Component adder ─────────────────────────────────────────────

    ImGui::BeginChild("##compadder", ImVec2(0.f, 0.f), true);

    if (registry.valid(activeInspectorEntity)) {

        std::string activeName = "Entity " + std::to_string((uint32_t)activeInspectorEntity);

        if (registry.all_of<TagComponent>(activeInspectorEntity))

            activeName = registry.get<TagComponent>(activeInspectorEntity).tag;

        ImGui::TextColored(ImVec4(0.40f, 0.80f, 1.f, 1.f), "%s %s",

            reinterpret_cast<const char*>(u8"\u2605"), activeName.c_str());

    } else ImGui::TextDisabled("No entity selected");

    ImGui::Separator();

    static char compFilter[128] = {};

    ImGui::SetNextItemWidth(-1.f);

    ImGui::InputTextWithHint("##cfilter", "Search components...", compFilter, sizeof(compFilter));

    ImGui::Spacing();

    if (registry.valid(activeInspectorEntity))

        for (auto& [name, adderFn] : componentAdderMap) {

            if (!StrContainsCI(name, compFilter)) continue;

            ImGui::PushID(name.c_str());

            if (ImGui::Selectable(("  + " + name).c_str())) adderFn(registry, activeInspectorEntity);

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

    auto view = registry.view<entt::entity>();

    for (auto entity : view) {

        if (!registry.valid(entity)) continue;

        std::string displayName = "Entity " + std::to_string((uint32_t)entity);

        if (registry.all_of<TagComponent>(entity))

            displayName = registry.get<TagComponent>(entity).tag;

        if (!StrContainsCI(displayName, entityFilter)) continue;

        bool isActive = (entity == activeInspectorEntity);

        if (isActive) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.80f, 1.00f, 1.f));

        if (ImGui::Selectable(displayName.c_str(), isActive)) activeInspectorEntity = entity;

        if (isActive) ImGui::PopStyleColor();

        if (ImGui::BeginPopupContextItem()) {

            ImGui::TextDisabled("%s", displayName.c_str());

            ImGui::Separator();

            if (ImGui::MenuItem("Delete Entity")) toDestroy = entity;

            ImGui::EndPopup();

        }

    }

    if (registry.valid(toDestroy)) {

        if (activeInspectorEntity == toDestroy) activeInspectorEntity = entt::null;

        registry.destroy(toDestroy);

    }

    if (ImGui::BeginPopup("NewEntityPopup")) {

        ImGui::Text("New Entity Name");

        ImGui::Separator();

        ImGui::SetNextItemWidth(220.f);

        if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();

        bool confirm = ImGui::InputText("##newname", newEntityNameBuf, sizeof(newEntityNameBuf),

            ImGuiInputTextFlags_EnterReturnsTrue);

        ImGui::SameLine();

        confirm |= ImGui::Button("Create");

        if (confirm && newEntityNameBuf[0] != '\0') {

            auto e = registry.create();

            registry.emplace<TagComponent>(e, std::string(newEntityNameBuf));

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



     ImGui::BeginChild("##systems", ImVec2(0.f, 0.f), true);
    ImGui::TextColored(ImVec4(0.75f, 0.55f, 1.00f, 1.f), "Engine Submodules");
    ImGui::Separator(); ImGui::Spacing();

    struct SystemEntry { const char* label; bool* openFlag; };
    SystemEntry systemEntries[] = {
        { "Model Manager", &modelManagerOpen },
        { "Render Graph",  &renderGraphOpen },
    };

    for (auto& entry : systemEntries) {
        bool isOpen = *entry.openFlag;
        if (isOpen) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.80f, 1.00f, 1.f));
        if (ImGui::Selectable(entry.label, isOpen)) *entry.openFlag = !(*entry.openFlag);
        if (isOpen) ImGui::PopStyleColor();
    }

    ImGui::EndChild();

    ImGui::Columns(1);

    ImGui::End();

    if (rendergraph && renderGraphOpen)
    {
        DrawRenderGraphWindow(rendergraph, windowSize);
        DrawFramebufferStatesWindow(rendergraph, windowSize);
    }


    if (rendergraph && activePassIndex >= 0) {

        const auto& passes = rendergraph->getPasses();

        if (activePassIndex < (int)passes.size()) {

            RenderPass* pass = passes[activePassIndex].get();

            auto it = passRenderMap.find(typeid(*pass));

            if (it != passRenderMap.end())

                DrawRenderPassWindow(pass, it->second, windowSize);

        }

    }



    if (registry.valid(activeInspectorEntity)) {

        std::string entityName = "Entity " + std::to_string((uint32_t)activeInspectorEntity);

        if (registry.all_of<TagComponent>(activeInspectorEntity))

            entityName = registry.get<TagComponent>(activeInspectorEntity).tag;

        std::string title = std::string(reinterpret_cast<const char*>(u8"\u2605 ")) + entityName + "##insp";

        ImGui::SetNextWindowSize(ImVec2(inspW, usableH / 2), ImGuiCond_FirstUseEver);

        ImGui::SetNextWindowPos(ImVec2(windowWidth - inspW - pad, pad), ImGuiCond_FirstUseEver);

        bool windowOpen = true;

        ImGui::Begin(title.c_str(), &windowOpen, 0);

        ImGui::Text("ID: %u", (uint32_t)activeInspectorEntity);

        ImGui::Separator();

        renderComponents<ComponentTypes>(registry, activeInspectorEntity, componentRenderMap);

        ImGui::End();

        if (!windowOpen) activeInspectorEntity = entt::null;

    }



    if (modelManagerOpen && modelMgr)

        DrawModelManagerWindow(modelMgr, registry, windowSize);



    float gbufH = windowHeight / 2.f;

    ImGui::SetNextWindowSize(ImVec2(gbufW - pad, gbufH), ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowPos(

        ImVec2(windowWidth - gbufW - pad, windowHeight - gbufH - pad),

        ImGuiCond_FirstUseEver);

   /* ImGui::Begin("Render Passes", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    const float thumbW = ImGui::GetContentRegionAvail().x;

    const float thumbH = thumbW * (9.f / 16.f);

    for (auto& dt : debugData.debugTextures) {

        if (dt.textureID == 0) continue;

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.85f, 1.00f, 1.f));

        ImGui::TextUnformatted(dt.name.c_str());

        ImGui::PopStyleColor();

        ImGui::SameLine(thumbW - 60.f);

        ImGui::TextDisabled("id %u", dt.textureID);

        ImTextureID imID = (ImTextureID)(intptr_t)dt.textureID;

        ImGui::Image(imID, ImVec2(thumbW, thumbH), ImVec2(0,1), ImVec2(1,0));

        if (ImGui::IsItemHovered()) {

            ImGui::BeginTooltip();

            ImGui::Image(imID, ImVec2(512.f, 512.f * (9.f/16.f)), ImVec2(0,1), ImVec2(1,0));

            ImGui::TextDisabled("%s  |  id %u", dt.name.c_str(), dt.textureID);

            ImGui::EndTooltip();

        }

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    }

    ImGui::End();
*/


    for (auto it = windows.begin(); it != windows.end();) {

        ImGui::Begin(it->title.c_str(), &it->open);

        it->drawFunc(it->open);

        ImGui::End();

        if (!it->open) it = windows.erase(it);

        else           ++it;

    }

}

void UiInput::DrawRenderGraphWindow(RenderGraph* rendergraph, Vector2 windowSizes)
{
    const float windowWidth  = windowSizes.x;
    const float windowHeight = windowSizes.y;
    const float pad = 10.f;
    const float w = windowWidth / 3.5f;
    const float h = windowHeight - 2.f * pad;

    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(pad, pad), ImGuiCond_FirstUseEver);

    bool open = renderGraphOpen;
    ImGui::Begin("Render Graph", &open, ImGuiWindowFlags_NoCollapse);

    if (!rendergraph)
    {
        ImGui::TextDisabled("No render graph attached.");
        ImGui::End();
        renderGraphOpen = open;
        return;
    }

    auto passInfo = rendergraph->getPassDebugInfo();
    const auto& passes = rendergraph->getPasses();

    // ── Execution order ──────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Execution Order", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const auto& order = rendergraph->getExecutionOrder();

        if (order.empty())
        {
            ImGui::TextDisabled("Not compiled yet, or nothing is live this frame.");
        }
        else
        {
            for (size_t i = 0; i < order.size(); ++i)
                ImGui::Text("%2zu.  %s", i + 1, order[i]->passName());
        }
    }

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // ── All registered passes ───────────────────────────────────────────
    if (ImGui::CollapsingHeader("All Passes", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (int i = 0; i < (int)passInfo.size(); ++i)
        {
            const auto& info = passInfo[i];

            ImVec4 stateColor;
            const char* stateLabel;

            if (!info.active)
            {
                stateColor = ImVec4(0.6f, 0.6f, 0.6f, 1.f);
                stateLabel = "inactive";
            }
            else if (!info.inExecutionOrder)
            {
                stateColor = ImVec4(0.9f, 0.55f, 0.2f, 1.f);
                stateLabel = "culled";
            }
            else
            {
                stateColor = ImVec4(0.4f, 0.85f, 0.4f, 1.f);
                stateLabel = "running";
            }

            ImGui::PushID(i);

            RenderPass& passRef = *passes[i];
            bool hasSettings =
                passRenderMap.find(typeid(passRef)) != passRenderMap.end();

            std::string label =
                info.name + "  (id " + std::to_string((uint32_t)info.id) + ")";

            bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);

            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 90.f);
            ImGui::TextColored(stateColor, "%s", stateLabel);

            if (info.hasSideEffect)
            {
                ImGui::SameLine();
                ImGui::TextDisabled("(side effect)");
            }

            if (nodeOpen)
            {
                if (hasSettings)
                {
                    if (ImGui::SmallButton("Open Settings"))
                        activePassIndex = (activePassIndex == i) ? -1 : i;
                }

                auto res = rendergraph->getResourcesForPass(info.id);

                if (res.empty())
                {
                    ImGui::TextDisabled("No resources attached.");
                }
                else
                {
                    for (auto& r : res)
                    {
                        const char* role = (r.producer == info.id) ? "writes" : "reads";

                        ImGui::BulletText(
                            "%-6s  %-24s  (res id %u)%s",
                            role,
                            r.debugName.c_str(),
                            (uint32_t)r.id,
                            r.imported ? "  [imported]" : ""
                        );
                    }
                }

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }

    ImGui::End();
    renderGraphOpen = open;
}

void UiInput::DrawFramebufferStatesWindow(RenderGraph* rendergraph, Vector2 windowSizes)
{
    const float windowWidth  = windowSizes.x;
    const float windowHeight = windowSizes.y;
    const float pad = 10.f;
    const float w = windowWidth / 4.f;
    const float h = windowHeight - 2.f * pad;

    ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(windowWidth - w - pad, pad), ImGuiCond_FirstUseEver);

    ImGui::Begin("Render Graph Textures", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    if (!rendergraph)
    {
        ImGui::TextDisabled("No render graph attached.");
        ImGui::End();
        return;
    }

    auto resourceInfo = rendergraph->getResourceDebugInfo();

    std::sort(
        resourceInfo.begin(), resourceInfo.end(),
        [](const RenderGraph::ResourceDebugInfo& a, const RenderGraph::ResourceDebugInfo& b)
        { return a.debugName < b.debugName; }
    );

    const float thumbW = ImGui::GetContentRegionAvail().x;

    for (auto& r : resourceInfo)
    {
        if (!r.isTexture || r.glTextureID == 0)
            continue;

        const float aspect =
            (r.width > 0 && r.height > 0)
                ? static_cast<float>(r.height) / static_cast<float>(r.width)
                : (9.f / 16.f);

        const float thumbH = thumbW * aspect;

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.55f, 0.85f, 1.00f, 1.f));
        ImGui::TextUnformatted(r.debugName.c_str());
        ImGui::PopStyleColor();

        ImGui::SameLine(thumbW - 90.f);
        ImGui::TextDisabled("%ux%u", r.width, r.height);

        ImTextureID imID = (ImTextureID)(intptr_t)r.glTextureID;
        ImGui::Image(imID, ImVec2(thumbW, thumbH), ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Image(imID, ImVec2(512.f, 512.f * aspect), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TextDisabled("%s  |  gl id %u", r.debugName.c_str(), r.glTextureID);
            ImGui::EndTooltip();
        }

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    }

    ImGui::End();
}