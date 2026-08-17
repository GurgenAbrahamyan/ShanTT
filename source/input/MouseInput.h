#pragma once

#include "../core/EventBus.h"

class MouseInput {

	bool cameraMode = false;
	EventBus* eventbus;
	
public:
	MouseInput(EventBus* eventbus) :
		eventbus(eventbus){}


    void proccessInput(int screenWidth, int screenHeight);
};
