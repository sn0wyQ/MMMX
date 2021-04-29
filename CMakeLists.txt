cmake_minimum_required(VERSION 3.10)
project(MMMX-Client)
project(MMMX-Server)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(
        "$<$<CONFIG:RELEASE>:-O3>"
        "$<$<CONFIG:DEBUG>:-O0>"
)

set(CMAKE_AUTOMOC ON) # Enable Qt MOC compiler
set(CMAKE_AUTORCC ON) # Enable Qt resources compiler
set(CMAKE_AUTOUIC ON) # Enable Qt UI compiler

set(CMAKE_PREFIX_PATH ${Qt_DIR})

# Set Qt version
set(QT_VERSION 5)
# Set necessary Qt modules
set(REQUIRED_LIBS Core Widgets Multimedia WebSockets Network)
# Set corresponding Qt libraries
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::WebSockets Qt5::Network)
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall")

# ThreadSanitizer
set(TSAN_FLAGS
        "-fsanitize=thread"
        CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
        FORCE)

# AddressSanitize
set(ASAN_FLAGS
        "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer"
        CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
        FORCE)

# LeakSanitizer
set(LSAN_FLAGS
        "-fsanitize=leak -fno-omit-frame-pointer"
        CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds."
        FORCE)

# MemorySanitizer
set(MSAN_FLAGS
        "-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer"
        CACHE STRING "Flags used by the C++ compiler during MemorySanitizer builds."
        FORCE)

# UndefinedBehaviour
set(UBSAN_FLAGS
        "-fsanitize=undefined"
        CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds."
        FORCE)

# Build Types
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ${TSAN_FLAGS}")

# Add code files
set(SOURCES
        Client/abstract_client_view.h
        Client/client_controller.cpp
        Client/client_view.cpp
        Controller/base_controller.cpp
        Converter/converter.cpp
        Event/event.cpp
        Event/event_type.h
        GUI/GameView/game_view.cpp
        GUI/GameView/player_bar.cpp
        GameObject/MovableObject/Bullet/bullet.cpp
        GameObject/MovableObject/Entity/Player/player.cpp
        GameObject/MovableObject/Entity/entity.cpp
        GameObject/MovableObject/movable_object.cpp
        GameObject/RigidBody/intersect_checker.cpp
        GameObject/RigidBody/intersect_constants.h
        GameObject/RigidBody/object_collision.cpp
        GameObject/RigidBody/rigid_body.cpp
        GameObject/RigidBody/rigid_body_circle.cpp
        GameObject/RigidBody/rigid_body_rectangle.cpp
        GameObject/map_border.cpp
        GameObject/game_object.cpp
        Hash/hash_calculator.cpp
        Interpolator/interpolator.cpp
        Math/math.cpp
        Model/client_game_model.cpp
        Model/game_model.cpp
        Model/room_game_model.cpp
        Model/server_model.cpp
        Painter/painter.cpp
        PlayerStats/player_stats.cpp
        Server/Room/room_controller.cpp
        Server/Room/room_settings.cpp
        Server/server_controller.cpp
        Weapon/AssaultRifle/assault_rifle.cpp
        Weapon/Crossbow/crossbow.cpp
        Weapon/MachineGun/machine_gun.cpp
        Weapon/Shotgun/shotgun.cpp
        Weapon/WeaponSettings/weapon_settings.cpp
        Weapon/weapon.cpp
        GameObject/MovableObject/Entity/Creep/creep.cpp
        )

# If necessary, add resources files
qt5_add_big_resources(RESOURCES_COMMON
        Res/Json/weapon_settings.qrc)

add_executable(MMMX-Client client_main.cpp ${SOURCES} ${RESOURCES_COMMON})
target_link_libraries(MMMX-Client ${REQUIRED_LIBS_QUALIFIED})

add_executable(MMMX-Server server_main.cpp ${SOURCES} ${RESOURCES_COMMON})
target_link_libraries(MMMX-Server ${REQUIRED_LIBS_QUALIFIED})
