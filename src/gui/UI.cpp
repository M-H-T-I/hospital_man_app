// =============================================================================
// UI.cpp  –  Full SFML GUI  (no blocking terminal menus)
//
// Architecture
// ────────────
// Each menu action that needs user input opens a "Modal" overlay built from
// TextField widgets.  The modal collects the required fields, calls the
// corresponding stateless action function, then shows the result in the
// Output Panel.  Actions that are purely read-only (view*) are called
// directly and their text is shown in the Output Panel.
//
// The Output Panel is a scrollable text area on the right side of each
// menu screen.  It shows the last result until the next action is triggered.
// =============================================================================

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

// ─────────────────────────────────────────────────────────────────────────────
// Internal string helpers (no <string>)
// ─────────────────────────────────────────────────────────────────────────────
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
        dst[i++] = src[j++];
    dst[i] = '\0';
}

static bool loadFont(sf::Font &font)
{
    const char *paths[] = {
        "assets/font.ttf", "assets/Roboto-Regular.ttf",
        "assets/OpenSans-Regular.ttf", "assets/Jaro-Regular.ttf", nullptr};
    for (int i = 0; paths[i]; i++)
        if (font.loadFromFile(paths[i]))
            return true;
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Drawing primitives
// ─────────────────────────────────────────────────────────────────────────────
static void drawRect(sf::RenderWindow &win, float x, float y, float w, float h,
                     sf::Color fill, sf::Color outline = sf::Color::Transparent,
                     float thick = 0.f)
{
    sf::RectangleShape r(sf::Vector2f(w, h));
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(0.f);
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
    t.setPosition(boxX + (boxW - t.getLocalBounds().width) / 2.f, boxY);
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
static void drawStatusBar(sf::RenderWindow &win, const sf::Font &font, AppState &app)
{
    if (app.statusTimer <= 0.f)
        return;
    sf::Color bg = app.statusIsError ? sf::Color(80, 20, 20, 230) : sf::Color(10, 55, 35, 230);
    drawRect(win, 0, WIN_H - 34.f, WIN_W, 34.f, bg);
    drawLabel(win, font, app.statusMsg, 14, 18.f, WIN_H - 24.f,
              app.statusIsError ? Colors::ERROR : Colors::SUCCESS);
}

// ─────────────────────────────────────────────────────────────────────────────
// TextField
// ─────────────────────────────────────────────────────────────────────────────
void TextField::init(float x, float y, float w, float h)
{
    box.setSize(sf::Vector2f(w, h));
    box.setPosition(x, y);
    box.setFillColor(Colors::INPUT_BG);
    box.setOutlineColor(Colors::BORDER);
    box.setOutlineThickness(0.f);
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
            buf[--len] = '\0';
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

// ─────────────────────────────────────────────────────────────────────────────
// Button
// ─────────────────────────────────────────────────────────────────────────────
void Button::init(const char *text, float x, float y, float w, float h, sf::Color accent)
{
    shape.setSize(sf::Vector2f(w, h));
    shape.setPosition(x, y);
    shape.setFillColor(Colors::BTN_NORM);
    shape.setOutlineColor(Colors::BORDER);
    shape.setOutlineThickness(0.f);
    safeCpy(label, text, 64);
    accentColor = accent;
}
bool Button::isHovered(sf::Vector2i mouse) const
{
    return shape.getGlobalBounds().contains((float)mouse.x, (float)mouse.y);
}
bool Button::isClicked(sf::Vector2i mouse, const sf::Event &ev) const
{
    if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left)
        return shape.getGlobalBounds().contains((float)ev.mouseButton.x, (float)ev.mouseButton.y);
    return false;
}
void Button::draw(sf::RenderWindow &win, const sf::Font &font, sf::Vector2i mouse, unsigned cs)
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

// ─────────────────────────────────────────────────────────────────────────────
// AppState
// ─────────────────────────────────────────────────────────────────────────────
void AppState::setStatus(const char *msg, bool isError)
{
    safeCpy(statusMsg, msg, 256);
    statusIsError = isError;
    statusTimer = 4.f;
}

void AppState::loadAll()
{
    auto tryLoad = [](auto fn)
    { try { fn(); } catch (...) {} };
    tryLoad([&]
            { FileHandler::loadPatients(patients); });
    tryLoad([&]
            { FileHandler::loadDoctors(doctors); });
    tryLoad([&]
            { adminLoaded = FileHandler::loadAdmin(admin); });
    tryLoad([&]
            { FileHandler::loadAppointments(appointments); });
    tryLoad([&]
            { FileHandler::loadBills(bills); });
    tryLoad([&]
            { FileHandler::loadPrescriptions(prescriptions); });
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

// ─────────────────────────────────────────────────────────────────────────────
// MenuPanel  –  vertical list of buttons
// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
// OutputPanel  –  scrollable multi-line text area
// ─────────────────────────────────────────────────────────────────────────────
struct OutputPanel
{
    static constexpr int BUFSIZE = 8192;
    char buf[BUFSIZE] = {};
    float scrollY = 0.f; // pixels scrolled down
    float panelH = 0.f;
    float panelW = 0.f;
    float panelX = 0.f;
    float panelY = 0.f;

    void setup(float x, float y, float w, float h)
    {
        panelX = x;
        panelY = y;
        panelW = w;
        panelH = h;
    }

    void setText(const char *text)
    {
        safeCpy(buf, text, BUFSIZE);
        scrollY = 0.f;
    }

    void handleScroll(const sf::Event &ev, sf::Vector2i mouse)
    {
        if (ev.type != sf::Event::MouseWheelScrolled)
            return;
        sf::FloatRect bounds(panelX, panelY, panelW, panelH);
        if (!bounds.contains((float)mouse.x, (float)mouse.y))
            return;
        scrollY -= ev.mouseWheelScroll.delta * 18.f;
        if (scrollY < 0.f)
            scrollY = 0.f;
    }

    void draw(sf::RenderWindow &win, const sf::Font &font, sf::Color accent)
    {
        drawRect(win, panelX, panelY, panelW, panelH, Colors::PANEL, Colors::BORDER, 1.f);
        drawRect(win, panelX, panelY, panelW, 28.f, accent);
        drawLabel(win, font, "Output", 12, panelX + 8.f, panelY + 7.f, Colors::BG);

        // Clip via a RenderTexture trick is not available here; we use scissor
        // via sf::View instead.
        sf::View oldView = win.getView();

        // Create a viewport that maps to the panel interior
        float tx = panelX, ty = panelY + 28.f, tw = panelW, th = panelH - 28.f;
        sf::FloatRect vp(tx / WIN_W, ty / WIN_H, tw / WIN_W, th / WIN_H);
        sf::View clipView(sf::FloatRect(0, 0, tw, th));
        clipView.setViewport(vp);
        win.setView(clipView);

        // Draw text lines
        const unsigned CS = 13;
        float lineH = (float)(CS + 4);
        float y = -scrollY + 4.f;
        int start = 0;
        int blen = (int)strlen(buf);
        for (int i = 0; i <= blen; i++)
        {
            if (buf[i] == '\n' || buf[i] == '\0')
            {
                int len = i - start;
                if (len > 0)
                {
                    char line[512] = {};
                    int copy = len < 511 ? len : 511;
                    for (int k = 0; k < copy; k++)
                        line[k] = buf[start + k];
                    line[copy] = '\0';

                    if (y + lineH > 0.f && y < th)
                    {
                        sf::Text t;
                        t.setFont(font);
                        t.setString(line);
                        t.setCharacterSize(CS);
                        t.setFillColor(Colors::TEXT);
                        t.setPosition(6.f, y);
                        win.draw(t);
                    }
                }
                y += lineH;
                start = i + 1;
            }
        }

        win.setView(oldView);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Modal dialog  –  collects multi-field input then fires an action
// ─────────────────────────────────────────────────────────────────────────────
static constexpr int MODAL_MAX_FIELDS = 8;
static constexpr int MODAL_MAX_LABELS = 8;

struct Modal
{
    bool active = false;
    int action = -1; // which menu item triggered this modal
    char title[64] = {};
    char labels[MODAL_MAX_LABELS][64] = {};
    int fieldCount = 0;
    TextField fields[MODAL_MAX_FIELDS];
    Button submitBtn, cancelBtn;
    // Optional preview text (e.g. list of available doctors / slots)
    char previewBuf[4096] = {};
    bool hasPreview = false;
    bool inited = false;

    void open(int act, const char *ttl, int numFields,
              const char (*lbl)[64], float cx, float cy)
    {
        action = act;
        safeCpy(title, ttl, 64);
        fieldCount = numFields < MODAL_MAX_FIELDS ? numFields : MODAL_MAX_FIELDS;

        float fw = 360.f, fh = 36.f, gap = 8.f;
        float startY = cy + 54.f;
        for (int i = 0; i < fieldCount; i++)
        {
            safeCpy(labels[i], lbl[i], 64);
            fields[i].init(cx + 20.f, startY + i * (fh + gap + 18.f), fw, fh);
            fields[i].clear();
            fields[i].isPassword = false;
        }
        // Password fields: detect by label keyword
        for (int i = 0; i < fieldCount; i++)
        {
            const char *l = labels[i];
            int li = 0;
            while (l[li])
                li++;
            // simple case-insensitive search for "password"
            for (int k = 0; k + 7 < li; k++)
            {
                char s[9];
                for (int m = 0; m < 8; m++)
                    s[m] = l[k + m];
                s[8] = '\0';
                if (s[0] == 'P' || s[0] == 'p')
                    if (s[1] == 'a' || s[1] == 'A')
                    {
                        fields[i].isPassword = true;
                        break;
                    }
            }
        }

        float totalH = 54.f + fieldCount * (fh + gap + 18.f) + 60.f;
        if (hasPreview)
            totalH += 120.f;

        submitBtn.init("Submit", cx + 20.f, cy + totalH - 48.f, 160.f, 36.f, Colors::ACCENT);
        cancelBtn.init("Cancel", cx + 200.f, cy + totalH - 48.f, 160.f, 36.f, Colors::ERROR);
        previewBuf[0] = '\0';
        active = true;
        inited = true;
    }

    void close()
    {
        active = false;
        inited = false;
        action = -1;
    }

    // Returns true if submit was pressed, false if cancel
    // ev may be nullptr (render pass)
    int handleAndDraw(sf::RenderWindow &win, const sf::Font &font,
                      sf::Vector2i mouse, const sf::Event *ev)
    {
        if (!active)
            return 0;

        // Dim background
        sf::RectangleShape dim(sf::Vector2f((float)WIN_W, (float)WIN_H));
        dim.setFillColor(sf::Color(0, 0, 0, 160));
        win.draw(dim);

        float fw = 400.f;
        float totalH = 54.f + fieldCount * (54.f) + 60.f + (hasPreview ? 130.f : 0.f);
        float cx = (WIN_W - fw) / 2.f;
        float cy = (WIN_H - totalH) / 2.f;

        drawRect(win, cx, cy, fw, totalH, Colors::PANEL, Colors::BORDER, 1.5f);
        drawRect(win, cx, cy, fw, 40.f, Colors::ACCENT);
        drawCentered(win, font, title, 16, cx, cy + 10.f, fw, Colors::BG);

        float fy = cy + 50.f;
        if (hasPreview)
        {
            drawRect(win, cx + 10.f, fy, fw - 20.f, 110.f,
                     sf::Color(15, 25, 42), Colors::BORDER, 1.f);
            // draw preview lines
            const unsigned CS = 12;
            float py = fy + 4.f;
            int ps = 0;
            int plen = (int)strlen(previewBuf);
            for (int i = 0; i <= plen && py < fy + 105.f; i++)
            {
                if (previewBuf[i] == '\n' || previewBuf[i] == '\0')
                {
                    int ll = i - ps;
                    if (ll > 0)
                    {
                        char line[256] = {};
                        int cp = ll < 255 ? ll : 255;
                        for (int k = 0; k < cp; k++)
                            line[k] = previewBuf[ps + k];
                        line[cp] = '\0';
                        drawLabel(win, font, line, CS, cx + 14.f, py, Colors::TEXT_DIM);
                    }
                    py += (float)(CS + 4);
                    ps = i + 1;
                }
            }
            fy += 120.f;
        }

        for (int i = 0; i < fieldCount; i++)
        {
            // reposition fields to match final layout
            fields[i].box.setPosition(cx + 20.f, fy + 18.f);
            drawLabel(win, font, labels[i], 12, cx + 20.f, fy + 2.f, Colors::TEXT_DIM);
            if (ev)
            {
                fields[i].handleClick(mouse);
                fields[i].handleEvent(*ev);
            }
            fields[i].draw(win, font, 14);
            fy += 54.f;
        }

        // Reposition buttons
        submitBtn.shape.setPosition(cx + 20.f, fy + 6.f);
        cancelBtn.shape.setPosition(cx + 210.f, fy + 6.f);
        submitBtn.draw(win, font, mouse);
        cancelBtn.draw(win, font, mouse);

        if (ev)
        {
            if (submitBtn.isClicked(mouse, *ev))
                return 1;
            if (cancelBtn.isClicked(mouse, *ev))
                return -1;
            // Enter key submits
            if (ev->type == sf::Event::KeyPressed &&
                ev->key.code == sf::Keyboard::Return)
                return 1;
        }
        return 0;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Forward declarations
// ─────────────────────────────────────────────────────────────────────────────
static void drawSplash(sf::RenderWindow &, const sf::Font &, float, AppState &, const sf::Event *);
static void drawRoleSelect(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawLogin(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawPatientMenu(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawDoctorMenu(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);
static void drawAdminMenu(sf::RenderWindow &, const sf::Font &, AppState &, sf::Vector2i, const sf::Event *);

// ─────────────────────────────────────────────────────────────────────────────
// SPLASH
// ─────────────────────────────────────────────────────────────────────────────
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
        t.setPosition((WIN_W - t.getLocalBounds().width) / 2.f, cy + 218.f);
        win.draw(t);
        if (ev && (ev->type == sf::Event::KeyPressed ||
                   ev->type == sf::Event::MouseButtonPressed))
        {
            app.screen = Screen::ROLE_SELECT;
            splashElapsed = 0.f;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// ROLE SELECT
// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
// LOGIN
// ─────────────────────────────────────────────────────────────────────────────
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
static void drawLogin(sf::RenderWindow &win, const sf::Font &font,
                      AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    // drawRect(win, 0, 0, 5, WIN_H, Colors::ACCENT);
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
        if (!doLogin && ev->type == sf::Event::KeyPressed &&
            ev->key.code == sf::Keyboard::Return)
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

// ─────────────────────────────────────────────────────────────────────────────
// PATIENT MENU
// ─────────────────────────────────────────────────────────────────────────────
static MenuPanel patPanel;
static OutputPanel patOutput;
static Modal patModal;
// Book Appointment is a 4-step wizard: we track which step we're in.
// Step 0: spec input  -> preview doctors
// Step 1: doctor ID + date -> preview slots
// Step 2: slot selection -> book
static int patBookStep = 0;
static BookAppointmentInput patBookInput;

static void patHandleAction(int action, AppState &app)
{
    static char outBuf[8192];
    outBuf[0] = '\0';

    switch (action)
    {
    // ── 0: Book Appointment ─────────────────────────────────────────────────
    case 0:
    {
        // Step 0: ask for specialization
        patBookStep = 0;
        patBookInput = BookAppointmentInput{};
        char lbl[1][64];
        safeCpy(lbl[0], "Specialization (e.g. Cardiology)", 64);
        patModal.hasPreview = false;
        patModal.open(0, "Book Appointment – Step 1/3: Specialization",
                      1, lbl, (WIN_W - 400.f) / 2.f, (WIN_H - 220.f) / 2.f);
        break;
    }
    // ── 1: Cancel Appointment ───────────────────────────────────────────────
    case 1:
    {
        PatientMenu::listPendingAppointments(*app.loggedPatient, app.appointments,
                                             app.doctors, outBuf, 8192);
        char lbl[1][64];
        safeCpy(lbl[0], "Appointment ID to cancel", 64);
        patModal.hasPreview = true;
        safeCpy(patModal.previewBuf, outBuf, 4096);
        patModal.open(1, "Cancel Appointment", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 300.f) / 2.f);
        break;
    }
    // ── 2: View My Appointments ─────────────────────────────────────────────
    case 2:
    {
        PatientMenu::viewMyAppointments(*app.loggedPatient, app.appointments,
                                        app.doctors, outBuf, 8192);
        patOutput.setText(outBuf);
        break;
    }
    // ── 3: View My Medical Records ──────────────────────────────────────────
    case 3:
    {
        PatientMenu::viewMyMedicalRecords(*app.loggedPatient, app.prescriptions,
                                          app.doctors, outBuf, 8192);
        patOutput.setText(outBuf);
        break;
    }
    // ── 4: View My Bills ────────────────────────────────────────────────────
    case 4:
    {
        PatientMenu::viewMyBills(*app.loggedPatient, app.bills, outBuf, 8192);
        patOutput.setText(outBuf);
        break;
    }
    // ── 5: Pay Bill ─────────────────────────────────────────────────────────
    case 5:
    {
        PatientMenu::listUnpaidBills(*app.loggedPatient, app.bills, outBuf, 8192);
        char lbl[1][64];
        safeCpy(lbl[0], "Bill ID to pay", 64);
        patModal.hasPreview = true;
        safeCpy(patModal.previewBuf, outBuf, 4096);
        patModal.open(5, "Pay Bill", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 300.f) / 2.f);
        break;
    }
    // ── 6: Top Up Balance ───────────────────────────────────────────────────
    case 6:
    {
        char lbl[1][64];
        safeCpy(lbl[0], "Amount to add (PKR)", 64);
        patModal.hasPreview = false;
        patModal.open(6, "Top Up Balance", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 220.f) / 2.f);
        break;
    }
    default:
        break;
    }
}

static void drawPatientMenu(sf::RenderWindow &win, const sf::Font &font,
                            AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawTopBar(win, font);

    // Profile card
    float px = WIN_W - 310.f, py = 68.f;
    drawRect(win, px, py, 280.f, 178.f, Colors::PANEL, Colors::BORDER, 1.f);
    drawRect(win, px, py, 280.f, 32.f, Colors::ACCENT2);
    drawCentered(win, font, "Patient Profile", 12, px, py + 8.f, 280.f, Colors::BG);
    if (app.loggedPatient)
    {
        drawLabel(win, font, app.loggedPatient->getName(), 16, px + 10.f, py + 40.f, Colors::TEXT);
        char tmp[64], line[80];
        Validator::floatToStr(app.loggedPatient->getBalance(), tmp, 64, 2);
        safeCpy(line, "Balance: PKR ", 80);
        safeCat(line, tmp, 80);
        drawLabel(win, font, line, 13, px + 10.f, py + 70.f, Colors::SUCCESS);
        Validator::intToStr(app.loggedPatient->getAge(), tmp, 64);
        safeCpy(line, "Age: ", 80);
        safeCat(line, tmp, 80);
        safeCat(line, "   Gender: ", 80);
        safeCat(line, app.loggedPatient->getGender(), 80);
        drawLabel(win, font, line, 12, px + 10.f, py + 96.f, Colors::TEXT_DIM);
        drawLabel(win, font, app.loggedPatient->getContact(), 12, px + 10.f, py + 118.f, Colors::TEXT_DIM);
    }

    drawLabel(win, font, "Patient Menu", 22, 48.f, 78.f, Colors::ACCENT);
    const char *items[] = {
        "1.  Book Appointment", "2.  Cancel Appointment",
        "3.  View My Appointments", "4.  View My Medical Records",
        "5.  View My Bills", "6.  Pay Bill", "7.  Top Up Balance", "8.  Logout"};
    if (!patPanel.inited)
        patPanel.init(items, 8, 48.f, 116.f, 310.f, 40.f, 5.f);

    // Output panel
    patOutput.setup(390.f, 68.f, WIN_W - 710.f, WIN_H - 112.f);
    if (ev)
        patOutput.handleScroll(*ev, mouse);
    patOutput.draw(win, font, Colors::ACCENT2);

    int clicked = patPanel.draw(win, font, mouse, ev);
    if (clicked == 7)
    {
        app.loggedPatient = nullptr;
        app.screen = Screen::ROLE_SELECT;
        patPanel.reset();
        patOutput.setText("");
        app.setStatus("Logged out.");
    }
    else if (clicked >= 0 && !patModal.active)
        patHandleAction(clicked, app);

    // ── Modal handling ───────────────────────────────────────────────────────
    if (patModal.active)
    {
        int res = patModal.handleAndDraw(win, font, mouse, ev);
        if (res == -1)
        {
            patModal.close();
        }
        else if (res == 1)
        {
            static char outBuf[8192];
            outBuf[0] = '\0';
            bool ok = false;

            if (patModal.action == 0)
            {
                // Book step 0: got specialization -> show doctors, open step 1
                if (patBookStep == 0)
                {
                    safeCpy(patBookInput.specialization, patModal.fields[0].buf, 51);
                    char preview[4096];
                    preview[0] = '\0';
                    int found = PatientMenu::listDoctorsBySpec(app.doctors,
                                                               patBookInput.specialization, preview, 4096);
                    if (found == 0)
                    {
                        patOutput.setText(preview);
                        patModal.close();
                    }
                    else
                    {
                        // Step 1: ask doctor ID + date
                        patBookStep = 1;
                        char lbl[2][64];
                        safeCpy(lbl[0], "Doctor ID", 64);
                        safeCpy(lbl[1], "Date (DD-MM-YYYY)", 64);
                        patModal.hasPreview = true;
                        safeCpy(patModal.previewBuf, preview, 4096);
                        patModal.open(0, "Book Appointment – Step 2/3: Doctor & Date",
                                      2, lbl, (WIN_W - 400.f) / 2.f, (WIN_H - 320.f) / 2.f);
                    }
                }
                else if (patBookStep == 1)
                {
                    // Got doctor ID + date -> show slots, open step 2
                    patBookInput.doctorID = Validator::strToInt(patModal.fields[0].buf);
                    safeCpy(patBookInput.date, patModal.fields[1].buf, 16);
                    if (!Validator::isValidDate(patBookInput.date))
                    {
                        app.setStatus("Invalid date. Use DD-MM-YYYY.", true);
                        patModal.close();
                    }
                    else
                    {
                        char slotPreview[1024];
                        slotPreview[0] = '\0';
                        PatientMenu::listAvailableSlots(app.appointments,
                                                        patBookInput.doctorID, patBookInput.date,
                                                        slotPreview, 1024);
                        patBookStep = 2;
                        char lbl[1][64];
                        safeCpy(lbl[0], "Time Slot (e.g. 09:00)", 64);
                        patModal.hasPreview = true;
                        safeCpy(patModal.previewBuf, slotPreview, 4096);
                        patModal.open(0, "Book Appointment – Step 3/3: Choose Slot",
                                      1, lbl, (WIN_W - 400.f) / 2.f, (WIN_H - 300.f) / 2.f);
                    }
                }
                else // step 2: got time slot -> book
                {
                    safeCpy(patBookInput.timeSlot, patModal.fields[0].buf, 8);
                    ok = PatientMenu::bookAppointment(*app.loggedPatient,
                                                      app.doctors, app.appointments, app.bills, app.patients,
                                                      patBookInput, outBuf, 8192);
                    patOutput.setText(outBuf);
                    app.setStatus(ok ? "Appointment booked!" : outBuf, !ok);
                    if (ok)
                        app.reloadAndRefresh();
                    patModal.close();
                    patBookStep = 0;
                }
            }
            else if (patModal.action == 1)
            {
                CancelAppointmentInput cin;
                cin.appointmentID = Validator::strToInt(patModal.fields[0].buf);
                ok = PatientMenu::cancelAppointment(*app.loggedPatient,
                                                    app.appointments, app.bills, app.doctors, app.patients,
                                                    cin, outBuf, 8192);
                patOutput.setText(outBuf);
                app.setStatus(ok ? "Appointment cancelled." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                patModal.close();
            }
            else if (patModal.action == 5)
            {
                PayBillInput pin;
                pin.billID = Validator::strToInt(patModal.fields[0].buf);
                ok = PatientMenu::payBill(*app.loggedPatient, app.bills, app.patients,
                                          pin, outBuf, 8192);
                patOutput.setText(outBuf);
                app.setStatus(ok ? "Bill paid." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                patModal.close();
            }
            else if (patModal.action == 6)
            {
                TopUpInput tin;
                safeCpy(tin.amount, patModal.fields[0].buf, 32);
                ok = PatientMenu::topUpBalance(*app.loggedPatient, app.patients,
                                               tin, outBuf, 8192);
                patOutput.setText(outBuf);
                app.setStatus(ok ? "Balance topped up." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                patModal.close();
            }
        }
    }
    drawStatusBar(win, font, app);
}

// ─────────────────────────────────────────────────────────────────────────────
// DOCTOR MENU
// ─────────────────────────────────────────────────────────────────────────────
static MenuPanel docPanel;
static OutputPanel docOutput;
static Modal docModal;

static void docHandleAction(int action, AppState &app)
{
    static char outBuf[8192];
    outBuf[0] = '\0';

    switch (action)
    {
    case 0: // View Today's Appointments
        DoctorMenu::viewTodayAppointments(*app.loggedDoctor, app.appointments,
                                          app.patients, outBuf, 8192);
        docOutput.setText(outBuf);
        break;
    case 1: // Mark Complete
    {
        DoctorMenu::listTodayPending(*app.loggedDoctor, app.appointments,
                                     outBuf, 8192);
        char lbl[1][64];
        safeCpy(lbl[0], "Appointment ID to mark complete", 64);
        docModal.hasPreview = true;
        safeCpy(docModal.previewBuf, outBuf, 4096);
        docModal.open(1, "Mark Appointment Complete", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 300.f) / 2.f);
        break;
    }
    case 2: // Mark No-Show
    {
        DoctorMenu::listTodayPending(*app.loggedDoctor, app.appointments,
                                     outBuf, 8192);
        char lbl[1][64];
        safeCpy(lbl[0], "Appointment ID to mark no-show", 64);
        docModal.hasPreview = true;
        safeCpy(docModal.previewBuf, outBuf, 4096);
        docModal.open(2, "Mark Appointment No-Show", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 300.f) / 2.f);
        break;
    }
    case 3: // Write Prescription
    {
        DoctorMenu::listCompletedWithoutPrescription(
            *app.loggedDoctor, app.appointments, app.prescriptions,
            outBuf, 8192);
        char lbl[3][64];
        safeCpy(lbl[0], "Appointment ID", 64);
        safeCpy(lbl[1], "Medicines (e.g. Paracetamol 500mg;...)", 64);
        safeCpy(lbl[2], "Notes", 64);
        docModal.hasPreview = true;
        safeCpy(docModal.previewBuf, outBuf, 4096);
        docModal.open(3, "Write Prescription", 3, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 380.f) / 2.f);
        break;
    }
    case 4: // View Patient History
    {
        char lbl[1][64];
        safeCpy(lbl[0], "Patient ID", 64);
        docModal.hasPreview = false;
        docModal.open(4, "View Patient Medical History", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 220.f) / 2.f);
        break;
    }
    default:
        break;
    }
}

static void drawDoctorMenu(sf::RenderWindow &win, const sf::Font &font,
                           AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawTopBar(win, font);

    float px = WIN_W - 310.f, py = 68.f;
    drawRect(win, px, py, 280.f, 155.f, Colors::PANEL, Colors::BORDER, 1.f);
    drawRect(win, px, py, 280.f, 32.f, Colors::SUCCESS);
    drawCentered(win, font, "Doctor Profile", 12, px, py + 8.f, 280.f, Colors::BG);
    if (app.loggedDoctor)
    {
        drawLabel(win, font, app.loggedDoctor->getName(), 16, px + 10.f, py + 40.f, Colors::TEXT);
        drawLabel(win, font, app.loggedDoctor->getSpecialization(), 12, px + 10.f, py + 66.f, Colors::TEXT_DIM);
        char tmp[64], line[80];
        Validator::floatToStr(app.loggedDoctor->getFee(), tmp, 64, 2);
        safeCpy(line, "Fee: PKR ", 80);
        safeCat(line, tmp, 80);
        drawLabel(win, font, line, 13, px + 10.f, py + 90.f, Colors::SUCCESS);
    }

    drawLabel(win, font, "Doctor Menu", 22, 48.f, 78.f, Colors::SUCCESS);
    const char *items[] = {
        "1.  View Today's Appointments", "2.  Mark Appointment Complete",
        "3.  Mark Appointment No-Show", "4.  Write Prescription",
        "5.  View Patient Medical History", "6.  Logout"};
    if (!docPanel.inited)
        docPanel.init(items, 6, 48.f, 116.f, 310.f, 40.f, 5.f);

    docOutput.setup(390.f, 68.f, WIN_W - 710.f, WIN_H - 112.f);
    if (ev)
        docOutput.handleScroll(*ev, mouse);
    docOutput.draw(win, font, Colors::SUCCESS);

    int clicked = docPanel.draw(win, font, mouse, ev);
    if (clicked == 5)
    {
        app.loggedDoctor = nullptr;
        app.screen = Screen::ROLE_SELECT;
        docPanel.reset();
        docOutput.setText("");
        app.setStatus("Logged out.");
    }
    else if (clicked >= 0 && !docModal.active)
        docHandleAction(clicked, app);

    if (docModal.active)
    {
        int res = docModal.handleAndDraw(win, font, mouse, ev);
        if (res == -1)
        {
            docModal.close();
        }
        else if (res == 1)
        {
            static char outBuf[8192];
            outBuf[0] = '\0';
            bool ok = false;

            if (docModal.action == 1)
            {
                MarkAppointmentInput min;
                min.appointmentID = Validator::strToInt(docModal.fields[0].buf);
                ok = DoctorMenu::markComplete(*app.loggedDoctor, app.appointments,
                                              min, outBuf, 8192);
                docOutput.setText(outBuf);
                app.setStatus(ok ? "Appointment completed." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                docModal.close();
            }
            else if (docModal.action == 2)
            {
                MarkAppointmentInput min;
                min.appointmentID = Validator::strToInt(docModal.fields[0].buf);
                ok = DoctorMenu::markNoShow(*app.loggedDoctor, app.appointments,
                                            app.bills, min, outBuf, 8192);
                docOutput.setText(outBuf);
                app.setStatus(ok ? "Marked no-show." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                docModal.close();
            }
            else if (docModal.action == 3)
            {
                WritePrescriptionInput win2;
                win2.appointmentID = Validator::strToInt(docModal.fields[0].buf);
                safeCpy(win2.medicines, docModal.fields[1].buf, 500);
                safeCpy(win2.notes, docModal.fields[2].buf, 300);
                ok = DoctorMenu::writePrescription(*app.loggedDoctor, app.appointments,
                                                   app.prescriptions, win2, outBuf, 8192);
                docOutput.setText(outBuf);
                app.setStatus(ok ? "Prescription saved." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                docModal.close();
            }
            else if (docModal.action == 4)
            {
                ViewPatientHistoryInput hin;
                hin.patientID = Validator::strToInt(docModal.fields[0].buf);
                ok = DoctorMenu::viewPatientHistory(*app.loggedDoctor, app.patients,
                                                    app.appointments, app.prescriptions,
                                                    hin, outBuf, 8192);
                docOutput.setText(outBuf);
                if (!ok)
                    app.setStatus(outBuf, true);
                docModal.close();
            }
        }
    }
    drawStatusBar(win, font, app);
}

// ─────────────────────────────────────────────────────────────────────────────
// ADMIN MENU
// ─────────────────────────────────────────────────────────────────────────────
static MenuPanel admPanel;
static OutputPanel admOutput;
static Modal admModal;

static void admHandleAction(int action, AppState &app)
{
    static char outBuf[8192];
    outBuf[0] = '\0';

    switch (action)
    {
    case 0: // Add Doctor
    {
        char lbl[5][64];
        safeCpy(lbl[0], "Name", 64);
        safeCpy(lbl[1], "Specialization", 64);
        safeCpy(lbl[2], "Contact (11 digits)", 64);
        safeCpy(lbl[3], "Password (min 6)", 64);
        safeCpy(lbl[4], "Consultation Fee", 64);
        admModal.hasPreview = false;
        admModal.open(0, "Add Doctor", 5, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 420.f) / 2.f);
        break;
    }
    case 1: // Remove Doctor
    {
        AdminMenu::viewAllDoctors(app.doctors, outBuf, 8192);
        char lbl[1][64];
        safeCpy(lbl[0], "Doctor ID to remove", 64);
        admModal.hasPreview = true;
        safeCpy(admModal.previewBuf, outBuf, 4096);
        admModal.open(1, "Remove Doctor", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 300.f) / 2.f);
        break;
    }
    case 2: // Add Patient
    {
        char lbl[6][64];
        safeCpy(lbl[0], "Name", 64);
        safeCpy(lbl[1], "Age", 64);
        safeCpy(lbl[2], "Gender (M/F)", 64);
        safeCpy(lbl[3], "Contact (11 digits)", 64);
        safeCpy(lbl[4], "Password (min 6)", 64);
        safeCpy(lbl[5], "Initial Balance", 64);
        admModal.hasPreview = false;
        admModal.open(2, "Add Patient", 6, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 480.f) / 2.f);
        break;
    }
    case 3: // Remove Patient
    {
        AdminMenu::viewAllPatients(app.patients, app.bills, outBuf, 8192);
        char lbl[1][64];
        safeCpy(lbl[0], "Patient ID to remove", 64);
        admModal.hasPreview = true;
        safeCpy(admModal.previewBuf, outBuf, 4096);
        admModal.open(3, "Remove Patient", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 300.f) / 2.f);
        break;
    }
    case 4:
        AdminMenu::viewAllPatients(app.patients, app.bills, outBuf, 8192);
        admOutput.setText(outBuf);
        break;
    case 5:
        AdminMenu::viewAllDoctors(app.doctors, outBuf, 8192);
        admOutput.setText(outBuf);
        break;
    case 6:
        AdminMenu::viewAllAppointments(app.appointments, app.patients, app.doctors,
                                       outBuf, 8192);
        admOutput.setText(outBuf);
        break;
    case 7:
        AdminMenu::viewUnpaidBills(app.bills, app.patients, outBuf, 8192);
        admOutput.setText(outBuf);
        break;
    case 8: // Discharge Patient
    {
        char lbl[1][64];
        safeCpy(lbl[0], "Patient ID to discharge", 64);
        admModal.hasPreview = false;
        admModal.open(8, "Discharge Patient", 1, lbl,
                      (WIN_W - 400.f) / 2.f, (WIN_H - 220.f) / 2.f);
        break;
    }
    case 9:
        AdminMenu::viewSecurityLog(outBuf, 8192);
        admOutput.setText(outBuf);
        break;
    case 10:
        AdminMenu::generateDailyReport(app.appointments, app.bills,
                                       app.patients, app.doctors, outBuf, 8192);
        admOutput.setText(outBuf);
        break;
    default:
        break;
    }
}

static void drawAdminMenu(sf::RenderWindow &win, const sf::Font &font,
                          AppState &app, sf::Vector2i mouse, const sf::Event *ev)
{
    drawRect(win, 0, 0, WIN_W, WIN_H, Colors::BG);
    drawTopBar(win, font);

    // Stats bar
    float sy = 64.f;
    drawRect(win, 16.f, sy, WIN_W - 32.f, 52.f, Colors::PANEL, Colors::BORDER, 1.f);
    auto statAt = [&](const char *lbl, int val, float x, sf::Color col)
    {
        char num[16];
        Validator::intToStr(val, num, 16);
        char line[48];
        safeCpy(line, lbl, 48);
        safeCat(line, num, 48);
        drawLabel(win, font, line, 13, x, sy + 16.f, col);
    };
    statAt("Patients: ", app.patients.size(), 28.f, Colors::ACCENT);
    statAt("Doctors: ", app.doctors.size(), 188.f, Colors::SUCCESS);
    statAt("Appointments: ", app.appointments.size(), 348.f, Colors::WARNING);
    int unpaid = 0;
    for (int i = 0; i < app.bills.size(); i++)
        if (Validator::strEq(app.bills.get(i).getStatus(), "unpaid"))
            unpaid++;
    statAt("Unpaid Bills: ", unpaid, 548.f, Colors::ERROR);
    statAt("Total Bills: ", app.bills.size(), 730.f, Colors::TEXT_DIM);

    drawLabel(win, font, "Admin Panel", 20, 48.f, 126.f, Colors::WARNING);
    const char *items[] = {
        "1.   Add Doctor", "2.   Remove Doctor",
        "3.   Add Patient", "4.   Remove Patient",
        "5.   View All Patients", "6.   View All Doctors",
        "7.   View All Appointments", "8.   View Unpaid Bills",
        "9.   Discharge Patient", "10.  View Security Log",
        "11.  Generate Daily Report", "12.  Logout"};
    if (!admPanel.inited)
        admPanel.init(items, 12, 48.f, 154.f, 300.f, 32.f, 3.f);

    admOutput.setup(380.f, 120.f, WIN_W - 400.f, WIN_H - 164.f);
    if (ev)
        admOutput.handleScroll(*ev, mouse);
    admOutput.draw(win, font, Colors::WARNING);

    int clicked = admPanel.draw(win, font, mouse, ev);
    if (clicked == 11)
    {
        app.adminLoggedIn = false;
        app.screen = Screen::ROLE_SELECT;
        admPanel.reset();
        admOutput.setText("");
        app.setStatus("Logged out.");
    }
    else if (clicked >= 0 && !admModal.active)
        admHandleAction(clicked, app);

    if (admModal.active)
    {
        int res = admModal.handleAndDraw(win, font, mouse, ev);
        if (res == -1)
        {
            admModal.close();
        }
        else if (res == 1)
        {
            static char outBuf[8192];
            outBuf[0] = '\0';
            bool ok = false;

            if (admModal.action == 0) // Add Doctor                                                                                   
            {
                AddDoctorInput in;
                safeCpy(in.name, admModal.fields[0].buf, 51);
                safeCpy(in.spec, admModal.fields[1].buf, 51);
                safeCpy(in.contact, admModal.fields[2].buf, 12);
                safeCpy(in.password, admModal.fields[3].buf, 51);
                safeCpy(in.fee, admModal.fields[4].buf, 32);
                ok = AdminMenu::addDoctor(app.doctors, in, outBuf, 8192);
                admOutput.setText(outBuf);
                app.setStatus(ok ? "Doctor added." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                admModal.close();
            }
            else if (admModal.action == 1) // Remove Doctor
            {
                RemoveDoctorInput in;
                in.doctorID = Validator::strToInt(admModal.fields[0].buf);
                ok = AdminMenu::removeDoctor(app.doctors, app.appointments, in, outBuf, 8192);
                admOutput.setText(outBuf);
                app.setStatus(ok ? "Doctor removed." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                admModal.close();
            }
            else if (admModal.action == 2) // Add Patient
            {
                AddPatientInput in;
                safeCpy(in.name, admModal.fields[0].buf, 51);
                safeCpy(in.age, admModal.fields[1].buf, 8);
                safeCpy(in.gender, admModal.fields[2].buf, 3);
                safeCpy(in.contact, admModal.fields[3].buf, 12);
                safeCpy(in.password, admModal.fields[4].buf, 51);
                safeCpy(in.balance, admModal.fields[5].buf, 32);
                ok = AdminMenu::addPatient(app.patients, in, outBuf, 8192);
                admOutput.setText(outBuf);
                app.setStatus(ok ? "Patient added." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                admModal.close();
            }
            else if (admModal.action == 3) // Remove Patient
            {
                RemovePatientInput in;
                in.patientID = Validator::strToInt(admModal.fields[0].buf);
                ok = AdminMenu::removePatient(app.patients, app.appointments,
                                              app.bills, app.prescriptions,
                                              in, outBuf, 8192);
                admOutput.setText(outBuf);
                app.setStatus(ok ? "Patient removed." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                admModal.close();
            }
            else if (admModal.action == 8) // Discharge Patient
            {
                DischargePatientInput in;
                in.patientID = Validator::strToInt(admModal.fields[0].buf);
                ok = AdminMenu::dischargePatient(app.patients, app.appointments,
                                                 app.bills, app.prescriptions,
                                                 in, outBuf, 8192);
                admOutput.setText(outBuf);
                app.setStatus(ok ? "Patient discharged." : outBuf, !ok);
                if (ok)
                    app.reloadAndRefresh();
                admModal.close();
            }
        }
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
        printf("ERROR: No usable font found. Place any .ttf in assets/\n");
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