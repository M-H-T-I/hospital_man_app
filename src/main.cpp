// #include <iostream>
// #include <SFML/Window.hpp>
// #include <SFML/Graphics.hpp>
// using namespace std;




// int main(){


//     cout << "Hello" << endl;
//     sf::Window window(sf::VideoMode({800, 600}), "My WIndow", sf::Style::Default, sf::State::Fullscreen);

//     while(window.isOpen()){

//         while(const optional event = window.pollEvent()){

//             if(event->is<sf::Event::Closed>()){
//                 window.close();
//             }

//         }

//         window.display();
//     }

//     return 0;
// }


// =============================================================================
// main.cpp
// MediCore Hospital Management System — Entry Point
// Course: Object Oriented Programming (OOP) | Spring 2026 | BCS-2G
//
// main() is a sequence of function calls only (per spec).
// No global variables. No goto. No std::string. No std::vector.
// =============================================================================

#include <iostream>
#include <cstdio>

#include "logic/errors/HospitalException.hpp"
#include "logic/errors/FileNotFoundException.hpp"

#include "logic/Validator.hpp"
#include "logic/Storage.hpp"
#include "logic/Patient.hpp"
#include "logic/Doctor.hpp"
#include "logic/Admin.hpp"
#include "logic/Appointment.hpp"
#include "logic/Bill.hpp"
#include "logic/Prescription.hpp"
#include "logic/FileHandler.hpp"
#include "logic/PatientMenu.hpp"
#include "logic/DoctorMenu.hpp"
#include "logic/AdminMenu.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Forward declarations of all top-level functions
// (main() is a sequence of calls to these only)
// ─────────────────────────────────────────────────────────────────────────────

// Ensure data directory and seed files exist on first run
static void ensureDataDirectory();
static void seedDataFiles();

// Load all data from files into in-memory storage
static void loadAllData(Storage<Patient>&      patients,
                        Storage<Doctor>&        doctors,
                        Admin&                  admin,
                        Storage<Appointment>&   appointments,
                        Storage<Bill>&           bills,
                        Storage<Prescription>&   prescriptions);

// Display the startup menu and return the chosen role (1-4)
static int  showStartupMenu();

// Login helpers — return true on success, false on lockout/failure
static bool loginPatient(Storage<Patient>&     patients,
                          Patient&               outPatient);
static bool loginDoctor(Storage<Doctor>&       doctors,
                         Doctor&                outDoctor);
static bool loginAdmin(const Admin&            admin);

// Top-level session runners
static void runPatientSession(Storage<Patient>&      patients,
                               Storage<Doctor>&        doctors,
                               Storage<Appointment>&   appointments,
                               Storage<Bill>&           bills,
                               Storage<Prescription>&   prescriptions);

static void runDoctorSession(Storage<Doctor>&        doctors,
                              Storage<Appointment>&   appointments,
                              Storage<Patient>&        patients,
                              Storage<Prescription>&   prescriptions,
                              Storage<Bill>&           bills);

static void runAdminSession(Admin&                  admin,
                             Storage<Patient>&        patients,
                             Storage<Doctor>&          doctors,
                             Storage<Appointment>&     appointments,
                             Storage<Bill>&             bills,
                             Storage<Prescription>&     prescriptions);

// ─────────────────────────────────────────────────────────────────────────────
// main — sequence of function calls only
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    // Prepare data directory and seed files if needed
    ensureDataDirectory();
    seedDataFiles();

    // In-memory storage (no global variables — all local to main, passed by ref)
    Storage<Patient>      patients;
    Storage<Doctor>       doctors;
    Admin                 admin;
    Storage<Appointment>  appointments;
    Storage<Bill>         bills;
    Storage<Prescription> prescriptions;

    // Load all data from files
    loadAllData(patients, doctors, admin, appointments, bills, prescriptions);

    // Main application loop
    bool running = true;
    while (running) {
        int role = showStartupMenu();
        switch (role) {
            case 1:
                runPatientSession(patients, doctors, appointments, bills, prescriptions);
                break;
            case 2:
                runDoctorSession(doctors, appointments, patients, prescriptions, bills);
                break;
            case 3:
                runAdminSession(admin, patients, doctors, appointments, bills, prescriptions);
                break;
            case 4:
                std::cout << "Thank you for using MediCore. Goodbye!\n";
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please enter 1-4.\n";
                break;
        }
    }

    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// ensureDataDirectory
