# ============================================================================
# ShanTT Game Engine - CMake Configuration
# ============================================================================
cmake_minimum_required(VERSION 3.20)
project(ShanTT VERSION 1.0.0 LANGUAGES CXX C)

# ============================================================================
# CMake Modules Path
# ============================================================================
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(ENGINE_PLATFORM_BACKEND "GLFW" CACHE STRING "Platform backend: GLFW | SDL | WIN32")
set_property(CACHE ENGINE_PLATFORM_BACKEND PROPERTY STRINGS GLFW SDL WIN32)

# Windows users
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()
# ============================================================================
# Build Configuration
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Source Files Organization
# ============================================================================

# Main entry point
set(MAIN_SOURCES
    source/ShanTT.cpp
)

# Core engine sources
set(CORE_SOURCES
    source/core/Engine.cpp
)

set(CORE_HEADERS
    source/core/Engine.h
    source/core/EngineContext.h
    source/core/Event.h
    source/core/EventBus.h
)

# ECS Components (header-only)
set(ECS_HEADERS
    source/ecs/components/core/ParentComponent.h
    source/ecs/components/core/TagComponent.h
    source/ecs/components/core/TransformComponent.h
    source/ecs/components/core/WorldMatrixComponent.h
    source/ecs/components/graphics/ActiveCameraTag.h
    source/ecs/components/graphics/CameraComponent.h
    source/ecs/components/graphics/CubeMapComponent.h
    source/ecs/components/graphics/LightComponent.h
    source/ecs/components/graphics/MaterialComponent.h
    source/ecs/components/graphics/MeshComponent.h
    source/ecs/components/graphics/ModelComponent.h
    source/ecs/components/physics/CollisionShapeComponent.h
    source/ecs/components/physics/RigidBodyComponent.h
    source/ecs/components/physics/SoftBodyComponent.h
)

# ECS Factories
set(ECS_FACTORY_SOURCES
    source/ecs/factories/GraphicsEntityFactory.cpp
)

set(ECS_FACTORY_HEADERS
    source/ecs/factories/GraphicsEntityFactory.h
    source/ecs/factories/PhysicsComponentFactory.h
)

# Input system
set(INPUT_SOURCES
    source/input/Keyboard/Keyboard.cpp
    source/input/Mouse/Mouse.cpp
    source/input/InputManager.cpp
    source/input/UIInput.cpp
)

set(INPUT_HEADERS
    source/input/Keyboard/Keyboard.h
    source/input/Mouse/Mouse.h
    source/input/InputManager.h
    source/input/UIInput.h
)

# Third-party library implementations
set(LIBRARY_SOURCES
    source/libraries/stb.cpp
    source/libraries/imgui/imgui.cpp
    source/libraries/imgui/imgui_demo.cpp
    source/libraries/imgui/imgui_draw.cpp
    source/libraries/imgui/imgui_impl_glfw.cpp
    source/libraries/imgui/imgui_impl_opengl3.cpp
    source/libraries/imgui/imgui_tables.cpp
    source/libraries/imgui/imgui_widgets.cpp
    source/libraries/glad.c
)

set_source_files_properties(source/libraries/glad.c PROPERTIES LANGUAGE C)

# Math library (header-only)
set(MATH_HEADERS
    source/math_custom/GeneralFunctions.h
    source/math_custom/Mat4.h
    source/math_custom/Quat.h
    source/math_custom/Vector2.h
    source/math_custom/Vector3.h
    source/math_custom/Vector4.h
)

# Physics engine
set(PHYSICS_SOURCES
    source/physics/PhysicsEngine.cpp
    source/physics/dynamics/ForceGenerator.cpp
    source/physics/dynamics/GeneralIntegrators.cpp
    source/physics/ecs_systems/RigidBodyIntegrator.cpp
)

set(PHYSICS_HEADERS
    source/physics/PhysicsEngine.h
    source/physics/dynamics/ForceGenerator.h
    source/physics/dynamics/GeneralIntegrators.h
    source/physics/dynamics/colider/Header.h
    source/physics/ecs_systems/RigidBodyIntegrator.h
    source/physics/particles/Particle.h
)

# Rendering system
set(RENDER_SOURCES
    source/render/Renderer.cpp
    source/render/backend/Shader.cpp
    source/render/backend/ShaderManager.cpp
    source/render/backend/containers/RenderBuffer.cpp
)

