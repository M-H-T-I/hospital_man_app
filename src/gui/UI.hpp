#pragma once
// =============================================================================
// UI.hpp  —  SFML 3 GUI layer for MediCore
// All drawing, input handling, and screen state live here.
// main.cpp only calls the functions declared at the bottom of this file.
// =============================================================================

#include <SFML/Graphics.hpp>
#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

// ─────────────────────────────────────────────────────────────────────────────
// Colour palette
// ─────────────────────────────────────────────────────────────────────────────
namespace Colors {
    inline const sf::Color BG       {  12,  18,  30 };
    inline const sf::Color PANEL    {  20,  30,  50 };
    inline const sf::Color ACCENT   {   0, 180, 216 };
    inline const sf::Color ACCENT2  {  72, 149, 239 };
    inline const sf::Color TEXT     { 220, 230, 245 };
    inline const sf::Color TEXT_DIM { 120, 140, 170 };
    inline const sf::Color SUCCESS  {  56, 183, 100 };
    inline const sf::Color ERROR    { 230,  70,  70 };
    inline const sf::Color WARNING  { 255, 185,   0 };
    inline const sf::Color BORDER   {  40,  60,  90 };
    inline const sf::Color BTN_NORM {  30,  45,  70 };
    inline const sf::Color BTN_HOV  {   0, 150, 190 };
    inline const sf::Color INPUT_BG {  28,  40,  62 };
    inline const sf::Color INPUT_ON {  35,  50,  80 };
}

static constexpr unsigned WIN_W = 1100;
static constexpr unsigned WIN_H = 700;

// ─────────────────────────────────────────────────────────────────────────────
// Screen enum – which screen is currently active
// ─────────────────────────────────────────────────────────────────────────────
enum class Screen {
    SPLASH,
    ROLE_SELECT,
    LOGIN,
    PATIENT_MENU,
    DOCTOR_MENU,
    ADMIN_MENU
};

// ─────────────────────────────────────────────────────────────────────────────
// TextField  (SFML 3 compatible)
// ─────────────────────────────────────────────────────────────────────────────
struct TextField {
    sf::RectangleShape box;
    char   buf[256] = {};
    int    len      = 0;
    bool   focused  = false;
    bool   isPassword = false;

    void init(float x, float y, float w, float h);
    void handleEvent(const sf::Event& ev);
    void handleClick(sf::Vector2i mouse);
    void clear();
    void draw(sf::RenderWindow& win, const sf::Font& font, unsigned charSize = 16);
};

// ─────────────────────────────────────────────────────────────────────────────
// Button  (SFML 3 compatible)
// ─────────────────────────────────────────────────────────────────────────────
struct Button {
    sf::RectangleShape shape;
    char label[64] = {};
    sf::Color accentColor = Colors::ACCENT;

    void init(const char* text, float x, float y, float w, float h,
              sf::Color accent = Colors::ACCENT);
    bool isHovered(sf::Vector2i mouse) const;
    bool isClicked(sf::Vector2i mouse, const sf::Event& ev) const;
    void draw(sf::RenderWindow& win, const sf::Font& font,
              sf::Vector2i mouse, unsigned charSize = 15);
};

// ─────────────────────────────────────────────────────────────────────────────
// AppState – all session data in one place
// ─────────────────────────────────────────────────────────────────────────────
struct AppState {
    Storage<Patient>      patients;
    Storage<Doctor>       doctors;
    Storage<Appointment>  appointments;
    Storage<Bill>         bills;
    Storage<Prescription> prescriptions;
    Admin                 admin;
    bool                  adminLoaded = false;

    Screen  screen        = Screen::SPLASH;
    int     selectedRole  = 0;   // 1=Patient 2=Doctor 3=Admin
    int     loginAttempts = 0;

    Patient* loggedPatient = nullptr;
    Doctor*  loggedDoctor  = nullptr;
    bool     adminLoggedIn = false;

    // Status banner
    char  statusMsg[256]  = {};
    bool  statusIsError   = false;
    float statusTimer     = 0.f;

    void setStatus(const char* msg, bool isError = false);
    void loadAll();
    void reloadAndRefresh();  // reload + fix stale pointers
};

// ─────────────────────────────────────────────────────────────────────────────
// Public API called by main()
// ─────────────────────────────────────────────────────────────────────────────

// Creates window, runs the full GUI event loop until the window closes.
void ui_run(AppState& app);