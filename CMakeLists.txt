cmake_minimum_required(VERSION 3.10)
project(HospitalApp)

# --- SFML Dependency ---
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0  # Note: 3.1.0 isn't out yet, 3.0.0 is the latest major
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
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

    src/logic/UI_Manager.cpp

)

# --- Include Directories ---
# This allows you to #include "Admin.hpp" instead of #include "logic/Admin.hpp"
target_include_directories(app PRIVATE 
    src/logic
    src/logic/errors
)

# --- Linking ---
target_link_libraries(app PRIVATE SFML::Graphics SFML::Window SFML::System)