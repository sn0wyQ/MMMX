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
        Client/abstract_client_view.cpp
        Client/client_controller.cpp
        Client/client_view.cpp
        Controller/base_controller.cpp
        Event/event.cpp
        Field/field.cpp
        GameObject/Base/base.cpp
        GameObject/Building/Mine/mine.cpp
        GameObject/Building/Obstacle/obstacle.cpp
        GameObject/Building/Spawner/spawner.cpp
        GameObject/Building/Store/store.cpp
        GameObject/Building/Turret/turret.cpp
        GameObject/Building/building.cpp
        GameObject/Loot/Bonus/bonus.cpp
        GameObject/Loot/Item/item.cpp
        GameObject/Loot/loot.cpp
        GameObject/MovableObject/Bullet/bullet.cpp
        GameObject/MovableObject/Entity/Creep/creep.cpp
        GameObject/MovableObject/Entity/Player/active_bonus.cpp
        GameObject/MovableObject/Entity/Player/inventory.cpp
        GameObject/MovableObject/Entity/Player/passive_ability.cpp
        GameObject/MovableObject/Entity/Player/player.cpp
        GameObject/MovableObject/Entity/Player/skill.cpp
        GameObject/MovableObject/Entity/Player/weapon.cpp
        GameObject/MovableObject/Entity/entity.cpp
        GameObject/MovableObject/movable_object.cpp
        GameObject/game_object.cpp
        Model/game_data_model.cpp
        Model/server_model.cpp
        Server/Room/room_controller.cpp
        Server/server_controller.cpp
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
