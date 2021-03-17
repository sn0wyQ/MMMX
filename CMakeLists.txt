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
        Client/abstract_client_view.h
        Client/client_controller.cpp
        Client/client_view.cpp
        Controller/base_controller.cpp
        Converter/converter.cpp
        Event/event_type.h
        Event/event.cpp
        GUI/GameView/game_view.cpp
        GameObject/MovableObject/Entity/Player/player.cpp
        GameObject/MovableObject/Entity/entity.cpp
        GameObject/MovableObject/movable_object.cpp
        GameObject/game_object.cpp
        Math/math.cpp
        Model/game_data_model.cpp
        Model/server_model.cpp
        Painter/painter.cpp
        Server/Room/room_controller.cpp
        Server/Room/room_settings.cpp
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
