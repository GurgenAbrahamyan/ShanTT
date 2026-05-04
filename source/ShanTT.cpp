#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "core/Engine.h"
#include <atomic>
#include "EnTT/entt.hpp"
#include <iostream>
int main()
{


   
        std::cout << "EnTT version: "
            << ENTT_VERSION_MAJOR << "."
            << ENTT_VERSION_MINOR << "."
            << ENTT_VERSION_PATCH
            << "\n";
    
	Engine engine;
    

	engine.run();
    return 0;
}
