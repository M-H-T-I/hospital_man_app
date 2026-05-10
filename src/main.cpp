// // #include <iostream>
// // #include <SFML/Window.hpp>
// // #include <SFML/Graphics.hpp>
// // using namespace std;

// // int main(){

// //     cout << "Hello" << endl;
// //     sf::Window window(sf::VideoMode({800, 600}), "My WIndow", sf::Style::Default, sf::State::Fullscreen);

// //     while(window.isOpen()){

// //         while(const optional event = window.pollEvent()){

// //             if(event->is<sf::Event::Closed>()){
// //                 window.close();
// //             }

// //         }

// //         window.display();
// //     }

// //     return 0;
// // }

// // =============================================================================
// // main.cpp
// // MediCore Hospital Management System — Entry Point
// // Course: Object Oriented Programming (OOP) | Spring 2026 | BCS-2G
// //
// // main() is a sequence of function calls only (per spec).
// // No global variables. No goto. No std::string. No std::vector.
// // =============================================================================

// #include <iostream>
// #include <cstdio>

// #include "logic/errors/HospitalException.hpp"
// #include "logic/errors/FileNotFoundException.hpp"

// #include "logic/Validator.hpp"
// #include "logic/Storage.hpp"
// #include "logic/Patient.hpp"
// #include "logic/Doctor.hpp"
// #include "logic/Admin.hpp"
// #include "logic/Appointment.hpp"
// #include "logic/Bill.hpp"
// #include "logic/Prescription.hpp"
// #include "logic/FileHandler.hpp"
// #include "logic/PatientMenu.hpp"
// #include "logic/DoctorMenu.hpp"
// #include "logic/AdminMenu.hpp"

// // ─────────────────────────────────────────────────────────────────────────────
// // Forward declarations of all top-level functions
// // (main() is a sequence of calls to these only)
// // ─────────────────────────────────────────────────────────────────────────────

// // Ensure data directory and seed files exist on first run
// static void ensureDataDirectory();
// static void seedDataFiles();

// // Load all data from files into in-memory storage
// static void loadAllData(Storage<Patient>&      patients,
//                         Storage<Doctor>&        doctors,
//                         Admin&                  admin,
//                         Storage<Appointment>&   appointments,
//                         Storage<Bill>&           bills,
//                         Storage<Prescription>&   prescriptions);

// // Display the startup menu and return the chosen role (1-4)
// static int  showStartupMenu();

// // Login helpers — return true on success, false on lockout/failure
// static bool loginPatient(Storage<Patient>&     patients,
//                           Patient&               outPatient);
// static bool loginDoctor(Storage<Doctor>&       doctors,
//                          Doctor&                outDoctor);
// static bool loginAdmin(const Admin&            admin);

// // Top-level session runners
// static void runPatientSession(Storage<Patient>&      patients,
//                                Storage<Doctor>&        doctors,
//                                Storage<Appointment>&   appointments,
//                                Storage<Bill>&           bills,
//                                Storage<Prescription>&   prescriptions);

// static void runDoctorSession(Storage<Doctor>&        doctors,
//                               Storage<Appointment>&   appointments,
//                               Storage<Patient>&        patients,
//                               Storage<Prescription>&   prescriptions,
//                               Storage<Bill>&           bills);

// static void runAdminSession(Admin&                  admin,
//                              Storage<Patient>&        patients,
//                              Storage<Doctor>&          doctors,
//                              Storage<Appointment>&     appointments,
//                              Storage<Bill>&             bills,
//                              Storage<Prescription>&     prescriptions);

// // ─────────────────────────────────────────────────────────────────────────────
// // main — sequence of function calls only
// // ─────────────────────────────────────────────────────────────────────────────
// int main() {
//     // Prepare data directory and seed files if needed
//     ensureDataDirectory();
//     seedDataFiles();

//     // In-memory storage (no global variables — all local to main, passed by ref)
//     Storage<Patient>      patients;
//     Storage<Doctor>       doctors;
//     Admin                 admin;
//     Storage<Appointment>  appointments;
//     Storage<Bill>         bills;
//     Storage<Prescription> prescriptions;

//     // Load all data from files
//     loadAllData(patients, doctors, admin, appointments, bills, prescriptions);

//     // Main application loop
//     bool running = true;
//     while (running) {
//         int role = showStartupMenu();
//         switch (role) {
//             case 1:
//                 runPatientSession(patients, doctors, appointments, bills, prescriptions);
//                 break;
//             case 2:
//                 runDoctorSession(doctors, appointments, patients, prescriptions, bills);
//                 break;
//             case 3:
//                 runAdminSession(admin, patients, doctors, appointments, bills, prescriptions);
//                 break;
//             case 4:
//                 std::cout << "Thank you for using MediCore. Goodbye!\n";
//                 running = false;
//                 break;
//             default:
//                 std::cout << "Invalid choice. Please enter 1-4.\n";
//                 break;
//         }
//     }

//     return 0;
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // ensureDataDirectory
// // Creates the data/ directory if it does not exist.
// // ─────────────────────────────────────────────────────────────────────────────
// static void ensureDataDirectory() {
//     // Attempt to open a known file; if it fails the directory may not exist.
//     // Use system mkdir (portable enough for the submission environment).
// #if defined(_WIN32) || defined(_WIN64)
//     system("if not exist data mkdir data");
// #else
//     system("mkdir -p data");
// #endif
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // seedDataFiles
// // Writes header lines (and a default admin record) for any file that does
// // not yet exist. This ensures loadAllData never throws FileNotFoundException
// // on a fresh install while still honouring the "no hardcoded data" rule —
// // only structural headers and a single mandatory admin account are written.
// // ─────────────────────────────────────────────────────────────────────────────
// static void seedDataFiles() {
//     // Helper lambda-style struct to create a file with a header if absent
//     struct Seeder {
//         static void create(const char* path, const char* header) {
//             // Try opening for read; if it fails the file is absent
//             FILE* f = fopen(path, "r");
//             if (f) { fclose(f); return; } // already exists

//             f = fopen(path, "w");
//             if (!f) return;
//             fprintf(f, "%s\n", header);
//             fclose(f);
//         }
//     };

//     Seeder::create(FileHandler::PATIENTS_FILE,
//                    "patient_id,name,age,gender,contact,password,balance");

//     Seeder::create(FileHandler::DOCTORS_FILE,
//                    "doctor_id,name,specialization,contact,password,fee");

//     Seeder::create(FileHandler::APPOINTMENTS_FILE,
//                    "appointment_id,patient_id,doctor_id,date,time_slot,status");

//     Seeder::create(FileHandler::BILLS_FILE,
//                    "bill_id,patient_id,appointment_id,amount,status,date");

//     Seeder::create(FileHandler::PRESCRIPTIONS_FILE,
//                    "prescription_id,appointment_id,patient_id,doctor_id,date,medicines,notes");

//     Seeder::create(FileHandler::SECURITY_LOG_FILE,
//                    "timestamp,role,entered_id,result");

//     // discharged.txt shares the patients.txt header
//     Seeder::create(FileHandler::DISCHARGED_FILE,
//                    "patient_id,name,age,gender,contact,password,balance");

