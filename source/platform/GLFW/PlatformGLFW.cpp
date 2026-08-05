#include "PlatformGLFW.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "core/EventBus.h"
#include "input/InputTypes.h"

namespace {

KeyCode TranslateKey(int glfwKey)
{
    switch (glfwKey) {
        case GLFW_KEY_A: return KeyCode::A;
        case GLFW_KEY_B: return KeyCode::B;
        case GLFW_KEY_C: return KeyCode::C;
        case GLFW_KEY_D: return KeyCode::D;
        case GLFW_KEY_E: return KeyCode::E;
        case GLFW_KEY_F: return KeyCode::F;
        case GLFW_KEY_G: return KeyCode::G;
        case GLFW_KEY_H: return KeyCode::H;
        case GLFW_KEY_I: return KeyCode::I;
        case GLFW_KEY_J: return KeyCode::J;
        case GLFW_KEY_K: return KeyCode::K;
        case GLFW_KEY_L: return KeyCode::L;
        case GLFW_KEY_M: return KeyCode::M;
        case GLFW_KEY_N: return KeyCode::N;
        case GLFW_KEY_O: return KeyCode::O;
        case GLFW_KEY_P: return KeyCode::P;
        case GLFW_KEY_Q: return KeyCode::Q;
        case GLFW_KEY_R: return KeyCode::R;
        case GLFW_KEY_S: return KeyCode::S;
        case GLFW_KEY_T: return KeyCode::T;
        case GLFW_KEY_U: return KeyCode::U;
        case GLFW_KEY_V: return KeyCode::V;
        case GLFW_KEY_W: return KeyCode::W;
        case GLFW_KEY_X: return KeyCode::X;
        case GLFW_KEY_Y: return KeyCode::Y;
        case GLFW_KEY_Z: return KeyCode::Z;
        case GLFW_KEY_0: return KeyCode::Num0;
        case GLFW_KEY_1: return KeyCode::Num1;
        case GLFW_KEY_2: return KeyCode::Num2;
        case GLFW_KEY_3: return KeyCode::Num3;
        case GLFW_KEY_4: return KeyCode::Num4;
        case GLFW_KEY_5: return KeyCode::Num5;
        case GLFW_KEY_6: return KeyCode::Num6;
        case GLFW_KEY_7: return KeyCode::Num7;
        case GLFW_KEY_8: return KeyCode::Num8;
        case GLFW_KEY_9: return KeyCode::Num9;
        case GLFW_KEY_SPACE:         return KeyCode::Space;
        case GLFW_KEY_ENTER:         return KeyCode::Enter;
        case GLFW_KEY_ESCAPE:        return KeyCode::Escape;
        case GLFW_KEY_TAB:           return KeyCode::Tab;
        case GLFW_KEY_BACKSPACE:     return KeyCode::Backspace;
        case GLFW_KEY_DELETE:        return KeyCode::Delete;
        case GLFW_KEY_LEFT:          return KeyCode::Left;
        case GLFW_KEY_RIGHT:         return KeyCode::Right;
        case GLFW_KEY_UP:            return KeyCode::Up;
        case GLFW_KEY_DOWN:          return KeyCode::Down;
        case GLFW_KEY_LEFT_SHIFT:    return KeyCode::LeftShift;
        case GLFW_KEY_RIGHT_SHIFT:   return KeyCode::RightShift;
        case GLFW_KEY_LEFT_CONTROL:  return KeyCode::LeftControl;
        case GLFW_KEY_RIGHT_CONTROL: return KeyCode::RightControl;
        case GLFW_KEY_LEFT_ALT:      return KeyCode::LeftAlt;
        case GLFW_KEY_RIGHT_ALT:     return KeyCode::RightAlt;
        case GLFW_KEY_F1: return KeyCode::F1;
        case GLFW_KEY_F2: return KeyCode::F2;
        case GLFW_KEY_F3: return KeyCode::F3;
        case GLFW_KEY_F4: return KeyCode::F4;
        case GLFW_KEY_F5: return KeyCode::F5;
        case GLFW_KEY_F6: return KeyCode::F6;
        case GLFW_KEY_F7: return KeyCode::F7;
        case GLFW_KEY_F8: return KeyCode::F8;
        case GLFW_KEY_F9: return KeyCode::F9;
        case GLFW_KEY_F10: return KeyCode::F10;
        case GLFW_KEY_F11: return KeyCode::F11;
        case GLFW_KEY_F12: return KeyCode::F12;
        default: return KeyCode::Unknown;
    }
}

MouseButton TranslateMouseButton(int glfwButton)
{
    switch (glfwButton) {
        case GLFW_MOUSE_BUTTON_LEFT:   return MouseButton::Left;
        case GLFW_MOUSE_BUTTON_RIGHT:  return MouseButton::Right;
        case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
        default: return MouseButton::Count;
    }
}

} 

