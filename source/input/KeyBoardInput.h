#pragma once

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../core/EventBus.h"
#include "../core/Event.h"
#include "../core/EngineContext.h"

class KeyboardInput {

public:

    KeyboardInput(EventBus* bus) {
        this->bus = bus;
        bus->subscribe<CameraMode>([this](CameraMode& event) {
			cameraMode = event.key;
            if (event.key) {
                std::cout << "Switched to Camera Mode\n";
            }       
            else {
                std::cout << "Switched to UI Mode\n";
            }
			});
    }


    void processInput() {

        if (!cameraMode)
            return;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            return;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            PressedKey W('W');
            bus->publish(W);
            return;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            PressedKey S('S');
            bus->publish(S);
            return;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            PressedKey A('A');
            bus->publish(A);
            return;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            PressedKey D('D');
            bus->publish(D);
            return;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            PressedKey Q('Q');
            bus->publish(Q);
            return;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            PressedKey E('E');
            bus->publish(E);
            return;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {


            Vector3 v(0, 0, 3);
            CreateObject E(v);
            bus->publish(E);

            return;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            PressedKey Q('Q');
            bus->publish(Q);
            return;
        }
    }
private:
	bool cameraMode = false;
    EventBus* bus;
    GLFWwindow* window = EngineContext::get().getWindow();
};