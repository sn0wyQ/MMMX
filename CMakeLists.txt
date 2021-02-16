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
set(REQUIRED_LIBS Core Widgets Multimedia WebSockets)
# Set corresponding Qt libraries
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::WebSockets)
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -O3")

# Add code files
set(SOURCES
        Animation/animation.cpp
        Animation/animation.h
        Controller/abstract_controller.cpp
        Controller/abstract_controller.h
        Field/field.cpp
        Field/field.h
        GameEvent/game_event.cpp
        GameEvent/game_event.h
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
        GameObject/Bullet/bullet.cpp
        GameObject/Bullet/bullet.h
        GameObject/Entity/Creep/creep.cpp
        GameObject/Entity/Creep/creep.h
        GameObject/Entity/Player/active_bonus.cpp
        GameObject/Entity/Player/active_bonus.h
        GameObject/Entity/Player/inventory.cpp
        GameObject/Entity/Player/inventory.h
        GameObject/Entity/Player/passive_ability.cpp
        GameObject/Entity/Player/passive_ability.h
        GameObject/Entity/Player/player.cpp
        GameObject/Entity/Player/player.h
        GameObject/Entity/Player/skill.cpp
        GameObject/Entity/Player/skill.h
        GameObject/Entity/Player/weapon.cpp
        GameObject/Entity/Player/weapon.h
        GameObject/Entity/entity.cpp
        GameObject/Entity/entity.h
        GameObject/Loot/Bonus/bonus.cpp
        GameObject/Loot/Bonus/bonus.h
        GameObject/Loot/Item/item.cpp
        GameObject/Loot/Item/item.h
        GameObject/Loot/loot.cpp
        GameObject/Loot/loot.h
        GameObject/game_object.cpp
        GameObject/game_object.h
        Model/game_data_model.cpp
        Model/game_data_model.h
        constants.h
        )

# If necessary, add resources files
qt5_add_big_resources(RESOURCES_CLIENT

        )
qt5_add_big_resources(RESOURCES_SERVER

        )

add_executable(MMMX-Client Client/client.cpp ${SOURCES_CLIENT} ${RESOURCES_CLIENT})
target_link_libraries(MMMX-Client ${REQUIRED_LIBS_QUALIFIED})

add_executable(MMMX-Server Server/server.cpp ${SOURCES_SERVER} ${RESOURCES_SERVER})
target_link_libraries(MMMX-Server ${REQUIRED_LIBS_QUALIFIED})
