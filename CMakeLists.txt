# Copyright (c) 2026 Klumzy Duk Gamez. All rights reserved.
# This file is apart of CSVWorkLog.
# See LICENSE.md for license details.

cmake_minimum_required(VERSION 3.20)
project(CSVWorkLog)

set(CMAKE_CXX_STANDARD           23)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_C_STANDARD             23)
set(CMAKE_C_STANDARD_REQUIRED    ON)
set(CMAKE_C_EXTENSIONS          OFF)

set(CMAKE_EXPORT_COMPILE_COMMAND ON)
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}")

set(SDL_SHARED        OFF CACHE BOOL   "" FORCE)
set(SDL_STATIC        ON  CACHE BOOL   "" FORCE)
set(SDL_TESTS         OFF CACHE BOOL   "" FORCE)
set(SDL_EXAMPLES      OFF CACHE BOOL   "" FORCE)

add_subdirectory(Vendor/SDL  EXCLUDE_FROM_ALL SYSTEM)
add_subdirectory(Vendor/json EXCLUDE_FROM_ALL SYSTEM)

set(imgui_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_widgets.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/imgui_tables.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/backends/imgui_impl_sdl3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/backends/imgui_impl_sdlrenderer3.cpp"
)

file(GLOB_RECURSE ${PROJECT_NAME}_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/**/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/**/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c"
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES} ${imgui_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME ${PROJECT_NAME}
	PREFIX ""
)
target_include_directories(${PROJECT_NAME}
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/Source"
		"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui"
		"${CMAKE_CURRENT_SOURCE_DIR}/Vendor/imgui/backends"
)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
		SDL3::SDL3
		nlohmann_json::nlohmann_json
)
target_link_options(${PROJECT_NAME} 
	PRIVATE
		-static
)
if(WIN32)
	add_compile_options(${PROJECT_NAME} x86_64-pc-windows-msvc)
endif()

install(
	FILES
		$<TARGET_FILE:${PROJECT_NAME}>
	DESTINATION
		Bin
)
