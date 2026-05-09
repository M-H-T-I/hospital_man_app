cmake_minimum_required(VERSION 3.10)

project(HospitalApp)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)

FetchContent_MakeAvailable(SFML)


add_executable(app src/main.cpp)
target_link_directories(app 

    PRIVATE src/

)
target_link_libraries(app PRIVATE SFML::Graphics SFML::Window SFML::System)
