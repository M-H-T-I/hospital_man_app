#pragma once

#include <SFML/Graphics.hpp>
#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

namespace Colors
{
    // inline const sf::Color BG{12, 18, 30};
    // inline const sf::Color PANEL{20, 30, 50};
    // inline const sf::Color ACCENT{0, 180, 216};
    // inline const sf::Color ACCENT2{72, 149, 239};
    // inline const sf::Color TEXT{220, 230, 245};
    // inline const sf::Color TEXT_DIM{120, 140, 170};
    // inline const sf::Color SUCCESS{56, 183, 100};
    // inline const sf::Color ERROR{230, 70, 70};
    // inline const sf::Color WARNING{255, 185, 0};
    // inline const sf::Color BORDER{40, 60, 90};
    // inline const sf::Color BTN_NORM{30, 45, 70};
    // inline const sf::Color BTN_HOV{0, 150, 190};
    // inline const sf::Color INPUT_BG{28, 40, 62};
    // inline const sf::Color INPUT_ON{35, 50, 80};

    // Base Surfaces
    inline const sf::Color BG{255, 255, 255};       // Pure White
    inline const sf::Color PANEL{255, 255, 255};    
    // Accents
    inline const sf::Color ACCENT{0, 0, 0};         // Pure Black
    inline const sf::Color ACCENT2{0, 0, 0};        // Pure Black
    
    // Typography
    inline const sf::Color TEXT{0, 0, 0};           // Pure Black
    inline const sf::Color TEXT_DIM{0, 0, 0};       // Pure Black (No grey allowed)
    
    // Status (All mapped to Black for a true 1-bit look)
    inline const sf::Color SUCCESS{0, 0, 0}; 
    inline const sf::Color ERROR{0, 0, 0};   
    inline const sf::Color WARNING{0, 0, 0}; 
    
    // UI Elements
    inline const sf::Color BORDER{0, 0, 0};         // Pure Black
    inline const sf::Color BTN_NORM{255, 255, 255}; // White Button
    inline const sf::Color BTN_HOV{0, 0, 0};        // Black Button (Inverts on hover)
    
    // Inputs
    inline const sf::Color INPUT_BG{255, 255, 255}; 
    inline const sf::Color INPUT_ON{255, 255, 255};
}

static constexpr unsigned WIN_W = 1100;
static constexpr unsigned WIN_H = 700;

enum class Screen
{
    SPLASH,
    ROLE_SELECT,
    LOGIN,
    PATIENT_MENU,
    DOCTOR_MENU,
    ADMIN_MENU
};

struct TextField
{
    sf::RectangleShape box;
    char buf[256] = {};
    int len = 0;
    bool focused = false;
    bool isPassword = false;

    //(call once or after a resize for fixing screen weghaira
    void init(float x, float y, float w, float h);

    // moiue focus events
    void handleEvent(const sf::Event &ev);

    // Focus this field if the mouse is inside its box
    void handleClick(sf::Vector2i mouse);

    // Wipe contents
    void clear();

    // Draw box + text; charSize defaults to 16
    void draw(sf::RenderWindow &win, const sf::Font &font, unsigned charSize = 16);
};


// all buttons
struct Button
{
    sf::RectangleShape shape;
    char label[64] = {};
    sf::Color accentColor = Colors::ACCENT;

    // initialise x and y wegaira + label
    void init(const char *text, float x, float y, float w, float h,
              sf::Color accent = Colors::ACCENT);

    bool isHovered(sf::Vector2i mouse) const;

    // Returns true if a left-click lands inside the shape
    bool isClicked(sf::Vector2i mouse, const sf::Event &ev) const;

    // Draw with hover highlight; charSize defaults to 15
    void draw(sf::RenderWindow &win, const sf::Font &font,
              sf::Vector2i mouse, unsigned charSize = 15);
};


struct AppState
{
    // Persistent data
    Storage<Patient> patients;
    Storage<Doctor> doctors;
    Storage<Appointment> appointments;
    Storage<Bill> bills;
    Storage<Prescription> prescriptions;
    Admin admin;
    bool adminLoaded = false;

    // Navigation
    Screen screen = Screen::SPLASH;
    int selectedRole = 0; // 1 = Patient, 2 = Doctor, 3 = Admin
    int loginAttempts = 0;

    // Logged-in session (raw pointers into the Storage arrays)
    Patient *loggedPatient = nullptr;
    Doctor *loggedDoctor = nullptr;
    bool adminLoggedIn = false;

    // Transient status banner
    char statusMsg[256] = {};
    bool statusIsError = false;
    float statusTimer = 0.f; // seconds remaining

    // Show a banner at the bottom of the window
    void setStatus(const char *msg, bool isError = false);

    // (Re)load all data files; tolerates missing files
    void loadAll();

    // Reload data files and fix stale pointers into Storage after a write
    void reloadAndRefresh();
};

// ─────────────────────────────────────────────────────────────────────────────
// Public API  — called once from main()
// ─────────────────────────────────────────────────────────────────────────────

// Creates the SFML window and runs the full GUI event loop until it closes.
void ui_run(AppState &app);