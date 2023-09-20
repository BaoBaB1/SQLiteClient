cmake_minimum_required(VERSION 3.13.0)
# latest imgui requires at least c++17
set(CMAKE_CXX_STANDARD 17)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_NAME "SQLiteClient")
project(${PROJECT_NAME})

#glfw
set(GLFW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw)
set(GLAD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad)
add_subdirectory(${GLFW_PATH})
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# can only use PRIVATE due to: Target "glfw" INTERFACE_INCLUDE_DIRECTORIES property contains path: ./path/ which is prefixed in the source directory.
target_include_directories(glfw PRIVATE libs/glad/include)
target_sources(glfw PRIVATE ${GLAD_PATH}/src/glad.c)

# imgui
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui)
file(GLOB IMGUI_SRC
	${IMGUI_PATH}/*.h
	${IMGUI_PATH}/*.cpp
	)
add_library("imgui" STATIC ${IMGUI_SRC})
target_include_directories("imgui" PUBLIC ${IMGUI_PATH})
target_include_directories("imgui" PUBLIC ${GLFW_PATH}/include)
# --------imgui--------

#sqlite
set(SQLITE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite)
file(GLOB SQLITE_SRC
	${SQLITE_PATH}/*.h
	${SQLITE_PATH}/*.c
	)
add_library("sqlite" STATIC ${SQLITE_SRC})
target_include_directories("sqlite" PUBLIC ${SQLITE_PATH})
# --------sqlite--------

set(SUBDIRS "core;models;utils")
set(SOURCES "")
# make filters for visual studio (show sources in corresponding folder)
foreach(dir ${SUBDIRS})	
	file(GLOB SRC
		"src/${dir}/*.h"
		"src/${dir}/*.cpp"
		)
	# message("${SRC}")
	list(APPEND SOURCES ${SRC})
	source_group(${dir} FILES ${SRC})
endforeach()

add_executable(${PROJECT_NAME} "src/main.cpp" ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw imgui sqlite)
target_include_directories(${PROJECT_NAME} PRIVATE libs/glad/include)