set(RENDER_HEADERS
    source/render/Renderer.h
    source/render/RenderGraph.h
    source/render/backend/Shader.h
    source/render/backend/ShaderManager.h
    source/render/backend/containers/EBO.h
    source/render/backend/containers/FrameBuffer.h
    source/render/backend/containers/GlobalUniformBuffer.h
    source/render/backend/containers/RenderBuffer.h
    source/render/backend/containers/VAO.h
    source/render/backend/containers/VBO.h
    source/render/data/BatchMap.h
    source/render/data/GPULight.h
    source/render/data/MeshBatch.h
    source/render/data/DebugRenderData.h
    source/render/data/EngineResources.h
    source/render/data/FrameRenderData.h
    source/render/data/RenderResource.h
    source/render/data/ShaderType.h
    source/render/data/ShadowData.h
    source/render/ecs_systems/ShadowAtlas.h
    source/render/ecs_systems/ShadowSystem.h
    source/render/handlers/BloomPass.h
    source/render/handlers/BlurPass.h
    source/render/handlers/CubeMapPass.h
    source/render/handlers/FinalBlitPass.h
    source/render/handlers/FXAAPass.h
    source/render/handlers/GeometryPass.h
    source/render/handlers/LightingPass.h
    source/render/handlers/RenderPass.h
    source/render/handlers/ShadowPass.h
)

# Resource management
set(RESOURCE_SOURCES
    source/resources/assets/CubeMap.cpp
    source/resources/assets/Material.cpp
    source/resources/assets/StaticMesh.cpp
    source/resources/assets/Texture.cpp
    source/resources/loaders/ModelLoader.cpp
    source/resources/loaders/TextureLoader.cpp
    source/resources/managers/MaterialManager.cpp
    source/resources/managers/MeshManager.cpp
    source/resources/managers/ModelManager.cpp
    source/resources/managers/TextureManager.cpp
)

set(RESOURCE_HEADERS
    source/resources/assets/CubeMap.h
    source/resources/assets/Material.h
    source/resources/assets/Model.h
    source/resources/assets/RenderMesh.h
    source/resources/assets/StaticMesh.h
    source/resources/assets/Texture.h
    source/resources/data/GlobalUBOData.h
    source/resources/data/MaterialData.h
    source/resources/data/MaterialTextureInfo.h
    source/resources/data/MeshData.h
    source/resources/data/ModelAsset.h
    source/resources/data/ModelData.h
    source/resources/data/ModelLoadConfig.h
    source/resources/data/ShaderData.h
    source/resources/data/SubMeshData.h
    source/resources/data/TextureType.h
    source/resources/data/Vertex.h
    source/resources/loaders/ModelLoader.h
    source/resources/loaders/TextureLoader.h
    source/resources/managers/MaterialManager.h
    source/resources/managers/MeshManager.h
    source/resources/managers/ModelManager.h
    source/resources/managers/TextureManager.h
)
# Platform-specific headers
set(PLATFORM_HEADERS
    source/platform/IPlatform.h
)
set(PLATFORM_SOURCES "")

if(ENGINE_PLATFORM_BACKEND STREQUAL "GLFW")
    list(APPEND PLATFORM_HEADERS
        source/platform/GLFW/PlatformGLFW.h
    )
    list(APPEND PLATFORM_SOURCES
        source/platform/GLFW/PlatformGLFW.cpp
        source/platform/GLFW/CreatePlatform.cpp
    )
elseif(ENGINE_PLATFORM_BACKEND STREQUAL "SDL")
    message(FATAL_ERROR "SDL backend not implemented yet")
elseif(ENGINE_PLATFORM_BACKEND STREQUAL "WIN32")
    message(FATAL_ERROR "WIN32 native backend not implemented yet")
endif()

# Scene system
set(SCENE_SOURCES
    source/scene/Scene.cpp
    source/scene/SceneManager.cpp
    source/scene/SceneExtractor.cpp
    source/scene/loaders/SceneLoader.cpp
)

set(SCENE_HEADERS
    source/scene/Scene.h
    source/scene/SceneManager.h
    source/scene/SceneExtractor.h
    source/scene/loaders/SceneLoader.h
)


