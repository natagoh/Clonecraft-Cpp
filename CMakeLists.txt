cmake_minimum_required(VERSION 3.0)
project(Clonecraft)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Clonecraft/Vendor/glfw)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(Clonecraft/Vendor/bullet)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(Clonecraft/headers/
                    Clonecraft/src/
                    Clonecraft/resources/
                    Clonecraft/vendor/bullet/src/
                    Clonecraft/vendor/glad/include/
                    Clonecraft/vendor/glfw/include/
                    Clonecraft/vendor/glm/
                    Clonecraft/vendor/stb/)

file(GLOB VENDORS_SOURCES Clonecraft/vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS Clonecraft/headers/*.hpp)
file(GLOB PROJECT_RESOURCES Clonecraft/resources/*.png)
file(GLOB PROJECT_SOURCES Clonecraft/src/*.cpp 
                          Clonecraft/src/*.h
                          Clonecraft/src/render/*.cpp
                          Clonecraft/src/render/*.h 
                          Clonecraft/src/world/*.cpp 
                          Clonecraft/src/world/*.h)
file(GLOB PROJECT_SHADERS Clonecraft/shaders/*.comp
                          Clonecraft/shaders/*.frag
                          Clonecraft/shaders/*.geom
                          Clonecraft/shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("headers" FILES ${PROJECT_HEADERS})
source_group("resources" FILES ${PROJECT_RESOURCES})
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_RESOURCES} 
                               ${PROJECT_HEADERS} ${PROJECT_SHADERS} 
                               ${PROJECT_CONFIGS} ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Clonecraft/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>
    DEPENDS ${PROJECT_SHADERS})
