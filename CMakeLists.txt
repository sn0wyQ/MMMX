cmake_minimum_required(VERSION 3.10)
project(MMMX-Client)
project(MMMX-Server)

set(CMAKE_CXX_STANDARD 20)

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
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -O3")

# Add code files
set(SOURCES
        Animation/animation.cpp
        Animation/animation.h
        Client/client_controller.cpp
        Client/client_controller.h
        Client/client_view.cpp
        Client/client_view.h
        Controller/base_controller.cpp
        Controller/base_controller.h
        Event/event.cpp
        Event/event.h
        Field/field.cpp
        Field/field.h
        GameObject/Base/base.cpp
        GameObject/Base/base.h
        GameObject/Building/Mine/mine.cpp
        GameObject/Building/Mine/mine.h
        GameObject/Building/Obstacle/obstacle.cpp
        GameObject/Building/Obstacle/obstacle.h
        GameObject/Building/Spawner/spawner.cpp
        GameObject/Building/Spawner/spawner.h
        GameObject/Building/Store/store.cpp
        GameObject/Building/Store/store.h
        GameObject/Building/Turret/turret.cpp
        GameObject/Building/Turret/turret.h
        GameObject/Building/building.cpp
        GameObject/Building/building.h
        GameObject/Loot/Bonus/bonus.cpp
        GameObject/Loot/Bonus/bonus.h
        GameObject/Loot/Item/item.cpp
        GameObject/Loot/Item/item.h
        GameObject/Loot/loot.cpp
        GameObject/Loot/loot.h
        GameObject/MovableObject/Bullet/bullet.cpp
        GameObject/MovableObject/Bullet/bullet.h
        GameObject/MovableObject/Entity/Creep/creep.cpp
        GameObject/MovableObject/Entity/Creep/creep.h
        GameObject/MovableObject/Entity/Player/active_bonus.cpp
        GameObject/MovableObject/Entity/Player/active_bonus.h
        GameObject/MovableObject/Entity/Player/inventory.cpp
        GameObject/MovableObject/Entity/Player/inventory.h
        GameObject/MovableObject/Entity/Player/passive_ability.cpp
        GameObject/MovableObject/Entity/Player/passive_ability.h
        GameObject/MovableObject/Entity/Player/player.cpp
        GameObject/MovableObject/Entity/Player/player.h
        GameObject/MovableObject/Entity/Player/skill.cpp
        GameObject/MovableObject/Entity/Player/skill.h
        GameObject/MovableObject/Entity/Player/weapon.cpp
        GameObject/MovableObject/Entity/Player/weapon.h
        GameObject/MovableObject/Entity/entity.cpp
        GameObject/MovableObject/Entity/entity.h
        GameObject/MovableObject/movable_object.cpp
        GameObject/MovableObject/movable_object.h
        GameObject/game_object.cpp
        GameObject/game_object.h
        Model/game_data_model.cpp
        Model/game_data_model.h
        Model/server_model.cpp
        Model/server_model.h
        Server/Room/room_controller.cpp
        Server/Room/room_controller.h
        Server/server_controller.cpp
        Server/server_controller.h
        constants.h
        )

# If necessary, add resources files
qt5_add_big_resources(RESOURCES_CLIENT

        )
qt5_add_big_resources(RESOURCES_SERVER

        )

add_executable(MMMX-Client client_main.cpp ${SOURCES} ${RESOURCES_CLIENT})
target_link_libraries(MMMX-Client ${REQUIRED_LIBS_QUALIFIED})

add_executable(MMMX-Server server_main.cpp ${SOURCES} ${RESOURCES_SERVER})
target_link_libraries(MMMX-Server ${REQUIRED_LIBS_QUALIFIED})
