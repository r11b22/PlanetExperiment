cmake_minimum_required(VERSION 3.15)
project(PlanetExperiment VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(SpeltEngine
        GIT_REPOSITORY https://github.com/r11b22/SpeltEngine.git
        GIT_TAG        main
)

FetchContent_MakeAvailable(SpeltEngine)

# Force CMake to output compile commands for Zed's LSP (clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define the executable target name as 'app'
add_executable(${PROJECT_NAME}
    src/main.cpp

    src/MainScene.cpp
    include/MainScene.hpp

    src/IcoSphere.cpp
    include/IcoSphere.hpp

    src/PlanetBody.cpp
    include/PlanetBody.hpp
)

# Include the headers folder
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE SpeltEngine)