//     // admin.txt — seed only if absent (provides the one mandatory admin account)
//     FILE* af = fopen(FileHandler::ADMIN_FILE, "r");
//     if (!af) {
//         af = fopen(FileHandler::ADMIN_FILE, "w");
//         if (af) {
//             fprintf(af, "admin_id,name,password\n");
//             fprintf(af, "1,Admin,admin123\n");
//             fclose(af);
//         }
//     } else {
//         fclose(af);
//     }
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // loadAllData
// // Reads every .txt file into the corresponding in-memory storage.
// // Throws FileNotFoundException (caught here) if a critical file is missing.
// // ─────────────────────────────────────────────────────────────────────────────
// static void loadAllData(Storage<Patient>&      patients,
//                         Storage<Doctor>&        doctors,
//                         Admin&                  admin,
//                         Storage<Appointment>&   appointments,
//                         Storage<Bill>&           bills,
//                         Storage<Prescription>&   prescriptions) {
//     try {
//         FileHandler::loadPatients(patients);
//         FileHandler::loadDoctors(doctors);

//         if (!FileHandler::loadAdmin(admin)) {
//             throw FileNotFoundException(FileHandler::ADMIN_FILE);
//         }

//         FileHandler::loadAppointments(appointments);
//         FileHandler::loadBills(bills);
//         FileHandler::loadPrescriptions(prescriptions);

//     } catch (FileNotFoundException& e) {
//         std::cout << "STARTUP ERROR: " << e.what() << "\n";
//         std::cout << "Please ensure the data directory and required files exist.\n";
//         // Non-fatal for most files; admin file absence is critical but handled above.
//     }
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // showStartupMenu
// // Displays the welcome banner and role selection. Returns 1-4.
// // ─────────────────────────────────────────────────────────────────────────────
// static int showStartupMenu() {
//     std::cout << "\nWelcome to MediCore Hospital Management System\n";
//     std::cout << "===============================================\n";
//     std::cout << "Login as:\n";
//     std::cout << "1. Patient\n";
//     std::cout << "2. Doctor\n";
//     std::cout << "3. Admin\n";
//     std::cout << "4. Exit\n";
//     std::cout << "Enter choice: ";

//     char buf[8];
//     std::cin.getline(buf, 8);
//     Validator::trim(buf);

//     // Validate choice is 1-4
//     if (!Validator::isValidMenuChoice(buf, 1, 4)) return -1;
//     return Validator::strToInt(buf);
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // loginPatient
// // Prompts for patient ID and password; validates against loaded records.
// // Locks session after 3 consecutive failures and logs to security_log.txt.
// // Returns true on success and fills outPatient.
// // ─────────────────────────────────────────────────────────────────────────────
// static bool loginPatient(Storage<Patient>& patients, Patient& outPatient) {
//     const int MAX_ATTEMPTS = 3;
//     char idStr[16], password[51];

//     for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
//         std::cout << "Enter Patient ID: ";
//         std::cin.getline(idStr, 16);
//         Validator::trim(idStr);

//         std::cout << "Enter Password: ";
//         std::cin.getline(password, 51);
//         Validator::trim(password);

//         if (!Validator::isValidID(idStr)) {
//             std::cout << "Invalid ID format.\n";
//             FileHandler::logSecurityEvent("Patient", idStr, "FAILED");
//             continue;
//         }

//         int id = Validator::strToInt(idStr);
//         Patient* p = patients.findByID(id);

//         if (p && Validator::strEq(p->getPassword(), password)) {
//             outPatient = *p;
//             std::cout << "Login successful.\n";
//             return true;
//         }

//         std::cout << "Invalid ID or password. "
//                   << (MAX_ATTEMPTS - attempt - 1) << " attempt(s) remaining.\n";
//         FileHandler::logSecurityEvent("Patient", idStr, "FAILED");
//     }

//     std::cout << "Account locked. Contact admin.\n";
//     FileHandler::logSecurityEvent("Patient", idStr, "LOCKED");
//     return false;
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // loginDoctor
// // ─────────────────────────────────────────────────────────────────────────────
// static bool loginDoctor(Storage<Doctor>& doctors, Doctor& outDoctor) {
//     const int MAX_ATTEMPTS = 3;
//     char idStr[16], password[51];

//     for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
//         std::cout << "Enter Doctor ID: ";
//         std::cin.getline(idStr, 16);
//         Validator::trim(idStr);

//         std::cout << "Enter Password: ";
//         std::cin.getline(password, 51);
//         Validator::trim(password);

//         if (!Validator::isValidID(idStr)) {
//             std::cout << "Invalid ID format.\n";
//             FileHandler::logSecurityEvent("Doctor", idStr, "FAILED");
//             continue;
//         }

//         int id = Validator::strToInt(idStr);
//         Doctor* d = doctors.findByID(id);

//         if (d && Validator::strEq(d->getPassword(), password)) {
//             outDoctor = *d;
//             std::cout << "Login successful.\n";
//             return true;
//         }

//         std::cout << "Invalid ID or password. "
//                   << (MAX_ATTEMPTS - attempt - 1) << " attempt(s) remaining.\n";
//         FileHandler::logSecurityEvent("Doctor", idStr, "FAILED");
//     }

//     std::cout << "Account locked. Contact admin.\n";
//     FileHandler::logSecurityEvent("Doctor", idStr, "LOCKED");
//     return false;
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // loginAdmin
// // ─────────────────────────────────────────────────────────────────────────────
// static bool loginAdmin(const Admin& admin) {
//     const int MAX_ATTEMPTS = 3;
//     char idStr[16], password[51];

//     for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
//         std::cout << "Enter Admin ID: ";
//         std::cin.getline(idStr, 16);
//         Validator::trim(idStr);

//         std::cout << "Enter Password: ";
//         std::cin.getline(password, 51);
//         Validator::trim(password);

//         if (!Validator::isValidID(idStr)) {
//             std::cout << "Invalid ID format.\n";
//             FileHandler::logSecurityEvent("Admin", idStr, "FAILED");
//             continue;
//         }

//         int id = Validator::strToInt(idStr);

//         if (id == admin.getID() && Validator::strEq(admin.getPassword(), password)) {
//             std::cout << "Login successful.\n";
//             return true;
//         }

//         std::cout << "Invalid ID or password. "
//                   << (MAX_ATTEMPTS - attempt - 1) << " attempt(s) remaining.\n";
//         FileHandler::logSecurityEvent("Admin", idStr, "FAILED");
//     }

//     std::cout << "Account locked. Contact admin.\n";
//     FileHandler::logSecurityEvent("Admin", idStr, "LOCKED");
//     return false;
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // runPatientSession
// // Handles login then hands off to PatientMenu.
// // NOTE: patient object is a local copy; PatientMenu receives it by reference
// //       and updates both the in-memory copy in the storage AND the file.
// // ─────────────────────────────────────────────────────────────────────────────
// static void runPatientSession(Storage<Patient>&      patients,
//                                Storage<Doctor>&        doctors,
//                                Storage<Appointment>&   appointments,
//                                Storage<Bill>&           bills,
//                                Storage<Prescription>&   prescriptions) {
//     Patient loggedIn;
//     if (!loginPatient(patients, loggedIn)) return;