bool PlatformGLFW::Init(const WindowDesc& desc, EventBus& bus)
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return false;
    }

    if (desc.api == GraphicsAPI::OpenGL) {
         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
   
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_SAMPLES, 8);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    } else if (desc.api == GraphicsAPI::Vulkan) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    window = glfwCreateWindow(desc.width, desc.height, desc.title, nullptr, nullptr);
    if (!window) {
        std::cerr << "GLFW window creation failed\n";
        glfwTerminate();
        return false;
    }

   
    glfwSetWindowUserPointer(window, &bus);

    if (desc.api == GraphicsAPI::OpenGL) {
        glfwMakeContextCurrent(window);
        glfwSwapInterval(desc.vsync ? 1 : 0);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "GLAD init failed\n";
            return false;
        }
    }

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    return true;
}

void PlatformGLFW::Shutdown()
{
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
    window = nullptr;
}

void PlatformGLFW::PollEvents()
{
    glfwPollEvents(); 
}

void PlatformGLFW::SwapBuffers()
{
    glfwSwapBuffers(window);
}

bool PlatformGLFW::ShouldClose() const
{
    return glfwWindowShouldClose(window);
}

void PlatformGLFW::RequestClose()
{
    glfwSetWindowShouldClose(window, true);
}

void* PlatformGLFW::GetNativeWindowHandle() const
{
    return window; // Vulkan: pass this to glfwCreateWindowSurface later
}

Vector2 PlatformGLFW::GetFramebufferSize() const
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    return { static_cast<float> (w),  static_cast<float> (h) }; //TO-DO
}

double PlatformGLFW::GetTime() const
{
    return glfwGetTime();
}



void PlatformGLFW::KeyCallback(GLFWwindow* w, int key, int, int action, int)
{
    if (action == GLFW_REPEAT) return; 

    auto& bus = *static_cast<EventBus*>(glfwGetWindowUserPointer(w));
    bus.publish<KeyEvent>(KeyEvent{
        TranslateKey(key),
        action == GLFW_PRESS ? InputAction::Pressed : InputAction::Released
    });
}

void PlatformGLFW::MouseButtonCallback(GLFWwindow* w, int button, int action, int)
{
    auto& bus = *static_cast<EventBus*>(glfwGetWindowUserPointer(w));
    bus.publish(MouseButtonEvent{
        TranslateMouseButton(button),
        action == GLFW_PRESS ? InputAction::Pressed : InputAction::Released
    });
}

void PlatformGLFW::CursorPosCallback(GLFWwindow* w, double x, double y)
{
    auto& bus = *static_cast<EventBus*>(glfwGetWindowUserPointer(w));
    bus.publish(MouseMoveEvent{ x, y });
}

void PlatformGLFW::ScrollCallback(GLFWwindow* w, double xoff, double yoff)
{
    auto& bus = *static_cast<EventBus*>(glfwGetWindowUserPointer(w));
    bus.publish(ScrollEvent{ xoff, yoff });
}

void PlatformGLFW::FramebufferSizeCallback(GLFWwindow*, int width, int height) {
    //auto* platform = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
   // platform->m_eventBus->publish(WindowResizeEvent{width, height});
   glViewport(0, 0, width, height);
}