set(DEMO_SOURCES
    main.cpp
    source/GameScene.cpp
    systems/game_logic/CameraSystem.cpp
)

set(DEMO_HEADERS
    source/GameScene.h
    systems/game_logic/CameraSystem.h
)

add_executable(
    ShanTT_Demo
    ${DEMO_SOURCES}
    ${DEMO_HEADERS}
)

target_link_libraries(
    ShanTT_Demo
    PRIVATE
        ShanTT::Engine
)