set(THIRD_PARTY_HEADERS
    include/EnTT/entt.hpp
    include/glad/glad.h
    include/imgui/imconfig.h
    include/imgui/imgui.h
    include/imgui/imgui_impl_glfw.h
    include/imgui/imgui_impl_opengl3.h
    include/imgui/imgui_impl_opengl3_loader.h
    include/imgui/imgui_internal.h
    include/imgui/imstb_rectpack.h
    include/imgui/imstb_textedit.h
    include/imgui/imstb_truetype.h
    include/json/json.h
    include/KHR/khrplatform.h
    include/stb/stb_image.h
)


add_library(ShanTT_Lib STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${ECS_HEADERS}
    ${ECS_FACTORY_SOURCES}
    ${ECS_FACTORY_HEADERS}
    ${INPUT_SOURCES}
    ${INPUT_HEADERS}
    ${LIBRARY_SOURCES}
    ${MATH_HEADERS}
    ${PHYSICS_SOURCES}
    ${PHYSICS_HEADERS}
    ${PLATFORM_HEADERS}
    ${PLATFORM_SOURCES}
    ${RENDER_SOURCES}
    ${RENDER_HEADERS}
    ${RESOURCE_SOURCES}
    ${RESOURCE_HEADERS}
    ${SCENE_SOURCES}
    ${SCENE_HEADERS}
)

add_executable(ShanTT
    ${MAIN_SOURCES}
)

target_link_libraries(ShanTT PRIVATE ShanTT_Lib)

target_include_directories(ShanTT_Lib PUBLIC
    ${CMAKE_SOURCE_DIR}/source
)

target_include_directories(ShanTT_Lib SYSTEM PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/glad
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/EnTT
    ${CMAKE_SOURCE_DIR}/include/json
    ${CMAKE_SOURCE_DIR}/include/stb
    ${CMAKE_SOURCE_DIR}/include/KHR
)

add_custom_command(TARGET ShanTT POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resource"
    "$<TARGET_FILE_DIR:ShanTT>/resource"
    COMMENT "Copying resource to output directory"
)
# Find OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(ShanTT_Lib PRIVATE OpenGL::GL)

# Link GLFW
if(ENGINE_PLATFORM_BACKEND STREQUAL "GLFW")
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)

    target_link_libraries(ShanTT_Lib PRIVATE glfw)
endif()


target_compile_options(ShanTT_Lib PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->

    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Werror>

    $<$<CXX_COMPILER_ID:GNU>:-Wall>
    $<$<CXX_COMPILER_ID:GNU>:-Wextra>
    $<$<CXX_COMPILER_ID:GNU>:-Wpedantic>
    $<$<CXX_COMPILER_ID:GNU>:-Werror>
)



source_group("Source Files\\Core" FILES ${CORE_SOURCES})
source_group("Header Files\\Core" FILES ${CORE_HEADERS})
source_group("Header Files\\ECS\\Components" FILES ${ECS_HEADERS})
source_group("Source Files\\ECS\\Factories" FILES ${ECS_FACTORY_SOURCES})
source_group("Header Files\\ECS\\Factories" FILES ${ECS_FACTORY_HEADERS})
source_group("Source Files\\Input" FILES ${INPUT_SOURCES})
source_group("Header Files\\Input" FILES ${INPUT_HEADERS})
source_group("Source Files\\Libraries" FILES ${LIBRARY_SOURCES})
source_group("Header Files\\Math" FILES ${MATH_HEADERS})
source_group("Source Files\\Physics" FILES ${PHYSICS_SOURCES})
source_group("Header Files\\Physics" FILES ${PHYSICS_HEADERS})
source_group("Source Files\\Render" FILES ${RENDER_SOURCES})
source_group("Header Files\\Render" FILES ${RENDER_HEADERS})
source_group("Source Files\\Resources" FILES ${RESOURCE_SOURCES})
source_group("Header Files\\Resources" FILES ${RESOURCE_HEADERS})
source_group("Source Files\\Scene" FILES ${SCENE_SOURCES})
source_group("Header Files\\Scene" FILES ${SCENE_HEADERS})
source_group("Third Party" FILES ${THIRD_PARTY_HEADERS})


set_target_properties(ShanTT PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)



add_subdirectory(tests)
