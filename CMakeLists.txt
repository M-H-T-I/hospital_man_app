cmake_minimum_required(VERSION 3.14) # Higher version for better FetchContent support
project(HospitalApp)

# --- SFML Dependency ---
include(FetchContent)


set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
)

# These flags are mandatory for SFML 2.x + FetchContent
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

# --- Executable Definition ---
# It is cleaner to define the app first, then add sources
add_executable(app src/main.cpp)


# --- Logic Files ---
# Ensure these paths match your folder structure (e.g., src/logic/...)
target_sources(app PRIVATE 
    src/logic/Admin.cpp
    src/logic/AdminMenu.cpp
    src/logic/Appointment.cpp
    src/logic/Bill.cpp
    src/logic/Doctor.cpp
    src/logic/DoctorMenu.cpp
    src/logic/FileHandler.cpp
    src/logic/Patient.cpp
    src/logic/PatientMenu.cpp
    src/logic/Person.cpp
    src/logic/Prescription.cpp
    src/logic/Validator.cpp

    src/logic/errors/FileNotFoundException.cpp
    src/logic/errors/HospitalException.cpp
    src/logic/errors/InsufficientFundsException.cpp
    src/logic/errors/InvalidInputException.cpp
    src/logic/errors/SlotUnavailbleException.cpp

    src/logic/utils/myUtils.cpp

    src/gui/UI_Manager.cpp
    src/gui/UI_Manager_Screens.cpp

)

# --- Include Directories ---
# This allows you to #include "Admin.hpp" instead of #include "logic/Admin.hpp"
target_include_directories(app PRIVATE 
    src/logic
    src/logic/errors
)

target_link_libraries(app PRIVATE sfml-graphics sfml-window sfml-system)



