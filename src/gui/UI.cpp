#include "UI.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "PatientMenu.hpp"
#include "DoctorMenu.hpp"
#include "AdminMenu.hpp"

#include <SFML/Graphics.hpp>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <iostream>
using namespace std;

static void safeCpy(char *dst, const char *src, int max)
{
    int i = 0;
    while (i < max - 1 && src[i])
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}
static void safeCat(char *dst, const char *src, int max)
{
    int i = 0;
    while (dst[i])
        i++;
    int j = 0;
    while (i < max - 1 && src[j])
    {
        dst[i++] = src[j++];
    }
    dst[i] = '\0';
}

static bool loadFont(sf::Font &font)
{
    const char *paths[] = {
        "assets/font.ttf",
        "assets/Roboto-Regular.ttf",
        "assets/OpenSans-Regular.ttf",
        "assets/Jaro-Regular.ttf",
        nullptr};
    for (int i = 0; paths[i]; i++)
        if (font.loadFromFile(paths[i]))
        {
            cout << "fonts loaded." << endl;
            return true;
        };
    return false;
}

// drawing functions
static void drawRect(sf::RenderWindow &win, float x, float y, float w, float h, sf::Color fill, sf::Color outline = sf::Color::Transparent,
                     float thick = 0.f)
{

    sf::RectangleShape r(sf::Vector2f(w, h));
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(thick);
    win.draw(r);
}

static void drawLabel(sf::RenderWindow &win, const sf::Font &font,
                      const char *str, unsigned size,
                      float x, float y, sf::Color col = Colors::TEXT)
{
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(col);
    t.setPosition(x, y);
    win.draw(t);
}

static void drawCentered(sf::RenderWindow &win, const sf::Font &font,
                         const char *str, unsigned size,
                         float boxX, float boxY, float boxW,
                         sf::Color col = Colors::TEXT)
{
    sf::Text t;
    t.setFont(font);
    t.setString(str);
    t.setCharacterSize(size);
    t.setFillColor(col);
    float tw = t.getLocalBounds().width;
    t.setPosition(boxX + (boxW - tw) / 2.f, boxY);
    win.draw(t);
}

static void drawTopBar(sf::RenderWindow &win, const sf::Font &font)
{
    drawRect(win, 0, 0, WIN_W, 58, Colors::PANEL, Colors::BORDER, 1.f);
    drawRect(win, 0, 0, 5, 58, Colors::ACCENT);
    drawLabel(win, font, "MediCore", 22, 20, 16, Colors::ACCENT);
    time_t now = time(nullptr);
    char tbuf[32];
    strftime(tbuf, 32, "%d-%m-%Y   %H:%M", localtime(&now));
    drawLabel(win, font, tbuf, 13, WIN_W - 195.f, 20.f, Colors::TEXT_DIM);
}

static void drawStatusBar(sf::RenderWindow &win, const sf::Font &font,
                          AppState &app)
{
    if (app.statusTimer <= 0.f)
        return;
    sf::Color bg = app.statusIsError ? sf::Color(80, 20, 20, 230)
                                     : sf::Color(10, 55, 35, 230);
    drawRect(win, 0, WIN_H - 34.f, WIN_W, 34.f, bg);
    drawLabel(win, font, app.statusMsg, 14, 18.f, WIN_H - 24.f,
              app.statusIsError ? Colors::ERROR : Colors::SUCCESS);
}


