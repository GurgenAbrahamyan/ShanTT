#pragma once
#include "MouseInput.h"


void MouseInput::proccessInput(int screenWidth, int screenHeight) {
    
        ImGuiIO& io = ImGui::GetIO();

        if (!io.WantCaptureMouse) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !cameraMode) {
                cameraMode = true; 
                CameraMode e(cameraMode);
                eventbus->publish( e);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides & grabs cursor
            }
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && cameraMode) {
                cameraMode = false; 
                CameraMode e(cameraMode);
                eventbus->publish(e);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // restores cursor
            }
        }
        if (cameraMode && !io.WantCaptureMouse) {
            double xpos, ypos;
            int xmid = float(screenWidth) / 2;
            int ymid = float(screenHeight) / 2;
            glfwGetCursorPos(window, &xpos, &ypos);

            double xoffset = xpos - xmid;
            double yoffset = ymid - ypos;

            if (xoffset != 0 || yoffset != 0) {
                MouseDragged m(xoffset, yoffset);
                eventbus->publish(m);
                glfwSetCursorPos(window, xmid, ymid);
            }
        }
    

}