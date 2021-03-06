cmake_minimum_required(VERSION 2.8)
project(quickescape)
# Use also project's custom cmake modules.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
SET(CMAKE_BUILD_TYPE Debug)

if(UNIX)
add_definitions("--std=c++11 -Wall")
endif()

include_directories(src)
# Windows needs its quirks

if(WIN32)

  set( Boost_USE_STATIC_LIBS TRUE)

  # You may need to set this if you did not install to default location
  # (C:\boost)
  # 
  #set( BOOST_ROOT "C:/Users/anssi.grohn/Downloads/boost/boost_1_55_0/")

  # You many need to set this if you use prebuilt binary AND 
  # its version does not match exactly the version of visual studio 
  # you are generating build files for.
  #
  # set( BOOST_LIBRARYDIR "C:/boost/lib64-msvc-12.0")
endif()

set( PROJECT_SRC 
  src/globals.h
  src/Game.h
  src/Game.cpp  
  src/Room.h
  src/Room.cpp 
  src/RoomFactory.h
  src/RoomFactory.cpp 
  src/GuardRoom.h
  src/GuardRoom.cpp 
  src/Stairway.h
  src/Stairway.cpp 
  src/GameObject.h
  src/GameObject.cpp
  src/Property.h
  src/PropertyHolder.h
  src/PropertyHolder.cpp
  src/Player.h
  src/Player.cpp
  src/Inventory.h
  src/Inventory.cpp
  src/main.cpp 
  src/Logger.h
  src/Logger.cpp
  src/Commands.cpp
  src/Commands.h
  src/SDLApp.h
  src/SDLApp.cpp
  src/Scene.h
  src/Scene.cpp
  src/IntroScene.h
  src/IntroScene.cpp
  src/GameScene.h
  src/GameScene.cpp
  src/Page.h
  src/Page.cpp
)

find_package(TinyXML2 REQUIRED)
find_package(Boost COMPONENTS regex REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_mixer REQUIRED)

# --------------------
# Set libraries requires for this project.
set( LIBS ${TINYXML2_LIBRARY} ${Boost_LIBRARIES} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_GFX_LIBRARY} ${SDL2_MIXER_LIBRARIES})

include_directories(${TINYXML2_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_GFX_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIRS})

# --------------------
# Resource copying target
add_custom_target(copy)
add_custom_command( TARGET copy PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res)
# --------------------------------------
# Get the dll's
file(GLOB_RECURSE DLLS *.dll)
foreach( file_i ${DLLS})
    add_custom_command(
    TARGET copy
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${file_i} ${CMAKE_CURRENT_BINARY_DIR}
)
endforeach( file_i )
# --------------------------------------
# Executable to be built
add_executable( quickescape ${PROJECT_SRC})
target_link_libraries (quickescape  ${LIBS})
# --------------------------------------
# dependencies
add_dependencies(quickescape copy)