//     PatientMenu::run(loggedIn, doctors, appointments, bills, prescriptions, patients);
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // runDoctorSession
// // ─────────────────────────────────────────────────────────────────────────────
// static void runDoctorSession(Storage<Doctor>&        doctors,
//                               Storage<Appointment>&   appointments,
//                               Storage<Patient>&        patients,
//                               Storage<Prescription>&   prescriptions,
//                               Storage<Bill>&           bills) {
//     Doctor loggedIn;
//     if (!loginDoctor(doctors, loggedIn)) return;

//     DoctorMenu::run(loggedIn, appointments, patients, prescriptions, bills);
// }

// // ─────────────────────────────────────────────────────────────────────────────
// // runAdminSession
// // ─────────────────────────────────────────────────────────────────────────────
// static void runAdminSession(Admin&                  admin,
//                              Storage<Patient>&        patients,
//                              Storage<Doctor>&          doctors,
//                              Storage<Appointment>&     appointments,
//                              Storage<Bill>&             bills,
//                              Storage<Prescription>&     prescriptions) {
//     if (!loginAdmin(admin)) return;

//     AdminMenu::run(admin, patients, doctors, appointments, bills, prescriptions);
// }

// =============================================================================
// main.cpp  —  MediCore Hospital Management System
// SFML-based GUI front-end that wraps all existing logic classes.
//
// Compile (Linux/macOS):
//   g++ -std=c++17 *.cpp -o medicore \
//       -lsfml-graphics -lsfml-window -lsfml-system
//
// Compile (Windows, MinGW):
//   g++ -std=c++17 *.cpp -o medicore.exe \
//       -lsfml-graphics -lsfml-window -lsfml-system
//
// A font file "assets/Roboto-Regular.ttf" (or any TTF) must exist beside
// the executable.  The fallback path list is tried in order so any common
// system font will work if the assets folder is absent.
// =============================================================================

#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
#include "FileHandler.hpp"
#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"
#include "PatientMenu.hpp"
#include "DoctorMenu.hpp"
#include "AdminMenu.hpp"
#include "Validator.hpp"

// Exception headers (declared in your project)
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

// If they live in a single header adjust the include:
#include "HospitalException.hpp"

#include <cstring>
#include <cstdio>
#include <ctime>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
// Colour palette  (dark hospital theme)
// ─────────────────────────────────────────────────────────────────────────────
namespace Colors
{
    const sf::Color BG{12, 18, 30};          // deep navy
    const sf::Color PANEL{20, 30, 50};       // card background
    const sf::Color ACCENT{0, 180, 216};     // cyan accent
    const sf::Color ACCENT2{72, 149, 239};   // blue highlight
    const sf::Color TEXT{220, 230, 245};     // near-white text
    const sf::Color TEXT_DIM{120, 140, 170}; // muted label
    const sf::Color SUCCESS{56, 183, 100};   // green
    const sf::Color ERROR{230, 70, 70};      // red
    const sf::Color WARNING{255, 185, 0};    // amber
    const sf::Color BORDER{40, 60, 90};      // panel border
    const sf::Color BTN_HOVER{0, 150, 190};  // button hover
    const sf::Color INPUT_BG{28, 40, 62};    // text-field bg
    const sf::Color INPUT_ACT{35, 50, 80};   // active input
}

// ─────────────────────────────────────────────────────────────────────────────
// Window constants
// ─────────────────────────────────────────────────────────────────────────────
static constexpr unsigned WIN_W = 1100;
static constexpr unsigned WIN_H = 700;
static constexpr unsigned FPS = 60;

// ─────────────────────────────────────────────────────────────────────────────
// Tiny string helpers (no std::string, no strcmp)
// ─────────────────────────────────────────────────────────────────────────────
static void clearBuf(char *buf, int sz)
{
    for (int i = 0; i < sz; i++)
        buf[i] = '\0';
}