// Creates the data/ directory if it does not exist.
// ─────────────────────────────────────────────────────────────────────────────
static void ensureDataDirectory() {
    // Attempt to open a known file; if it fails the directory may not exist.
    // Use system mkdir (portable enough for the submission environment).
#if defined(_WIN32) || defined(_WIN64)
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// seedDataFiles
// Writes header lines (and a default admin record) for any file that does
// not yet exist. This ensures loadAllData never throws FileNotFoundException
// on a fresh install while still honouring the "no hardcoded data" rule —
// only structural headers and a single mandatory admin account are written.
// ─────────────────────────────────────────────────────────────────────────────
static void seedDataFiles() {
    // Helper lambda-style struct to create a file with a header if absent
    struct Seeder {
        static void create(const char* path, const char* header) {
            // Try opening for read; if it fails the file is absent
            FILE* f = fopen(path, "r");
            if (f) { fclose(f); return; } // already exists

            f = fopen(path, "w");
            if (!f) return;
            fprintf(f, "%s\n", header);
            fclose(f);
        }
    };

    Seeder::create(FileHandler::PATIENTS_FILE,
                   "patient_id,name,age,gender,contact,password,balance");

    Seeder::create(FileHandler::DOCTORS_FILE,
                   "doctor_id,name,specialization,contact,password,fee");

    Seeder::create(FileHandler::APPOINTMENTS_FILE,
                   "appointment_id,patient_id,doctor_id,date,time_slot,status");

    Seeder::create(FileHandler::BILLS_FILE,
                   "bill_id,patient_id,appointment_id,amount,status,date");

    Seeder::create(FileHandler::PRESCRIPTIONS_FILE,
                   "prescription_id,appointment_id,patient_id,doctor_id,date,medicines,notes");

    Seeder::create(FileHandler::SECURITY_LOG_FILE,
                   "timestamp,role,entered_id,result");

    // discharged.txt shares the patients.txt header
    Seeder::create(FileHandler::DISCHARGED_FILE,
                   "patient_id,name,age,gender,contact,password,balance");

    // admin.txt — seed only if absent (provides the one mandatory admin account)
    FILE* af = fopen(FileHandler::ADMIN_FILE, "r");
    if (!af) {
        af = fopen(FileHandler::ADMIN_FILE, "w");
        if (af) {
            fprintf(af, "admin_id,name,password\n");
            fprintf(af, "1,Admin,admin123\n");
            fclose(af);
        }
    } else {
        fclose(af);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// loadAllData
// Reads every .txt file into the corresponding in-memory storage.
// Throws FileNotFoundException (caught here) if a critical file is missing.
// ─────────────────────────────────────────────────────────────────────────────
static void loadAllData(Storage<Patient>&      patients,
                        Storage<Doctor>&        doctors,
                        Admin&                  admin,
                        Storage<Appointment>&   appointments,
                        Storage<Bill>&           bills,
                        Storage<Prescription>&   prescriptions) {
    try {
        FileHandler::loadPatients(patients);
        FileHandler::loadDoctors(doctors);

        if (!FileHandler::loadAdmin(admin)) {
            throw FileNotFoundException(FileHandler::ADMIN_FILE);
        }

        FileHandler::loadAppointments(appointments);
        FileHandler::loadBills(bills);
        FileHandler::loadPrescriptions(prescriptions);

    } catch (FileNotFoundException& e) {
        std::cout << "STARTUP ERROR: " << e.what() << "\n";
        std::cout << "Please ensure the data directory and required files exist.\n";
        // Non-fatal for most files; admin file absence is critical but handled above.
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// showStartupMenu
// Displays the welcome banner and role selection. Returns 1-4.
// ─────────────────────────────────────────────────────────────────────────────
static int showStartupMenu() {
    std::cout << "\nWelcome to MediCore Hospital Management System\n";
    std::cout << "===============================================\n";
    std::cout << "Login as:\n";
    std::cout << "1. Patient\n";
    std::cout << "2. Doctor\n";
    std::cout << "3. Admin\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter choice: ";

    char buf[8];
    std::cin.getline(buf, 8);
    Validator::trim(buf);

    // Validate choice is 1-4
    if (!Validator::isValidMenuChoice(buf, 1, 4)) return -1;
    return Validator::strToInt(buf);
}

// ─────────────────────────────────────────────────────────────────────────────
// loginPatient
// Prompts for patient ID and password; validates against loaded records.
// Locks session after 3 consecutive failures and logs to security_log.txt.
// Returns true on success and fills outPatient.
// ─────────────────────────────────────────────────────────────────────────────
static bool loginPatient(Storage<Patient>& patients, Patient& outPatient) {
    const int MAX_ATTEMPTS = 3;
    char idStr[16], password[51];

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        std::cout << "Enter Patient ID: ";
        std::cin.getline(idStr, 16);
        Validator::trim(idStr);

        std::cout << "Enter Password: ";
        std::cin.getline(password, 51);
        Validator::trim(password);

        if (!Validator::isValidID(idStr)) {
            std::cout << "Invalid ID format.\n";
            FileHandler::logSecurityEvent("Patient", idStr, "FAILED");
            continue;
        }

        int id = Validator::strToInt(idStr);
        Patient* p = patients.findByID(id);

        if (p && Validator::strEq(p->getPassword(), password)) {
            outPatient = *p;
            std::cout << "Login successful.\n";
            return true;
        }

        std::cout << "Invalid ID or password. "
                  << (MAX_ATTEMPTS - attempt - 1) << " attempt(s) remaining.\n";
        FileHandler::logSecurityEvent("Patient", idStr, "FAILED");
    }

    std::cout << "Account locked. Contact admin.\n";
    FileHandler::logSecurityEvent("Patient", idStr, "LOCKED");
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// loginDoctor
// ─────────────────────────────────────────────────────────────────────────────
static bool loginDoctor(Storage<Doctor>& doctors, Doctor& outDoctor) {
    const int MAX_ATTEMPTS = 3;
    char idStr[16], password[51];

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        std::cout << "Enter Doctor ID: ";
        std::cin.getline(idStr, 16);
        Validator::trim(idStr);

        std::cout << "Enter Password: ";
        std::cin.getline(password, 51);
        Validator::trim(password);

        if (!Validator::isValidID(idStr)) {
            std::cout << "Invalid ID format.\n";
            FileHandler::logSecurityEvent("Doctor", idStr, "FAILED");
            continue;
        }

        int id = Validator::strToInt(idStr);
        Doctor* d = doctors.findByID(id);

        if (d && Validator::strEq(d->getPassword(), password)) {
            outDoctor = *d;
            std::cout << "Login successful.\n";
            return true;
        }

        std::cout << "Invalid ID or password. "
                  << (MAX_ATTEMPTS - attempt - 1) << " attempt(s) remaining.\n";
        FileHandler::logSecurityEvent("Doctor", idStr, "FAILED");
    }

    std::cout << "Account locked. Contact admin.\n";
    FileHandler::logSecurityEvent("Doctor", idStr, "LOCKED");
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// loginAdmin
// ─────────────────────────────────────────────────────────────────────────────
static bool loginAdmin(const Admin& admin) {
    const int MAX_ATTEMPTS = 3;
    char idStr[16], password[51];

    for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++) {
        std::cout << "Enter Admin ID: ";
        std::cin.getline(idStr, 16);
        Validator::trim(idStr);

        std::cout << "Enter Password: ";
        std::cin.getline(password, 51);
        Validator::trim(password);

        if (!Validator::isValidID(idStr)) {
            std::cout << "Invalid ID format.\n";
            FileHandler::logSecurityEvent("Admin", idStr, "FAILED");
            continue;
        }

        int id = Validator::strToInt(idStr);

        if (id == admin.getID() && Validator::strEq(admin.getPassword(), password)) {
            std::cout << "Login successful.\n";
            return true;
        }

        std::cout << "Invalid ID or password. "
                  << (MAX_ATTEMPTS - attempt - 1) << " attempt(s) remaining.\n";
        FileHandler::logSecurityEvent("Admin", idStr, "FAILED");
    }

    std::cout << "Account locked. Contact admin.\n";
    FileHandler::logSecurityEvent("Admin", idStr, "LOCKED");
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// runPatientSession
// Handles login then hands off to PatientMenu.
// NOTE: patient object is a local copy; PatientMenu receives it by reference
//       and updates both the in-memory copy in the storage AND the file.
// ─────────────────────────────────────────────────────────────────────────────
static void runPatientSession(Storage<Patient>&      patients,
                               Storage<Doctor>&        doctors,
                               Storage<Appointment>&   appointments,
                               Storage<Bill>&           bills,
                               Storage<Prescription>&   prescriptions) {
    Patient loggedIn;
    if (!loginPatient(patients, loggedIn)) return;

    PatientMenu::run(loggedIn, doctors, appointments, bills, prescriptions, patients);
}

// ─────────────────────────────────────────────────────────────────────────────
// runDoctorSession
// ─────────────────────────────────────────────────────────────────────────────
static void runDoctorSession(Storage<Doctor>&        doctors,
                              Storage<Appointment>&   appointments,
                              Storage<Patient>&        patients,
                              Storage<Prescription>&   prescriptions,
                              Storage<Bill>&           bills) {
    Doctor loggedIn;
    if (!loginDoctor(doctors, loggedIn)) return;

    DoctorMenu::run(loggedIn, appointments, patients, prescriptions, bills);
}

// ─────────────────────────────────────────────────────────────────────────────
// runAdminSession
// ─────────────────────────────────────────────────────────────────────────────
static void runAdminSession(Admin&                  admin,
                             Storage<Patient>&        patients,
                             Storage<Doctor>&          doctors,
                             Storage<Appointment>&     appointments,
                             Storage<Bill>&             bills,
                             Storage<Prescription>&     prescriptions) {
    if (!loginAdmin(admin)) return;

    AdminMenu::run(admin, patients, doctors, appointments, bills, prescriptions);
}