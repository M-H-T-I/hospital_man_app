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

    inline const sf::Color BG{255, 255, 255};
    inline const sf::Color PANEL{255, 255, 255};

    inline const sf::Color ACCENT{0, 0, 0};
    inline const sf::Color ACCENT2{0, 0, 0};

    inline const sf::Color TEXT{0, 0, 0};
    inline const sf::Color TEXT_DIM{0, 0, 0};

    inline const sf::Color SUCCESS{0, 0, 0};
    inline const sf::Color ERROR{0, 0, 0};
    inline const sf::Color WARNING{0, 0, 0};

    inline const sf::Color BORDER{0, 0, 0};
    inline const sf::Color BTN_NORM{255, 255, 255};
    inline const sf::Color BTN_HOV{0, 0, 0};

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

    void init(float x, float y, float w, float h);

    void handleEvent(const sf::Event &ev);

    void handleClick(sf::Vector2i mouse);

    void clear();

    void draw(sf::RenderWindow &win, const sf::Font &font, unsigned charSize = 16);
};

struct Button
{
    sf::RectangleShape shape;
    char label[64] = {};
    sf::Color accentColor = Colors::ACCENT;

    void init(const char *text, float x, float y, float w, float h,
              sf::Color accent = Colors::ACCENT);

    bool isHovered(sf::Vector2i mouse) const;

    bool isClicked(sf::Vector2i mouse, const sf::Event &ev) const;

    void draw(sf::RenderWindow &win, const sf::Font &font,
              sf::Vector2i mouse, unsigned charSize = 15);
};

struct AppState
{

    Storage<Patient> patients;
    Storage<Doctor> doctors;
    Storage<Appointment> appointments;
    Storage<Bill> bills;
    Storage<Prescription> prescriptions;
    Admin admin;
    bool adminLoaded = false;

    Screen screen = Screen::SPLASH;
    int selectedRole = 0;
    int loginAttempts = 0;

    Patient *loggedPatient = nullptr;
    Doctor *loggedDoctor = nullptr;
    bool adminLoggedIn = false;

    char statusMsg[256] = {};
    bool statusIsError = false;
    float statusTimer = 0.f;

    void setStatus(const char *msg, bool isError = false);

    void loadAll();

    void reloadAndRefresh();
};

void ui_run(AppState &app);