// ─────────────────────────────────────────────────────────────────────────────
// Load font – tries several common paths
// ─────────────────────────────────────────────────────────────────────────────
static bool loadFont(sf::Font &font)
{
    const char *paths[] = {
        "assets/Jaro-Regular.ttf",
        "assets/OpenSans-Regular.ttf",
        "./assets/font.ttf",
        nullptr};
    for (int i = 0; paths[i]; ++i)
        if (font.loadFromFile(paths[i]))
        {
            cout << "done" << endl;
            ;
            return true;
        }
        return false;
    }
    // =============================================================================
    // UI Component helpers
    // =============================================================================

    // Draw a rounded rectangle (SFML RectangleShape approximation)
    static void drawPanel(sf::RenderWindow & win, float x, float y, float w, float h,
                          sf::Color fill, sf::Color border = Colors::BORDER,
                          float borderThick = 1.f)
    {
        sf::RectangleShape rect({w, h});
        rect.setPosition({x, y});
        rect.setFillColor(fill);
        rect.setOutlineColor(border);
        rect.setOutlineThickness(borderThick);
        win.draw(rect);
    }

    // Draw centered text inside a rectangle
    static void drawText(sf::RenderWindow & win, const sf::Font &font,
                         const char *str, unsigned size,
                         float x, float y,
                         sf::Color color = Colors::TEXT,
                         bool centerX = false, float boxW = 0)
    {
        sf::Text t;
        t.setFont(font);
        t.setString(str);
        t.setCharacterSize(size);
        t.setFillColor(color);
        if (centerX && boxW > 0)
        {
            float tw = t.getLocalBounds().width;
            x = x + (boxW - tw) / 2.f;
        }
        t.setPosition(x, y);
        win.draw(t);
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // Simple Button
    // ─────────────────────────────────────────────────────────────────────────────
    struct Button
    {
        sf::RectangleShape shape;
        sf::Text label;
        bool hovered = false;
        bool active = false; // toggle / selected state

        void init(const sf::Font &f, const char *text,
                  float x, float y, float w, float h,
                  unsigned fontSize = 16)
        {
            shape.setSize({w, h});
            shape.setPosition(x, y);
            shape.setFillColor(Colors::PANEL);
            shape.setOutlineColor(Colors::BORDER);
            shape.setOutlineThickness(1.f);

            label.setFont(f);
            label.setString(text);
            label.setCharacterSize(fontSize);
            label.setFillColor(Colors::TEXT);
            // center label
            float lw = label.getLocalBounds().width;
            float lh = label.getLocalBounds().height;
            label.setPosition({x + (w - lw) / 2.f, y + (h - lh) / 2.f - 2.f});
        }

        void update(sf::Vector2i mouse)
        {
            hovered = shape.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y});
            if (active)
                shape.setFillColor(Colors::ACCENT);
            else if (hovered)
                shape.setFillColor(Colors::BTN_HOVER);
            else
                shape.setFillColor(Colors::PANEL);
        }

        bool clicked(sf::Event &e)
        {
            return e.type == sf::Event::MouseButtonPressed &&
                   e.mouseButton.button == sf::Mouse::Left &&
                   shape.getGlobalBounds().contains((float)e.mouseButton.x,
                                                    (float)e.mouseButton.y);
        }

        void draw(sf::RenderWindow &win)
        {
            win.draw(shape);
            win.draw(label);
        }
    };

    // ─────────────────────────────────────────────────────────────────────────────
    // Simple TextField
    // ─────────────────────────────────────────────────────────────────────────────
    struct TextField
    {
        sf::RectangleShape box;
        sf::Text display;
        char buf[256];
        int len = 0;
        bool focused = false;
        bool password = false; // mask with *

        void init(const sf::Font &f, float x, float y, float w, float h,
                  unsigned fontSize = 16)
        {
            clearBuf(buf, 256);
            box.setSize({w, h});
            box.setPosition(x, y);
            box.setFillColor(Colors::INPUT_BG);
            box.setOutlineColor(Colors::BORDER);
            box.setOutlineThickness(1.f);

            display.setFont(f);
            display.setCharacterSize(fontSize);
            display.setFillColor(Colors::TEXT);
            display.setPosition(x + 8.f, y + (h - fontSize) / 2.f - 2.f);
        }

        void handleEvent(sf::Event &e)
        {
            if (!focused)
                return;
            if (e.type == sf::Event::TextEntered)
            {
                sf::Uint32 c = e.text.unicode;
                if (c == 8 && len > 0)
                {
                    buf[--len] = '\0';
                } // backspace
                else if (c >= 32 && c < 127 && len < 254)
                {
                    buf[len++] = (char)c;
                    buf[len] = '\0';
                }
            }
        }

        void handleClick(sf::Event &e)
        {
            if (e.type == sf::Event::MouseButtonPressed)
                focused = box.getGlobalBounds().contains((float)e.mouseButton.x,
                                                         (float)e.mouseButton.y);
        }

        void clear()
        {
            clearBuf(buf, 256);
            len = 0;
        }

        void draw(sf::RenderWindow &win)
        {
            box.setFillColor(focused ? Colors::INPUT_ACT : Colors::INPUT_BG);
            box.setOutlineColor(focused ? Colors::ACCENT : Colors::BORDER);
            win.draw(box);

            // build display string
            static char tmp[258];
            if (password)
            {
                for (int i = 0; i < len; i++)
                    tmp[i] = '*';
                tmp[len] = '\0';
            }
            else
            {
                Validator::textCpy(tmp, buf, 257);
            }
            // append cursor
            if (focused)
            {
                tmp[len] = '|';
                tmp[len + 1] = '\0';
            }
            display.setString(tmp);
            win.draw(display);
        }
    };

    // =============================================================================
    // Screen IDs
    // =============================================================================
    enum class Screen
    {
        SPLASH,
        ROLE_SELECT,
        LOGIN,
        PATIENT_MENU,
        DOCTOR_MENU,
        ADMIN_MENU,
        // Sub-screens rendered in the same window via overlays:
        CONSOLE_OUTPUT // shows scrollable text output from logic functions
    };

    // =============================================================================
    // ConsoleCapture – redirects stdout to a char buffer so SFML can display it
    // =============================================================================
    struct ConsoleCapture
    {
        static constexpr int CAP_SIZE = 32768;
        char buf[CAP_SIZE];
        int len = 0;
        FILE *oldOut = nullptr;
        FILE *tmpF = nullptr;

        // We use a temporary file to capture printf output
        void begin()
        {
            clearBuf(buf, CAP_SIZE);
            len = 0;
            fflush(stdout);
            tmpF = tmpfile();
            oldOut = stdout;
#ifdef _WIN32
            freopen_s(reinterpret_cast<FILE **>(&stdout), "NUL", "w", stdout);
#else
            // Redirect stdout to tmpF
            // We use dup2 approach
            // (tmpfile fd trick): just capture with snprintf in wrapper calls
            // For simplicity, we read back from tmpF after each logic call.
            // On POSIX we use dup2:
            fflush(stdout);
            int fd = fileno(tmpF);
            dup2(fd, STDOUT_FILENO);
#endif
        }

        void end()
        {
#ifdef _WIN32
            // restore
            freopen_s(reinterpret_cast<FILE **>(&stdout), "CON", "w", stdout);
#else
            fflush(stdout);
            // restore stdout to terminal
            int tty = open("/dev/tty", O_WRONLY);
            if (tty >= 0)
            {
                dup2(tty, STDOUT_FILENO);
                close(tty);
            }
            else
            { /* fallback: reopen /proc/self/fd/1 */
            }
            // read from tmpF
            rewind(tmpF);
            int n = (int)fread(buf, 1, CAP_SIZE - 1, tmpF);
            if (n < 0)
                n = 0;
            buf[n] = '\0';
            len = n;
#endif
            if (tmpF)
            {
                fclose(tmpF);
                tmpF = nullptr;
            }
        }
    };

    // =============================================================================
    // Application state
    // =============================================================================
    struct AppState
    {
        // ── Storages (loaded once on startup) ───────────────────────────────────
        Storage<Patient> patients;
        Storage<Doctor> doctors;
        Storage<Appointment> appointments;
        Storage<Bill> bills;
        Storage<Prescription> prescriptions;
        Admin admin;
        bool adminLoaded = false;

        // ── Session ──────────────────────────────────────────────────────────────
        Screen currentScreen = Screen::SPLASH;
        int selectedRole = 0; // 1=Patient 2=Doctor 3=Admin
        int loginAttempts = 0;

        Patient *loggedPatient = nullptr;
        Doctor *loggedDoctor = nullptr;
        bool adminLoggedIn = false;

        // ── UI state ─────────────────────────────────────────────────────────────
        char statusMsg[256];
        bool statusIsError = false;
        float statusTimer = 0.f; // seconds remaining to show status

        // ── Console output overlay ───────────────────────────────────────────────
        char consoleText[32768];
        int consoleScroll = 0; // line offset for scrolling
        Screen consoleReturn = Screen::PATIENT_MENU;

        void setStatus(const char *msg, bool err = false)
        {
            Validator::textCpy(statusMsg, msg, 256);
            statusIsError = err;
            statusTimer = 3.5f;
        }

        void loadAll()
        {
            try
            {
                FileHandler::loadPatients(patients);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadDoctors(doctors);
            }
            catch (...)
            {
            }
            try
            {
                adminLoaded = FileHandler::loadAdmin(admin);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadAppointments(appointments);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadBills(bills);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadPrescriptions(prescriptions);
            }
            catch (...)
            {
            }
        }
    };

    // ─────────────────────────────────────────────────────────────────────────────
    // Forward declarations
    // ─────────────────────────────────────────────────────────────────────────────
    static void drawSplash(sf::RenderWindow &, sf::Font &, float dt, AppState &);
    static void drawRoleSelect(sf::RenderWindow &, sf::Font &, sf::Event &, AppState &);
    static void drawLogin(sf::RenderWindow &, sf::Font &, sf::Event &, AppState &);
    static void drawPatientMenu(sf::RenderWindow &, sf::Font &, sf::Event &, AppState &);
    static void drawDoctorMenu(sf::RenderWindow &, sf::Font &, sf::Event &, AppState &);
    static void drawAdminMenu(sf::RenderWindow &, sf::Font &, sf::Event &, AppState &);
    static void drawConsoleOut(sf::RenderWindow &, sf::Font &, sf::Event &, AppState &);
    static void drawStatusBar(sf::RenderWindow &, sf::Font &, AppState &);
    static void drawTopBar(sf::RenderWindow &, sf::Font &, AppState &);

    // ─────────────────────────────────────────────────────────────────────────────
    // Splash screen (animated logo)
    // ─────────────────────────────────────────────────────────────────────────────
    static float splashTime = 0.f;

    static void drawSplash(sf::RenderWindow & win, sf::Font & font, float dt, AppState &app)
    {
        splashTime += dt;

        // background gradient simulation via layered rectangles
        for (int i = 0; i < 8; i++)
        {
            sf::RectangleShape s({(float)WIN_W, (float)(WIN_H / 8)});
            s.setPosition(0, i * WIN_H / 8.f);
            int base = 12 + i * 3;
            s.setFillColor({(sf::Uint8)base, (sf::Uint8)(base + 6), (sf::Uint8)(base + 18)});
            win.draw(s);
        }

        // pulsing cross icon
        float pulse = 0.85f + 0.15f * std::sin(splashTime * 2.f);
        float cx = WIN_W / 2.f, cy = WIN_H / 2.f - 60.f;
        float crossSz = 60.f * pulse;
        float thick = 18.f * pulse;

        sf::RectangleShape hbar({crossSz * 2.f, thick});
        hbar.setOrigin(crossSz, thick / 2.f);
        hbar.setPosition(cx, cy);
        hbar.setFillColor(Colors::ACCENT);
        win.draw(hbar);

        sf::RectangleShape vbar({thick, crossSz * 2.f});
        vbar.setOrigin(thick / 2.f, crossSz);
        vbar.setPosition(cx, cy);
        vbar.setFillColor(Colors::ACCENT);
        win.draw(vbar);

        // Title
        drawText(win, font, "MediCore", 52, 0, cy + 80.f,
                 Colors::ACCENT, true, (float)WIN_W);
        drawText(win, font, "Hospital Management System", 22, 0, cy + 146.f,
                 Colors::TEXT_DIM, true, (float)WIN_W);

        // fade-in "Press any key"
        if (splashTime > 1.5f)
        {
            float alpha = std::min(1.f, (splashTime - 1.5f) / 0.6f);
            sf::Color c = Colors::TEXT_DIM;
            c.a = (sf::Uint8)(alpha * 200.f);
            sf::Text t;
            t.setFont(font);
            t.setString("Press any key to continue");
            t.setCharacterSize(16);
            t.setFillColor(c);
            float tw = t.getLocalBounds().width;
            t.setPosition((WIN_W - tw) / 2.f, cy + 220.f);
            win.draw(t);
        }

        if (splashTime > 2.f)
        {
            // advance on any key or mouse click
            // (handled in main event loop via Screen::SPLASH check)
        }
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // Role selection
    // ─────────────────────────────────────────────────────────────────────────────
    static Button roleBtns[4];
    static bool roleBtnsInit = false;

    static void drawRoleSelect(sf::RenderWindow & win, sf::Font & font,
                               sf::Event & ev, AppState & app)
    {
        // Background
        drawPanel(win, 0, 0, WIN_W, WIN_H, Colors::BG);

        // Left decorative strip
        drawPanel(win, 0, 0, 6, WIN_H, Colors::ACCENT);

        // Card
        float cardW = 420.f, cardH = 460.f;
        float cardX = (WIN_W - cardW) / 2.f, cardY = (WIN_H - cardH) / 2.f;
        drawPanel(win, cardX, cardY, cardW, cardH, Colors::PANEL, Colors::BORDER, 1.5f);

        // Header bar inside card
        drawPanel(win, cardX, cardY, cardW, 64.f, Colors::ACCENT);
        drawText(win, font, "MediCore", 26, cardX, cardY + 14.f,
                 Colors::BG, true, cardW);

        drawText(win, font, "Select your role to continue", 15,
                 cardX, cardY + 82.f, Colors::TEXT_DIM, true, cardW);

        // Role buttons
        const char *labels[4] = {"  Patient", "  Doctor", "  Admin", "  Exit"};
        const char *icons[4] = {"[P]", "[D]", "[A]", "[X]"};
        sf::Color btnColors[4] = {
            Colors::ACCENT2, Colors::SUCCESS, Colors::WARNING, Colors::ERROR};

        if (!roleBtnsInit)
        {
            for (int i = 0; i < 4; i++)
            {
                roleBtns[i].init(font, labels[i],
                                 cardX + 40.f, cardY + 118.f + i * 76.f,
                                 cardW - 80.f, 56.f, 17);
            }
            roleBtnsInit = true;
        }

        sf::Vector2i mouse = sf::Mouse::getPosition(win);
        for (int i = 0; i < 4; i++)
        {
            roleBtns[i].update(mouse);
            // colour tint on hover
            if (roleBtns[i].hovered)
                roleBtns[i].shape.setOutlineColor(btnColors[i]);
            else
                roleBtns[i].shape.setOutlineColor(Colors::BORDER);
            roleBtns[i].draw(win);
            // draw icon
            drawText(win, font, icons[i], 15,
                     cardX + 48.f,
                     cardY + 118.f + i * 76.f + 18.f,
                     btnColors[i]);

            if (roleBtns[i].clicked(ev))
            {
                if (i == 3)
                {
                    win.close();
                    return;
                }
                app.selectedRole = i + 1;
                app.loginAttempts = 0;
                app.currentScreen = Screen::LOGIN;
                roleBtnsInit = false;
            }
        }

        drawStatusBar(win, font, app);
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // Login screen
    // ─────────────────────────────────────────────────────────────────────────────
    static TextField loginID, loginPW;
    static Button loginBtn, backBtn;
    static bool loginInit = false;

    static void drawLogin(sf::RenderWindow & win, sf::Font & font,
                          sf::Event & ev, AppState & app)
    {
        drawPanel(win, 0, 0, WIN_W, WIN_H, Colors::BG);
        drawPanel(win, 0, 0, 6, WIN_H, Colors::ACCENT);

        float cardW = 420.f, cardH = 380.f;
        float cardX = (WIN_W - cardW) / 2.f, cardY = (WIN_H - cardH) / 2.f;
        drawPanel(win, cardX, cardY, cardW, cardH, Colors::PANEL, Colors::BORDER, 1.5f);
        drawPanel(win, cardX, cardY, cardW, 60.f, Colors::ACCENT);

        const char *roleNames[4] = {"", "Patient Login", "Doctor Login", "Admin Login"};
        drawText(win, font, roleNames[app.selectedRole], 22, cardX, cardY + 16.f,
                 Colors::BG, true, cardW);

        if (!loginInit)
        {
            loginID.init(font, cardX + 40.f, cardY + 90.f, cardW - 80.f, 42.f);
            loginPW.init(font, cardX + 40.f, cardY + 170.f, cardW - 80.f, 42.f);
            loginPW.password = true;
            loginBtn.init(font, "Login", cardX + 40.f, cardY + 240.f, cardW - 80.f, 46.f, 17);
            backBtn.init(font, "Back", cardX + 40.f, cardY + 300.f, cardW - 80.f, 40.f, 15);
            loginInit = true;
        }

        drawText(win, font, "User ID", 13, cardX + 40.f, cardY + 72.f, Colors::TEXT_DIM);
        loginID.handleClick(ev);
        loginID.handleEvent(ev);
        loginID.draw(win);

        drawText(win, font, "Password", 13, cardX + 40.f, cardY + 152.f, Colors::TEXT_DIM);
        loginPW.handleClick(ev);
        loginPW.handleEvent(ev);
        loginPW.draw(win);

        sf::Vector2i mouse = sf::Mouse::getPosition(win);
        loginBtn.update(mouse);
        loginBtn.draw(win);
        backBtn.update(mouse);
        backBtn.draw(win);

        // ── Handle Login ─────────────────────────────────────────────────────────
        bool doLogin = loginBtn.clicked(ev) ||
                       (ev.type == sf::Event::KeyPressed &&
                        ev.key.code == sf::Keyboard::Return);

        if (doLogin)
        {
            if (!Validator::isValidID(loginID.buf))
            {
                app.setStatus("Please enter a valid numeric ID.", true);
            }
            else
            {
                int enteredID = Validator::strToInt(loginID.buf);
                bool found = false;

                if (app.selectedRole == 1)
                {
                    // Patient
                    Patient *p = app.patients.findByID(enteredID);
                    if (p && Validator::strEq(p->getPassword(), loginPW.buf))
                    {
                        app.loggedPatient = p;
                        app.currentScreen = Screen::PATIENT_MENU;
                        app.setStatus("Welcome!");
                        found = true;
                        loginInit = false;
                        loginID.clear();
                        loginPW.clear();
                    }
                }
                else if (app.selectedRole == 2)
                {
                    // Doctor
                    Doctor *d = app.doctors.findByID(enteredID);
                    if (d && Validator::strEq(d->getPassword(), loginPW.buf))
                    {
                        app.loggedDoctor = d;
                        app.currentScreen = Screen::DOCTOR_MENU;
                        app.setStatus("Welcome!");
                        found = true;
                        loginInit = false;
                        loginID.clear();
                        loginPW.clear();
                    }
                }
                else if (app.selectedRole == 3)
                {
                    // Admin
                    if (app.adminLoaded &&
                        app.admin.getID() == enteredID &&
                        Validator::strEq(app.admin.getPassword(), loginPW.buf))
                    {
                        app.adminLoggedIn = true;
                        app.currentScreen = Screen::ADMIN_MENU;
                        app.setStatus("Welcome, Admin!");
                        found = true;
                        loginInit = false;
                        loginID.clear();
                        loginPW.clear();
                    }
                }

                if (!found)
                {
                    app.loginAttempts++;
                    // Log failed attempt
                    FileHandler::logSecurityEvent(
                        app.selectedRole == 1 ? "Patient" : app.selectedRole == 2 ? "Doctor"
                                                                                  : "Admin",
                        loginID.buf, "FAILED");

                    if (app.loginAttempts >= 3)
                    {
                        app.setStatus("Account locked. Contact admin.", true);
                        app.currentScreen = Screen::ROLE_SELECT;
                        loginInit = false;
                        loginID.clear();
                        loginPW.clear();
                        app.loginAttempts = 0;
                    }
                    else
                    {
                        char msg[80];
                        msg[0] = '\0';
                        const char *part1 = "Invalid credentials. Attempts: ";
                        const char *part2 = "/3";
                        char numBuf[8];
                        Validator::intToStr(app.loginAttempts, numBuf, 8);
                        Validator::textCpy(msg, part1, 80);
                        Validator::strCat(msg, numBuf, 80);
                        Validator::strCat(msg, part2, 80);
                        app.setStatus(msg, true);
                        loginPW.clear();
                    }
                }
            }
        }

        if (backBtn.clicked(ev))
        {
            app.currentScreen = Screen::ROLE_SELECT;
            loginInit = false;
            loginID.clear();
            loginPW.clear();
        }

        drawStatusBar(win, font, app);
    }

    // =============================================================================
    // Generic menu helper – draws a vertical list of action buttons
    //   returns index of clicked button (-1 if none)
    // =============================================================================
    struct MenuPanel
    {
        static constexpr int MAX_ITEMS = 15;
        Button btns[MAX_ITEMS];
        int count = 0;
        bool inited = false;

        void init(const sf::Font &f, const char **items, int n,
                  float x, float y, float w = 320.f, float h = 46.f, float gap = 8.f)
        {
            count = n < MAX_ITEMS ? n : MAX_ITEMS;
            for (int i = 0; i < count; i++)
                btns[i].init(f, items[i], x, y + i * (h + gap), w, h, 15);
            inited = true;
        }

        int update(sf::RenderWindow &win, sf::Event &ev)
        {
            sf::Vector2i m = sf::Mouse::getPosition(win);
            for (int i = 0; i < count; i++)
            {
                btns[i].update(m);
                btns[i].draw(win);
                if (btns[i].clicked(ev))
                    return i;
            }
            return -1;
        }

        void reset() { inited = false; }
    };

    // ─────────────────────────────────────────────────────────────────────────────
    // Patient Menu
    // ─────────────────────────────────────────────────────────────────────────────
    static MenuPanel patientPanel;

    static void drawPatientMenu(sf::RenderWindow & win, sf::Font & font,
                                sf::Event & ev, AppState & app)
    {
        drawPanel(win, 0, 0, WIN_W, WIN_H, Colors::BG);
        drawTopBar(win, font, app);

        // Patient info card (right side)
        float infoX = WIN_W - 340.f, infoY = 80.f;
        drawPanel(win, infoX, infoY, 310.f, 200.f, Colors::PANEL, Colors::BORDER);
        drawPanel(win, infoX, infoY, 310.f, 38.f, Colors::ACCENT2);
        drawText(win, font, "Patient Profile", 14, infoX, infoY + 10.f, Colors::BG, true, 310.f);

        if (app.loggedPatient)
        {
            char tmp[64];
            drawText(win, font, app.loggedPatient->getName(), 17,
                     infoX + 14.f, infoY + 48.f, Colors::TEXT);

            // Balance
            Validator::floatToStr(app.loggedPatient->getBalance(), tmp, 64, 2);
            char balLine[80];
            Validator::textCpy(balLine, "Balance: PKR ", 80);
            Validator::strCat(balLine, tmp, 80);
            drawText(win, font, balLine, 14, infoX + 14.f, infoY + 80.f, Colors::SUCCESS);

            char ageGend[40];
            Validator::intToStr(app.loggedPatient->getAge(), tmp, 64);
            Validator::textCpy(ageGend, "Age: ", 40);
            Validator::strCat(ageGend, tmp, 40);
            Validator::strCat(ageGend, "  Gender: ", 40);
            Validator::strCat(ageGend, app.loggedPatient->getGender(), 40);
            drawText(win, font, ageGend, 13, infoX + 14.f, infoY + 108.f, Colors::TEXT_DIM);

            drawText(win, font, app.loggedPatient->getContact(), 13,
                     infoX + 14.f, infoY + 134.f, Colors::TEXT_DIM);
        }

        // Menu title
        drawText(win, font, "Patient Menu", 24, 50.f, 88.f, Colors::ACCENT);

        const char *items[] = {
            "1. Book Appointment",
            "2. Cancel Appointment",
            "3. View My Appointments",
            "4. View My Medical Records",
            "5. View My Bills",
            "6. Pay Bill",
            "7. Top Up Balance",
            "8. Logout"};

        if (!patientPanel.inited)
            patientPanel.init(font, items, 8, 50.f, 130.f, 340.f, 46.f, 7.f);

        int clicked = patientPanel.update(win, ev);
        if (clicked < 0)
        {
            drawStatusBar(win, font, app);
            return;
        }

        if (clicked == 7)
        { // Logout
            app.loggedPatient = nullptr;
            app.currentScreen = Screen::ROLE_SELECT;
            patientPanel.reset();
            app.setStatus("Logged out successfully.");
            return;
        }

        // ── Run the corresponding PatientMenu logic in console mode ─────────────
        // We hand off to the existing console-based logic.
        // The SFML window minimizes; the console takes over; then we restore.
        //
        // NOTE: For a fully-embedded approach you would redirect stdout and parse
        //       the output back into the SFML overlay.  Here we use the portable
        //       approach: briefly show the console (SFML window is still open but
        //       not updated) by calling the function directly.  On most systems
        //       the terminal window is visible behind the SFML window.
        //
        // A cleaner approach on Windows is to call AllocConsole() before this.

        app.setStatus("Running console operation… check terminal.", false);

        // Re-run everything through PatientMenu:
        PatientMenu::run(*app.loggedPatient,
                         app.doctors,
                         app.appointments,
                         app.bills,
                         app.prescriptions,
                         app.patients);

        // After console interaction, reload data to reflect changes
        app.loadAll();
        // Re-find the logged-in patient (pointer may be stale after reload)
        app.loggedPatient = app.patients.findByID(app.loggedPatient
                                                      ? app.loggedPatient->getID()
                                                      : -1);
        if (!app.loggedPatient)
            app.currentScreen = Screen::ROLE_SELECT;

        patientPanel.reset();
        drawStatusBar(win, font, app);
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // Doctor Menu
    // ─────────────────────────────────────────────────────────────────────────────
    static MenuPanel doctorPanel;

    static void drawDoctorMenu(sf::RenderWindow & win, sf::Font & font,
                               sf::Event & ev, AppState & app)
    {
        drawPanel(win, 0, 0, WIN_W, WIN_H, Colors::BG);
        drawTopBar(win, font, app);

        // Doctor info card
        float infoX = WIN_W - 340.f, infoY = 80.f;
        drawPanel(win, infoX, infoY, 310.f, 180.f, Colors::PANEL, Colors::BORDER);
        drawPanel(win, infoX, infoY, 310.f, 38.f, Colors::SUCCESS);
        drawText(win, font, "Doctor Profile", 14, infoX, infoY + 10.f, Colors::BG, true, 310.f);

        if (app.loggedDoctor)
        {
            char tmp[64];
            drawText(win, font, app.loggedDoctor->getName(), 17,
                     infoX + 14.f, infoY + 48.f, Colors::TEXT);
            drawText(win, font, app.loggedDoctor->getSpecialization(), 13,
                     infoX + 14.f, infoY + 76.f, Colors::TEXT_DIM);

            Validator::floatToStr(app.loggedDoctor->getFee(), tmp, 64, 2);
            char feeLine[80];
            Validator::textCpy(feeLine, "Fee: PKR ", 80);
            Validator::strCat(feeLine, tmp, 80);
            drawText(win, font, feeLine, 13, infoX + 14.f, infoY + 104.f, Colors::SUCCESS);
        }

        drawText(win, font, "Doctor Menu", 24, 50.f, 88.f, Colors::SUCCESS);

        const char *items[] = {
            "1. View Today's Appointments",
            "2. Mark Appointment Complete",
            "3. Mark Appointment No-Show",
            "4. Write Prescription",
            "5. View Patient Medical History",
            "6. Logout"};

        if (!doctorPanel.inited)
            doctorPanel.init(font, items, 6, 50.f, 130.f, 340.f, 46.f, 7.f);

        int clicked = doctorPanel.update(win, ev);
        if (clicked < 0)
        {
            drawStatusBar(win, font, app);
            return;
        }

        if (clicked == 5)
        {
            app.loggedDoctor = nullptr;
            app.currentScreen = Screen::ROLE_SELECT;
            doctorPanel.reset();
            app.setStatus("Logged out successfully.");
            return;
        }

        app.setStatus("Running console operation… check terminal.", false);
        DoctorMenu::run(*app.loggedDoctor,
                        app.appointments,
                        app.patients,
                        app.prescriptions,
                        app.bills);

        app.loadAll();
        app.loggedDoctor = app.doctors.findByID(app.loggedDoctor
                                                    ? app.loggedDoctor->getID()
                                                    : -1);
        if (!app.loggedDoctor)
            app.currentScreen = Screen::ROLE_SELECT;
        doctorPanel.reset();
        drawStatusBar(win, font, app);
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // Admin Menu
    // ─────────────────────────────────────────────────────────────────────────────
    static MenuPanel adminPanel;

    static void drawAdminMenu(sf::RenderWindow & win, sf::Font & font,
                              sf::Event & ev, AppState & app)
    {
        drawPanel(win, 0, 0, WIN_W, WIN_H, Colors::BG);
        drawTopBar(win, font, app);

        // Summary stats bar
        float statsY = 80.f;
        drawPanel(win, 20.f, statsY, WIN_W - 40.f, 72.f, Colors::PANEL, Colors::BORDER);

        // Patient count
        char tmp[32];
        Validator::intToStr(app.patients.size(), tmp, 32);
        char line[64];
        Validator::textCpy(line, "Patients: ", 64);
        Validator::strCat(line, tmp, 64);
        drawText(win, font, line, 15, 40.f, statsY + 10.f, Colors::ACCENT);

        Validator::intToStr(app.doctors.size(), tmp, 32);
        Validator::textCpy(line, "Doctors: ", 64);
        Validator::strCat(line, tmp, 64);
        drawText(win, font, line, 15, 220.f, statsY + 10.f, Colors::SUCCESS);

        Validator::intToStr(app.appointments.size(), tmp, 32);
        Validator::textCpy(line, "Appointments: ", 64);
        Validator::strCat(line, tmp, 64);
        drawText(win, font, line, 15, 400.f, statsY + 10.f, Colors::WARNING);

        Validator::intToStr(app.bills.size(), tmp, 32);
        Validator::textCpy(line, "Bills: ", 64);
        Validator::strCat(line, tmp, 64);
        drawText(win, font, line, 15, 600.f, statsY + 10.f, Colors::TEXT_DIM);

        // Count unpaid
        int unpaid = 0;
        for (int i = 0; i < app.bills.size(); i++)
            if (Validator::strEq(app.bills.get(i).getStatus(), "unpaid"))
                unpaid++;
        Validator::intToStr(unpaid, tmp, 32);
        Validator::textCpy(line, "Unpaid Bills: ", 64);
        Validator::strCat(line, tmp, 64);
        drawText(win, font, line, 15, 40.f, statsY + 42.f, Colors::ERROR);

        drawText(win, font, "Admin Panel", 24, 50.f, 165.f, Colors::WARNING);

        const char *items[] = {
            "1.  Add Doctor",
            "2.  Remove Doctor",
            "3.  Add Patient",
            "4.  Remove Patient",
            "5.  View All Patients",
            "6.  View All Doctors",
            "7.  View All Appointments",
            "8.  View Unpaid Bills",
            "9.  Discharge Patient",
            "10. View Security Log",
            "11. Generate Daily Report",
            "12. Logout"};

        if (!adminPanel.inited)
            adminPanel.init(font, items, 12, 50.f, 198.f, 340.f, 38.f, 5.f);

        int clicked = adminPanel.update(win, ev);
        if (clicked < 0)
        {
            drawStatusBar(win, font, app);
            return;
        }

        if (clicked == 11)
        {
            app.adminLoggedIn = false;
            app.currentScreen = Screen::ROLE_SELECT;
            adminPanel.reset();
            app.setStatus("Logged out.");
            return;
        }

        app.setStatus("Running console operation… check terminal.", false);
        AdminMenu::run(app.admin,
                       app.patients,
                       app.doctors,
                       app.appointments,
                       app.bills,
                       app.prescriptions);

        app.loadAll();
        adminPanel.reset();
        drawStatusBar(win, font, app);
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // Top Bar (persistent header)
    // ─────────────────────────────────────────────────────────────────────────────
    static void drawTopBar(sf::RenderWindow & win, sf::Font & font, AppState & app)
    {
        drawPanel(win, 0, 0, WIN_W, 58.f, Colors::PANEL, Colors::BORDER);
        drawPanel(win, 0, 0, 6.f, 58.f, Colors::ACCENT);
        drawText(win, font, "MediCore", 22, 20.f, 14.f, Colors::ACCENT);

        // Current time
        time_t now = time(nullptr);
        char timeBuf[32];
        strftime(timeBuf, 32, "%d-%m-%Y  %H:%M", localtime(&now));
        drawText(win, font, timeBuf, 13, WIN_W - 200.f, 20.f, Colors::TEXT_DIM);
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // Status bar (bottom)
    // ─────────────────────────────────────────────────────────────────────────────
    static void drawStatusBar(sf::RenderWindow & win, sf::Font & font, AppState & app)
    {
        if (app.statusTimer <= 0.f)
            return;
        sf::Color bg = app.statusIsError
                           ? sf::Color(80, 20, 20, 230)
                           : sf::Color(10, 60, 40, 230);
        drawPanel(win, 0, WIN_H - 36.f, WIN_W, 36.f, bg);
        drawText(win, font, app.statusMsg, 14,
                 20.f, WIN_H - 26.f,
                 app.statusIsError ? Colors::ERROR : Colors::SUCCESS);
    }

    // =============================================================================
    // main()
    // =============================================================================
    int main()
    {
        // ── Window ───────────────────────────────────────────────────────────────
        sf::RenderWindow window(
            sf::VideoMode(WIN_W, WIN_H),
            "MediCore — Hospital Management System",
            sf::Style::Titlebar | sf::Style::Close);
        window.setFramerateLimit(FPS);

        // ── Font ─────────────────────────────────────────────────────────────────
        sf::Font font;
        if (!loadFont(font))
        {
            // Cannot display without a font; fall back to console-only mode
            printf("ERROR: No font found. Please place a .ttf in the assets/ folder.\n");
            printf("Running in full console mode instead.\n\n");
            window.close();

            // Full console fallback
            Storage<Patient> patients;
            Storage<Doctor> doctors;
            Storage<Appointment> appointments;
            Storage<Bill> bills;
            Storage<Prescription> prescriptions;
            Admin admin;

            try
            {
                FileHandler::loadPatients(patients);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadDoctors(doctors);
            }
            catch (...)
            {
            }
            bool adminLoaded = false;
            try
            {
                adminLoaded = FileHandler::loadAdmin(admin);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadAppointments(appointments);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadBills(bills);
            }
            catch (...)
            {
            }
            try
            {
                FileHandler::loadPrescriptions(prescriptions);
            }
            catch (...)
            {
            }

            if (adminLoaded)
                AdminMenu::run(admin, patients, doctors, appointments, bills, prescriptions);
            return 0;
        }

        // ── App state ────────────────────────────────────────────────────────────
        AppState app;
        app.loadAll();

        sf::Clock clock;
        bool splashDone = false;

        // ── Main loop ─────────────────────────────────────────────────────────────
        while (window.isOpen())
        {
            float dt = clock.restart().asSeconds();

            // Tick status timer
            if (app.statusTimer > 0.f)
                app.statusTimer -= dt;

            sf::Event event;
            event.type = sf::Event::Count; // sentinel (no event)

            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Splash: any key advances
                if (app.currentScreen == Screen::SPLASH &&
                    splashTime > 1.5f &&
                    (event.type == sf::Event::KeyPressed ||
                     event.type == sf::Event::MouseButtonPressed))
                {
                    app.currentScreen = Screen::ROLE_SELECT;
                    splashDone = true;
                }

                // ── Route events to active screen ────────────────────────────────
                switch (app.currentScreen)
                {
                case Screen::ROLE_SELECT:
                    drawRoleSelect(window, font, event, app);
                    break;
                case Screen::LOGIN:
                    drawLogin(window, font, event, app);
                    break;
                case Screen::PATIENT_MENU:
                    drawPatientMenu(window, font, event, app);
                    break;
                case Screen::DOCTOR_MENU:
                    drawDoctorMenu(window, font, event, app);
                    break;
                case Screen::ADMIN_MENU:
                    drawAdminMenu(window, font, event, app);
                    break;
                default:
                    break;
                }
            }

            // ── Render ────────────────────────────────────────────────────────────
            window.clear(Colors::BG);

            switch (app.currentScreen)
            {
            case Screen::SPLASH:
                drawSplash(window, font, dt, app);
                break;
            case Screen::ROLE_SELECT:
                drawRoleSelect(window, font, event, app);
                break;
            case Screen::LOGIN:
                drawLogin(window, font, event, app);
                break;
            case Screen::PATIENT_MENU:
                drawPatientMenu(window, font, event, app);
                break;
            case Screen::DOCTOR_MENU:
                drawDoctorMenu(window, font, event, app);
                break;
            case Screen::ADMIN_MENU:
                drawAdminMenu(window, font, event, app);
                break;
            default:
                break;
            }

            window.display();
        }

        return 0;
    }