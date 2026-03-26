#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../core/EventBus.h"
#include "../core/Event.h"
#include "../core/EngineContext.h"

class MouseInput {

	bool cameraMode = false;
	EventBus* eventbus;
	GLFWwindow* window = EngineContext::get().getWindow();
public:
	MouseInput(EventBus* eventbus) :
		eventbus(eventbus){}
	
	

    void proccessInput(int screenWidth, int screenHeight);
};