void TextField::init(float x, float y, float w, float h)
{
    box.setSize(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(Colors::INPUT_BG);
    box.setOutlineColor(Colors::BORDER);
    box.setOutlineThickness(1.f);
    clear();
}

void TextField::handleEvent(const sf::Event &ev)
{
    if (!focused)
        return;
    if (ev.type == sf::Event::TextEntered)
    {
        sf::Uint32 c = ev.text.unicode;
        if (c == 8 && len > 0)
        {
            buf[--len] = '\0';
        }
        else if (c >= 32 && c < 127 && len < 254)
        {
            buf[len++] = (char)c;
            buf[len] = '\0';
        }
    }
}

void TextField::handleClick(sf::Vector2i mouse)
{
    focused = box.getGlobalBounds().contains((float)mouse.x, (float)mouse.y);
}

void TextField::clear()
{
    for (int i = 0; i < 256; i++)
        buf[i] = '\0';
    len = 0;
}

void TextField::draw(sf::RenderWindow &win, const sf::Font &font, unsigned cs)
{
    box.setFillColor(focused ? Colors::INPUT_ON : Colors::INPUT_BG);
    box.setOutlineColor(focused ? Colors::ACCENT : Colors::BORDER);
    win.draw(box);

    char tmp[260] = {};
    if (isPassword)
    {
        for (int i = 0; i < len; i++)
            tmp[i] = '*';
        tmp[len] = '\0';
    }
    else
        safeCpy(tmp, buf, 258);
    if (focused)
    {
        tmp[len] = '|';
        tmp[len + 1] = '\0';
    }

    sf::Text t;
    t.setFont(font);
    t.setString(tmp);
    t.setCharacterSize(cs);
    t.setFillColor(Colors::TEXT);
    sf::Vector2f pos = box.getPosition();
    t.setPosition(pos.x + 8.f, pos.y + (box.getSize().y - cs) / 2.f - 2.f);
    win.draw(t);
}


void Button::init(const char *text, float x, float y, float w, float h, sf::Color accent)
{
    shape.setSize(sf::Vector2f(w, h));
    shape.setPosition(x, y);
    shape.setFillColor(Colors::BTN_NORM);
    shape.setOutlineColor(Colors::BORDER);
    shape.setOutlineThickness(1.f);
    safeCpy(label, text, 64);
    accentColor = accent;
}

bool Button::isHovered(sf::Vector2i mouse) const
{
    return shape.getGlobalBounds().contains((float)mouse.x, (float)mouse.y);
}

bool Button::isClicked(sf::Vector2i mouse, const sf::Event &ev) const
{
    if (ev.type == sf::Event::MouseButtonPressed &&
        ev.mouseButton.button == sf::Mouse::Left)
        return shape.getGlobalBounds().contains((float)ev.mouseButton.x,
                                                (float)ev.mouseButton.y);
    return false;
}

void Button::draw(sf::RenderWindow &win, const sf::Font &font,
                  sf::Vector2i mouse, unsigned cs)
{
    bool hov = isHovered(mouse);
    shape.setFillColor(hov ? Colors::BTN_HOV : Colors::BTN_NORM);
    shape.setOutlineColor(hov ? accentColor : Colors::BORDER);
    win.draw(shape);

    sf::Text t;
    t.setFont(font);
    t.setString(label);
    t.setCharacterSize(cs);
    t.setFillColor(hov ? sf::Color::White : Colors::TEXT);
    sf::Vector2f pos = shape.getPosition(), sz = shape.getSize();
    float tw = t.getLocalBounds().width, th = t.getLocalBounds().height;
    t.setPosition(pos.x + (sz.x - tw) / 2.f, pos.y + (sz.y - th) / 2.f - 4.f);
    win.draw(t);
}


void AppState::setStatus(const char *msg, bool isError)
{
    safeCpy(statusMsg, msg, 256);
    statusIsError = isError;
    statusTimer = 4.f;
}

void AppState::loadAll()
{
    try
    {
        FileHandler::loadPatients(patients);
    }
    catch (...)
    {
        cout<< "Error in loading patients."<<endl;

    }
    try
    {
        FileHandler::loadDoctors(doctors);
    }
    catch (...)
    {
        cout<< "Error in loading doctors."<<endl;

    }
    try
    {
        adminLoaded = FileHandler::loadAdmin(admin);
    }
    catch (...)
    {
        cout<< "Error in loading admins."<<endl;

    }
    try
    {
        FileHandler::loadAppointments(appointments);
    }
    catch (...)
    {
        cout<< "Error in loading appointments."<<endl;

    }
    try
    {
        FileHandler::loadBills(bills);
    }
    catch (...)
    {
        cout<< "Error in loading bills."<<endl;

    }
    try
    {
        FileHandler::loadPrescriptions(prescriptions);
    }
    catch (...)
    {
        cout<< "Error in loading prescriptions."<<endl;
    }
}

void AppState::reloadAndRefresh()
{
    int pid = loggedPatient ? loggedPatient->getID() : -1;
    int did = loggedDoctor ? loggedDoctor->getID() : -1;
    patients.clear();
    doctors.clear();
    appointments.clear();
    bills.clear();
    prescriptions.clear();
    loadAll();
    loggedPatient = pid >= 0 ? patients.findByID(pid) : nullptr;
    loggedDoctor = did >= 0 ? doctors.findByID(did) : nullptr;
    if (!loggedPatient && screen == Screen::PATIENT_MENU)
        screen = Screen::ROLE_SELECT;
    if (!loggedDoctor && screen == Screen::DOCTOR_MENU)
        screen = Screen::ROLE_SELECT;
}


struct MenuPanel
{
    static constexpr int MAX = 13;
    Button btns[MAX];
    int count = 0;
    bool inited = false;

    void init(const char **items, int n, float x, float y,
              float w = 330.f, float h = 44.f, float gap = 6.f)
    {
        count = n < MAX ? n : MAX;
        for (int i = 0; i < count; i++)
            btns[i].init(items[i], x, y + i * (h + gap), w, h, Colors::ACCENT);
        inited = true;
    }
    int draw(sf::RenderWindow &win, const sf::Font &font,
             sf::Vector2i mouse, const sf::Event *ev)
    {
        for (int i = 0; i < count; i++)
        {
            btns[i].draw(win, font, mouse, 15);
            if (ev && btns[i].isClicked(mouse, *ev))
                return i;
        }
        return -1;
    }
    void reset() { inited = false; }
};


static void drawSplash(sf::RenderWindow &, const sf::Font &, float, AppState &, const sf::Event *);
static void drawRoleSelect(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawLogin(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawPatientMenu(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawDoctorMenu(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawAdminMenu(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);


static float splashElapsed = 0.f;
static void drawSplash(sf::RenderWindow &win, const sf::Font &font,
                       float dt, AppState &app, const sf::Event *ev)
{
    splashElapsed += dt;
    for (int i = 0; i < 8; i++)
    {
        int v = 12 + i * 3;
        drawRect(win, 0, i * WIN_H / 8.f, WIN_W, WIN_H / 8.f + 1.f,
                 sf::Color((sf::Uint8)v, (sf::Uint8)(v + 6), (sf::Uint8)(v + 18)));
    }
    float pulse = 0.85f + 0.15f * std::sin(splashElapsed * 2.f);
    float cx = WIN_W / 2.f, cy = WIN_H / 2.f - 70.f, arm = 55.f * pulse, thick = 17.f * pulse;
    sf::RectangleShape hb(sf::Vector2f(arm * 2.f, thick));
    hb.setOrigin(arm, thick / 2.f);
    hb.setPosition(cx, cy);
    hb.setFillColor(Colors::ACCENT);
    win.draw(hb);
    sf::RectangleShape vb(sf::Vector2f(thick, arm * 2.f));
    vb.setOrigin(thick / 2.f, arm);
    vb.setPosition(cx, cy);
    vb.setFillColor(Colors::ACCENT);
    win.draw(vb);
    drawCentered(win, font, "MediCore", 52, 0, cy + 80.f, WIN_W, Colors::ACCENT);
    drawCentered(win, font, "Hospital Management System", 20, 0, cy + 146.f, WIN_W, Colors::TEXT_DIM);
    if (splashElapsed > 1.4f)
    {
        float alpha = std::min(1.f, (splashElapsed - 1.4f) / 0.5f);
        sf::Color c = Colors::TEXT_DIM;
        c.a = (sf::Uint8)(alpha * 200.f);
        sf::Text t;
        t.setFont(font);
        t.setString("Press any key to continue");
        t.setCharacterSize(15);
        t.setFillColor(c);
        float tw = t.getLocalBounds().width;
        t.setPosition((WIN_W - tw) / 2.f, cy + 218.f);
        win.draw(t);
        if (ev && (ev->type == sf::Event::KeyPressed || ev->type == sf::Event::MouseButtonPressed))
        {
            app.screen = Screen::ROLE_SELECT;
            splashElapsed = 0.f;
        }
    }
}


static Button roleButtons[4];
static bool roleButtonsInit = false;
static void drawRoleSelect(sf::RenderWindow &win, const sf::Font &font,
                           AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawRect(win, 0, 0, 5, WIN_H, Colors::ACCENT);
    float cw = 420.f, ch = 460.f, cx = (WIN_W - cw) / 2.f, cy = (WIN_H - ch) / 2.f;
    drawRect(win, cx, cy, cw, ch, Colors::PANEL, Colors::BORDER, 1.5f);
    drawRect(win, cx, cy, cw, 64.f, Colors::ACCENT);
    drawCentered(win, font, "MediCore", 26, cx, cy + 16.f, cw, Colors::BG);
    drawCentered(win, font, "Select your role to continue", 14, cx, cy + 78.f, cw, Colors::TEXT_DIM);
    const char *labels[4] = {"Patient", "Doctor", "Admin", "Exit"};
    sf::Color accents[4] = {Colors::ACCENT2, Colors::SUCCESS, Colors::WARNING, Colors::ERROR};
    if (!roleButtonsInit)
    {
        for (int i = 0; i < 4; i++)
            roleButtons[i].init(labels[i], cx + 50.f, cy + 116.f + i * 74.f, cw - 100.f, 54.f, accents[i]);
        roleButtonsInit = true;
    }
    for (int i = 0; i < 4; i++)
    {
        roleButtons[i].draw(win, font, mouse, 17);
        if (ev && roleButtons[i].isClicked(mouse, *ev))
        {
            if (i == 3)
            {
                win.close();
                return;
            }
            app.selectedRole = i + 1;
            app.loginAttempts = 0;
            app.screen = Screen::LOGIN;
            roleButtonsInit = false;
        }
    }
    drawStatusBar(win, font, app);
}


static TextField loginIdField, loginPwField;
static Button loginBtn, loginBackBtn;
static bool loginInited = false;
static void initLoginWidgets(float cx, float cy, float cw)
{
    loginIdField.init(cx + 40.f, cy + 90.f, cw - 80.f, 42.f);
    loginIdField.isPassword = false;
    loginPwField.init(cx + 40.f, cy + 168.f, cw - 80.f, 42.f);
    loginPwField.isPassword = true;
    loginBtn.init("Login", cx + 40.f, cy + 238.f, cw - 80.f, 46.f, Colors::ACCENT);
    loginBackBtn.init("Back", cx + 40.f, cy + 298.f, cw - 80.f, 40.f, Colors::TEXT_DIM);
    loginInited = true;
}


// login screen
static void drawLogin(sf::RenderWindow &win, const sf::Font &font,
                      AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawRect(win, 0, 0, 5, WIN_H, Colors::ACCENT);
    float cw = 420.f, ch = 380.f, cx = (WIN_W - cw) / 2.f, cy = (WIN_H - ch) / 2.f;
    drawRect(win, cx, cy, cw, ch, Colors::PANEL, Colors::BORDER, 1.5f);
    drawRect(win, cx, cy, cw, 60.f, Colors::ACCENT);
    const char *titles[] = {"", "Patient Login", "Doctor Login", "Admin Login"};
    drawCentered(win, font, titles[app.selectedRole], 22, cx, cy + 16.f, cw, Colors::BG);
    if (!loginInited)
        initLoginWidgets(cx, cy, cw);
    if (ev)
    {
        loginIdField.handleClick(mouse);
        loginPwField.handleClick(mouse);
        loginIdField.handleEvent(*ev);
        loginPwField.handleEvent(*ev);
    }
    drawLabel(win, font, "User ID", 13, cx + 40.f, cy + 72.f, Colors::TEXT_DIM);
    loginIdField.draw(win, font);
    drawLabel(win, font, "Password", 13, cx + 40.f, cy + 152.f, Colors::TEXT_DIM);
    loginPwField.draw(win, font);
    loginBtn.draw(win, font, mouse);
    loginBackBtn.draw(win, font, mouse);

    bool doLogin = false;
    if (ev)
    {
        doLogin = loginBtn.isClicked(mouse, *ev);
        if (!doLogin && ev->type == sf::Event::KeyPressed && ev->key.code == sf::Keyboard::Return)
            doLogin = true;
    }
    if (doLogin)
    {
        if (!Validator::isValidID(loginIdField.buf))
        {
            app.setStatus("Please enter a valid numeric ID.", true);
        }
        else
        {
            int id = Validator::strToInt(loginIdField.buf);
            bool ok = false;
            if (app.selectedRole == 1)
            {
                Patient *p = app.patients.findByID(id);
                if (p && Validator::strEq(p->getPassword(), loginPwField.buf))
                {
                    app.loggedPatient = p;
                    app.screen = Screen::PATIENT_MENU;
                    char msg[64] = "Welcome, ";
                    safeCat(msg, p->getName(), 64);
                    safeCat(msg, "!", 64);
                    app.setStatus(msg);
                    ok = true;
                    loginInited = false;
                    loginIdField.clear();
                    loginPwField.clear();
                }
            }
            else if (app.selectedRole == 2)
            {
                Doctor *d = app.doctors.findByID(id);
                if (d && Validator::strEq(d->getPassword(), loginPwField.buf))
                {
                    app.loggedDoctor = d;
                    app.screen = Screen::DOCTOR_MENU;
                    char msg[64] = "Welcome, Dr. ";
                    safeCat(msg, d->getName(), 64);
                    app.setStatus(msg);
                    ok = true;
                    loginInited = false;
                    loginIdField.clear();
                    loginPwField.clear();
                }
            }
            else if (app.selectedRole == 3)
            {
                if (app.adminLoaded && app.admin.getID() == id &&
                    Validator::strEq(app.admin.getPassword(), loginPwField.buf))
                {
                    app.adminLoggedIn = true;
                    app.screen = Screen::ADMIN_MENU;
                    app.setStatus("Welcome, Admin!");
                    ok = true;
                    loginInited = false;
                    loginIdField.clear();
                    loginPwField.clear();
                }
            }
            if (!ok)
            {
                app.loginAttempts++;
                const char *role = app.selectedRole == 1 ? "Patient" : app.selectedRole == 2 ? "Doctor"
                                                                                             : "Admin";
                FileHandler::logSecurityEvent(role, loginIdField.buf, "FAILED");
                if (app.loginAttempts >= 3)
                {
                    app.setStatus("Account locked. Contact admin.", true);
                    FileHandler::logSecurityEvent(role, loginIdField.buf, "LOCKED");
                    app.screen = Screen::ROLE_SELECT;
                    app.loginAttempts = 0;
                    loginInited = false;
                    loginIdField.clear();
                    loginPwField.clear();
                }
                else
                {
                    char msg[80] = "Invalid credentials. Attempt ";
                    char num[4];
                    Validator::intToStr(app.loginAttempts, num, 4);
                    safeCat(msg, num, 80);
                    safeCat(msg, "/3", 80);
                    app.setStatus(msg, true);
                    loginPwField.clear();
                }
            }
        }
    }
    if (ev && loginBackBtn.isClicked(mouse, *ev))
    {
        app.screen = Screen::ROLE_SELECT;
        loginInited = false;
        loginIdField.clear();
        loginPwField.clear();
    }
    drawStatusBar(win, font, app);
}

// PATIENT MENU
static MenuPanel patPanel;
static void drawPatientMenu(sf::RenderWindow &win, const sf::Font &font,
                            AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawTopBar(win, font);
    float px = WIN_W - 330.f, py = 72.f;
    drawRect(win, px, py, 300.f, 190.f, Colors::PANEL, Colors::BORDER, 1.f);
    drawRect(win, px, py, 300.f, 36.f, Colors::ACCENT2);
    drawCentered(win, font, "Patient Profile", 13, px, py + 10.f, 300.f, Colors::BG);
    if (app.loggedPatient)
    {
        drawLabel(win, font, app.loggedPatient->getName(), 17, px + 12.f, py + 46.f, Colors::TEXT);
        char tmp[64], line[80];
        Validator::floatToStr(app.loggedPatient->getBalance(), tmp, 64, 2);
        safeCpy(line, "Balance: PKR ", 80);
        safeCat(line, tmp, 80);
        drawLabel(win, font, line, 13, px + 12.f, py + 78.f, Colors::SUCCESS);
        Validator::intToStr(app.loggedPatient->getAge(), tmp, 64);
        safeCpy(line, "Age: ", 80);
        safeCat(line, tmp, 80);
        safeCat(line, "   Gender: ", 80);
        safeCat(line, app.loggedPatient->getGender(), 80);
        drawLabel(win, font, line, 12, px + 12.f, py + 106.f, Colors::TEXT_DIM);
        drawLabel(win, font, app.loggedPatient->getContact(), 12, px + 12.f, py + 130.f, Colors::TEXT_DIM);
    }
    drawLabel(win, font, "Patient Menu", 24, 48.f, 80.f, Colors::ACCENT);
    const char *items[] = {"1.  Book Appointment", "2.  Cancel Appointment",
                           "3.  View My Appointments", "4.  View My Medical Records",
                           "5.  View My Bills", "6.  Pay Bill", "7.  Top Up Balance", "8.  Logout"};
    if (!patPanel.inited)
        patPanel.init(items, 8, 48.f, 120.f, 330.f, 44.f, 6.f);
    int clicked = patPanel.draw(win, font, mouse, ev);
    if (clicked == 7)
    {
        app.loggedPatient = nullptr;
        app.screen = Screen::ROLE_SELECT;
        patPanel.reset();
        app.setStatus("Logged out.");
    }
    else if (clicked >= 0)
    {
        PatientMenu::run(*app.loggedPatient, app.doctors, app.appointments, app.bills, app.prescriptions, app.patients);
        app.reloadAndRefresh();
        patPanel.reset();
        app.setStatus("Action complete.");
    }
    drawStatusBar(win, font, app);
}


// DOCTOR PANEL

static MenuPanel docPanel;
static void drawDoctorMenu(sf::RenderWindow &win, const sf::Font &font,
                           AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawTopBar(win, font);
    float px = WIN_W - 330.f, py = 72.f;
    drawRect(win, px, py, 300.f, 165.f, Colors::PANEL, Colors::BORDER, 1.f);
    drawRect(win, px, py, 300.f, 36.f, Colors::SUCCESS);
    drawCentered(win, font, "Doctor Profile", 13, px, py + 10.f, 300.f, Colors::BG);
    if (app.loggedDoctor)
    {
        drawLabel(win, font, app.loggedDoctor->getName(), 17, px + 12.f, py + 46.f, Colors::TEXT);
        drawLabel(win, font, app.loggedDoctor->getSpecialization(), 13, px + 12.f, py + 74.f, Colors::TEXT_DIM);
        char tmp[64], line[80];
        Validator::floatToStr(app.loggedDoctor->getFee(), tmp, 64, 2);
        safeCpy(line, "Fee: PKR ", 80);
        safeCat(line, tmp, 80);
        drawLabel(win, font, line, 13, px + 12.f, py + 100.f, Colors::SUCCESS);
    }
    drawLabel(win, font, "Doctor Menu", 24, 48.f, 80.f, Colors::SUCCESS);
    const char *items[] = {"1.  View Today's Appointments", "2.  Mark Appointment Complete",
                           "3.  Mark Appointment No-Show", "4.  Write Prescription",
                           "5.  View Patient Medical History", "6.  Logout"};
    if (!docPanel.inited)
        docPanel.init(items, 6, 48.f, 120.f, 330.f, 44.f, 6.f);
    int clicked = docPanel.draw(win, font, mouse, ev);
    if (clicked == 5)
    {
        app.loggedDoctor = nullptr;
        app.screen = Screen::ROLE_SELECT;
        docPanel.reset();
        app.setStatus("Logged out.");
    }
    else if (clicked >= 0)
    {
        DoctorMenu::run(*app.loggedDoctor, app.appointments, app.patients, app.prescriptions, app.bills);
        app.reloadAndRefresh();
        docPanel.reset();
        app.setStatus("Action complete.");
    }
    drawStatusBar(win, font, app);
}



// ADMIN MENU
static MenuPanel admPanel;
static void drawAdminMenu(sf::RenderWindow &win, const sf::Font &font,
                          AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawTopBar(win, font);
    float sy = 68.f;
    drawRect(win, 16.f, sy, WIN_W - 32.f, 60.f, Colors::PANEL, Colors::BORDER, 1.f);
    auto statAt = [&](const char *lbl, int val, float x, sf::Color col)
    {
        char num[16];
        Validator::intToStr(val, num, 16);
        char line[48];
        safeCpy(line, lbl, 48);
        safeCat(line, num, 48);
        drawLabel(win, font, line, 14, x, sy + 18.f, col);
    };
    statAt("Patients: ", app.patients.size(), 30.f, Colors::ACCENT);
    statAt("Doctors: ", app.doctors.size(), 200.f, Colors::SUCCESS);
    statAt("Appointments: ", app.appointments.size(), 370.f, Colors::WARNING);
    int unpaid = 0;
    for (int i = 0; i < app.bills.size(); i++)
        if (Validator::strEq(app.bills.get(i).getStatus(), "unpaid"))
            unpaid++;
    statAt("Unpaid Bills: ", unpaid, 580.f, Colors::ERROR);
    statAt("Total Bills: ", app.bills.size(), 780.f, Colors::TEXT_DIM);
    drawLabel(win, font, "Admin Panel", 22, 48.f, 140.f, Colors::WARNING);
    const char *items[] = {"1.   Add Doctor", "2.   Remove Doctor", "3.   Add Patient",
                           "4.   Remove Patient", "5.   View All Patients", "6.   View All Doctors",
                           "7.   View All Appointments", "8.   View Unpaid Bills", "9.   Discharge Patient",
                           "10.  View Security Log", "11.  Generate Daily Report", "12.  Logout"};
    if (!admPanel.inited)
        admPanel.init(items, 12, 48.f, 170.f, 330.f, 36.f, 4.f);
    int clicked = admPanel.draw(win, font, mouse, ev);
    if (clicked == 11)
    {
        app.adminLoggedIn = false;
        app.screen = Screen::ROLE_SELECT;
        admPanel.reset();
        app.setStatus("Logged out.");
    }
    else if (clicked >= 0)
    {
        AdminMenu::run(app.admin, app.patients, app.doctors, app.appointments, app.bills, app.prescriptions);
        app.reloadAndRefresh();
        admPanel.reset();
        app.setStatus("Action complete.");
    }
    drawStatusBar(win, font, app);
}


void ui_run(AppState &app)
{
    sf::RenderWindow window(sf::VideoMode(WIN_W, WIN_H),
                            "MediCore - Hospital Management System");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!loadFont(font))
    {
        printf("ERROR: No usable font found. Place any .ttf in assets/ folder.\n");
        window.close();
        return;
    }

    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (app.statusTimer > 0.f)
            app.statusTimer -= dt;
        sf::Vector2i mouse = sf::Mouse::getPosition(window);

        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
            switch (app.screen)
            {
            case Screen::SPLASH:
                drawSplash(window, font, 0.f, app, &ev);
                break;
            case Screen::ROLE_SELECT:
                drawRoleSelect(window, font, app, mouse, &ev);
                break;
            case Screen::LOGIN:
                drawLogin(window, font, app, mouse, &ev);
                break;
            case Screen::PATIENT_MENU:
                drawPatientMenu(window, font, app, mouse, &ev);
                break;
            case Screen::DOCTOR_MENU:
                drawDoctorMenu(window, font, app, mouse, &ev);
                break;
            case Screen::ADMIN_MENU:
                drawAdminMenu(window, font, app, mouse, &ev);
                break;
            }
        }

        window.clear(Colors::BG);
        switch (app.screen)
        {
        case Screen::SPLASH:
            drawSplash(window, font, dt, app, nullptr);
            break;
        case Screen::ROLE_SELECT:
            drawRoleSelect(window, font, app, mouse, nullptr);
            break;
        case Screen::LOGIN:
            drawLogin(window, font, app, mouse, nullptr);
            break;
        case Screen::PATIENT_MENU:
            drawPatientMenu(window, font, app, mouse, nullptr);
            break;
        case Screen::DOCTOR_MENU:
            drawDoctorMenu(window, font, app, mouse, nullptr);
            break;
        case Screen::ADMIN_MENU:
            drawAdminMenu(window, font, app, mouse, nullptr);
            break;
        }
        window.display();
    }
}