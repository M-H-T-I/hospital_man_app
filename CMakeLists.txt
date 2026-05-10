cmake_minimum_required(VERSION 3.14)
project(HospitalApp)

# --- SFML Dependency ---
include(FetchContent)
set(SFML_BUILD_EXAMPLES          OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TESTS             OFF CACHE BOOL "" FORCE)
set(SFML_INSTALL_PKGCONFIG_FILES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC               OFF CACHE BOOL "" FORCE)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
    GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(SFML)

# --- Executable ---
add_executable(app
    src/main.cpp
    src/gui/UI.cpp
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
)

# --- Include Directories ---
target_include_directories(app PRIVATE
    src/gui            # UI.hpp  (so main.cpp and UI.cpp can find it)
    src/logic          # Patient.hpp, Doctor.hpp, Storage.hpp, etc.
    src/logic/errors   # HospitalException.hpp, etc.
    src/logic/utils    # myUtils.hpp (if needed directly)
)

# --- SFML 2.x note ---
# UI.cpp uses SFML 2 API (sf::Font::loadFromFile, sf::Text::setFont, etc.)
# If you ever switch to SFML 3, those calls need updating.
target_link_libraries(app PRIVATE sfml-graphics sfml-window sfml-system)