#pragma once

#include "glad/glad.h"
#include "glfw/glfw3.h"
#include "../core/EventBus.h"

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
