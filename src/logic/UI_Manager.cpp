#include "UI_Manager.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
// Widget helpers
// ─────────────────────────────────────────────────────────────────────────────

void Button::setPosition(float x, float y) { shape.setPosition({x, y}); }
void Button::setSize(float w, float h) { shape.setSize({w, h}); }

bool Button::contains(sf::Vector2f p) const
{
    return shape.getGlobalBounds().contains(p);
}

void Button::update(sf::Vector2f mouse)
{
    hovered = enabled && shape.getGlobalBounds().contains(mouse);
    shape.setFillColor(hovered ? hoverColor : normalColor);
}

void Button::draw(sf::RenderWindow &w)
{
    if (!enabled)
        shape.setFillColor(sf::Color(40, 70, 100));
    w.draw(shape);
    w.draw(label);
}

bool TextInput::contains(sf::Vector2f p) const
{
    return box.getGlobalBounds().contains(p);
}

void TextInput::handleTextEntered(int c)
{
    if (!focused)
        return;
    if (c == '\b')
    {
        if (!value.empty())
            value.pop_back();
    }
    else if (c >= 32 && c < 127)
    {
        if ((int)value.size() < maxLength)
        {
            value += static_cast<char>(c);
        }
    }
}

void TextInput::handleKey(sf::Keyboard::Key k)
{
    if (!focused)
        return;
    if (k == sf::Keyboard::Key::Backspace && !value.empty())
        value.pop_back();
}

void TextInput::draw(sf::RenderWindow &w)
{
    box.setFillColor(focused ? Theme::INPUT_FOCUS : Theme::INPUT_BG);
    box.setOutlineColor(focused ? Theme::ACCENT : Theme::BORDER);
    box.setOutlineThickness(focused ? 2.f : 1.f);
    w.draw(box);

    if (value.empty() && !focused)
    {
        w.draw(placeholder);
    }
    else
    {
        std::string display = password ? std::string(value.size(), '*') : value;
        text.setString(display);
        w.draw(text);
    }
}

void Toast::show(const std::string &message, sf::Color c, sf::Font &font)
{
    msg.setFont(font);
    msg.setString(message);
    msg.setCharacterSize(Theme::FONT_MD);
    msg.setFillColor(sf::Color::White);
    color = c;
    float w = std::max(300.f, (msg.getLocalBounds().size.x + 40.f));
    box.setSize({w, 46.f});
    box.setFillColor(c);
    box.setPosition({1280.f / 2.f - w / 2.f, 720.f - 80.f});
    msg.setPosition({box.getPosition().x + 20.f, box.getPosition().y + 12.f});
    timer.restart();
    active = true;
}

bool Toast::isDone() const
{
    return active && timer.getElapsedTime().asSeconds() > duration;
}

void Toast::draw(sf::RenderWindow &w)
{
    if (!active)
        return;
    float elapsed = timer.getElapsedTime().asSeconds();
    float alpha = 1.f;
    if (elapsed > duration - 0.4f)
        alpha = 1.f - (elapsed - (duration - 0.4f)) / 0.4f;
    sf::Color c = color;
    c.a = static_cast<int>(alpha * 220.f);
    box.setFillColor(c);
    sf::Color tc = sf::Color::White;
    tc.a = c.a;
    msg.setFillColor(tc);
    w.draw(box);
    w.draw(msg);
    if (isDone())
        active = false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
UI_Manager::UI_Manager(Storage<Patient> &p,
                       Storage<Doctor> &d,
                       Admin &a,
                       Storage<Appointment> &ap,
                       Storage<Bill> &b,
                       Storage<Prescription> &pr)
    : patients(p), doctors(d), admin(a),
      appointments(ap), bills(b), prescriptions(pr){
    window.create(sf::VideoMode({1280, 720}), "Medicore", sf::Style::Default);
    window.setFramerateLimit(60);

    // Try to load a system font — try several common paths
    const char *fontPaths[] = {
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        nullptr};
    bool fontLoaded = false;
    for (int i = 0; fontPaths[i]; i++)
    {
        if (font.loadFromFile(fontPaths[i]))
        {
            fontLoaded = true;
            break;
        }
    }
    (void)fontLoaded; // SFML will use a fallback if loading fails

    buildStartup();
}

// ─────────────────────────────────────────────────────────────────────────────
// Main run loop
// ─────────────────────────────────────────────────────────────────────────────
void UI_Manager::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
            handleEvent(e);
        }
        // update button hover states
        sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto &btn : buttons)
            btn.update(mouse);

        draw();
        clock.restart();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Event dispatcher
// ─────────────────────────────────────────────────────────────────────────────
void UI_Manager::handleEvent(const sf::Event &e)
{
    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    if (e.type == sf::Event::TextEntered)
    {
        for (auto &inp : inputs)
            inp.handleTextEntered(e.text.unicode);
    }

    if (e.type == sf::Event::MouseButtonPressed &&
        e.mouseButton.button == sf::Mouse::Left)
    {
        // focus inputs
        for (auto &inp : inputs)
            inp.focused = inp.contains(mouse);

        // click buttons
        for (auto &btn : buttons)
        {
            if (btn.enabled && btn.contains(mouse))
                handleButtonClick(btn.id);
        }

        // click table rows
        if (!tableRows.empty())
        {
            float startY = 160.f + 36.f - scrollOffset;
            for (int i = 0; i < (int)tableRows.size(); i++)
            {
                sf::FloatRect row(220.f, startY + i * 36.f, 900.f, 36.f);
                if (row.contains(mouse))
                {
                    selectedRow = i;
                    break;
                }
            }
        }
    }

    if (e.type == sf::Event::MouseWheelScrolled)
    {
        scrollOffset -= e.mouseWheelScroll.delta * 30.f;
        if (scrollOffset < 0)
            scrollOffset = 0;
        if (scrollOffset > maxScroll)
            scrollOffset = maxScroll;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Master draw
// ─────────────────────────────────────────────────────────────────────────────
void UI_Manager::draw()
{
    window.clear(Theme::BG);
    drawBackground();

    for (auto &lbl : labels)
        lbl.draw(window);
    for (auto &inp : inputs)
        inp.draw(window);
    for (auto &btn : buttons)
        btn.draw(window);

    if (!tableHeaders.empty())
        drawTable(220.f, 160.f, 900.f);

    toast.draw(window);
    window.display();
}

// ─────────────────────────────────────────────────────────────────────────────
// Background: subtle grid + gradient overlay
// ─────────────────────────────────────────────────────────────────────────────
void UI_Manager::drawBackground()
{
    // Subtle dot grid
    for (int x = 0; x < 1280; x += 40)
    {
        for (int y = 0; y < 720; y += 40)
        {
            sf::CircleShape dot(1.f);
            dot.setFillColor(sf::Color(30, 70, 110, 60));
            dot.setPosition((float)x, (float)y);
            window.draw(dot);
        }
    }

    // Left sidebar panel
    sf::RectangleShape sidebar({200.f, 720.f});
    sidebar.setFillColor(Theme::PANEL);
    sidebar.setPosition(0.f, 0.f);
    window.draw(sidebar);

    // Sidebar accent line
    sf::RectangleShape accentLine({3.f, 720.f});
    accentLine.setFillColor(Theme::ACCENT);
    accentLine.setPosition(197.f, 0.f);
    window.draw(accentLine);

    // Top bar
    sf::RectangleShape topBar({1280.f, 56.f});
    topBar.setFillColor(sf::Color(Theme::PANEL.r, Theme::PANEL.g, Theme::PANEL.b, 230));
    topBar.setPosition(0.f, 0.f);
    window.draw(topBar);

    // Top bar bottom border
    sf::RectangleShape topBorder({1280.f, 1.f});
    topBorder.setFillColor(Theme::BORDER);
    topBorder.setPosition(0.f, 55.f);
    window.draw(topBorder);

    // Logo area in sidebar
    sf::Text logo;
    logo.setFont(font);
    logo.setString("Medi\nCore");
    logo.setCharacterSize(22);
    logo.setFillColor(Theme::ACCENT);
    logo.setStyle(sf::Text::Bold);
    logo.setPosition(20.f, 14.f);
    window.draw(logo);

    // Version tag
    sf::Text ver;
    ver.setFont(font);
    ver.setString("HMS v1.0");
    ver.setCharacterSize(10);
    ver.setFillColor(Theme::TEXT_DARK);
    ver.setPosition(20.f, 700.f);
    window.draw(ver);
}

// ─────────────────────────────────────────────────────────────────────────────
// Table renderer
// ─────────────────────────────────────────────────────────────────────────────
void UI_Manager::drawTable(float x, float y, float w, float rowH)
{
    if (tableHeaders.empty())
        return;

    int cols = (int)tableHeaders.size();
    float colW = w / cols;

    // Clip region (simple scissor via view — draw header always, rows scroll)
    // Header
    sf::RectangleShape hdrBg({w, rowH});
    hdrBg.setFillColor(sf::Color(0x00, 0x66, 0x88, 200));
    hdrBg.setPosition(x, y);
    window.draw(hdrBg);

    for (int c = 0; c < cols; c++)
    {
        sf::Text hdr;
        hdr.setFont(font);
        hdr.setString(tableHeaders[c]);
        hdr.setCharacterSize(Theme::FONT_SM);
        hdr.setFillColor(Theme::ACCENT);
        hdr.setStyle(sf::Text::Bold);
        hdr.setPosition(x + c * colW + 8.f, y + 8.f);
        window.draw(hdr);

        // Column divider
        if (c > 0)
        {
            sf::RectangleShape div({1.f, rowH});
            div.setFillColor(Theme::BORDER);
            div.setPosition(x + c * colW, y);
            window.draw(div);
        }
    }

    // Rows — scrollable
    float visY = y + rowH;
    float maxVisH = 720.f - visY - 60.f;
    maxScroll = std::max(0.f, (float)tableRows.size() * rowH - maxVisH);

    // Clip view
    sf::View oldView = window.getView();
    sf::View clipView = window.getDefaultView();
    clipView.setViewport(sf::FloatRect(x / 1280.f, visY / 720.f, w / 1280.f, maxVisH / 720.f));
    clipView.setCenter(x + w / 2.f, visY + maxVisH / 2.f + scrollOffset);
    clipView.setSize(w, maxVisH);
    window.setView(clipView);

    for (int i = 0; i < (int)tableRows.size(); i++)
    {
        float ry = visY + i * rowH;
        sf::Color rowFill = (i == selectedRow)
                                ? sf::Color(0x00, 0x66, 0x88, 120)
                                : (i % 2 == 0 ? Theme::CARD
                                              : sf::Color(Theme::PANEL.r, Theme::PANEL.g, Theme::PANEL.b, 180));

        sf::RectangleShape rowBg({w, rowH - 1.f});
        rowBg.setFillColor(rowFill);
        rowBg.setPosition(x, ry);
        window.draw(rowBg);

        // Bottom border
        sf::RectangleShape rowBdr({w, 1.f});
        rowBdr.setFillColor(Theme::BORDER);
        rowBdr.setPosition(x, ry + rowH - 1.f);
        window.draw(rowBdr);

        // Cells
        const auto &row = tableRows[i];
        for (int c = 0; c < cols && c < (int)row.cells.size(); c++)
        {
            sf::Text cell;
            cell.setFont(font);
            cell.setString(row.cells[c]);
            cell.setCharacterSize(Theme::FONT_SM);
            cell.setFillColor(row.rowColor.a > 0 ? row.rowColor : Theme::TEXT);
            cell.setPosition(x + c * colW + 8.f, ry + 9.f);
            window.draw(cell);
        }
    }

    window.setView(oldView);

    // Scrollbar
    if (maxScroll > 0)
    {
        float sbH = maxVisH * (maxVisH / ((float)tableRows.size() * rowH));
        float sbY = visY + scrollOffset / maxScroll * (maxVisH - sbH);
        sf::RectangleShape sb({6.f, sbH});
        sb.setFillColor(Theme::SCROLLBAR);
        sb.setPosition(x + w - 8.f, sbY);
        window.draw(sb);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Widget factories
// ─────────────────────────────────────────────────────────────────────────────
Button UI_Manager::makeButton(const std::string &id, const std::string &lbl,
                              float x, float y, float w, float h,
                              sf::Color fill)
{
    Button btn;
    btn.id = id;
    btn.normalColor = fill;
    btn.hoverColor = sf::Color(
        std::min(255, (int)fill.r + 30),
        std::min(255, (int)fill.g + 30),
        std::min(255, (int)fill.b + 30));
    btn.shape.setSize({w, h});
    btn.shape.setPosition(x, y);
    btn.shape.setFillColor(fill);
    btn.shape.setOutlineColor(Theme::BORDER);
    btn.shape.setOutlineThickness(1.f);
    btn.label.setFont(font);
    btn.label.setString(lbl);
    btn.label.setCharacterSize(Theme::FONT_MD);
    btn.label.setFillColor(sf::Color::White);
    // Center label
    sf::FloatRect tb = btn.label.getLocalBounds();
    btn.label.setPosition(x + w / 2.f - tb.width / 2.f - tb.left,
                          y + h / 2.f - tb.height / 2.f - tb.top);
    return btn;
}

TextInput UI_Manager::makeInput(const std::string &id, const std::string &ph,
                                float x, float y, float w,
                                bool pwd, int maxLen)
{
    TextInput inp;
    inp.id = id;
    inp.password = pwd;
    inp.maxLength = maxLen;
    inp.box.setSize({w, 40.f});
    inp.box.setPosition(x, y);
    inp.box.setOutlineThickness(1.f);

    inp.placeholder.setFont(font);
    inp.placeholder.setString(ph);
    inp.placeholder.setCharacterSize(Theme::FONT_MD);
    inp.placeholder.setFillColor(Theme::TEXT_DARK);
    inp.placeholder.setPosition(x + 10.f, y + 8.f);

    inp.text.setFont(font);
    inp.text.setCharacterSize(Theme::FONT_MD);
    inp.text.setFillColor(Theme::TEXT);
    inp.text.setPosition(x + 10.f, y + 8.f);

    return inp;
}

Label UI_Manager::makeLabel(const std::string &txt, float x, float y,
                            unsigned size, sf::Color color)
{
    Label l;
    l.text.setFont(font);
    l.text.setString(txt);
    l.text.setCharacterSize(size);
    l.text.setFillColor(color);
    l.text.setPosition(x, y);
    return l;
}

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────
TextInput *UI_Manager::findInput(const std::string &id)
{
    for (auto &inp : inputs)
        if (inp.id == id)
            return &inp;
    return nullptr;
}

std::string UI_Manager::getInputVal(const std::string &id)
{
    auto *inp = findInput(id);
    return inp ? inp->value : "";
}

void UI_Manager::clearInputs()
{
    for (auto &inp : inputs)
        inp.value = "";
}

void UI_Manager::showToast(const std::string &msg, sf::Color c)
{
    toast.show(msg, c, font);
}

void UI_Manager::goTo(Screen s)
{
    currentScreen = s;
    inputs.clear();
    buttons.clear();
    labels.clear();
    tableRows.clear();
    tableHeaders.clear();
    selectedRow = -1;
    scrollOffset = 0.f;
    maxScroll = 0.f;
    formMessage = "";

    switch (s)
    {
    case Screen::STARTUP:
        buildStartup();
        break;
    case Screen::PATIENT_LOGIN:
        buildPatientLogin();
        break;
    case Screen::PATIENT_MENU:
        buildPatientMenu();
        break;
    case Screen::PATIENT_BOOK:
        buildPatientBook_Step1();
        break;
    case Screen::PATIENT_CANCEL:
        buildPatientCancel();
        break;
    case Screen::PATIENT_VIEW_APPTS:
        buildPatientViewAppts();
        break;
    case Screen::PATIENT_RECORDS:
        buildPatientRecords();
        break;
    case Screen::PATIENT_BILLS:
        buildPatientBills();
        break;
    case Screen::PATIENT_PAY:
        buildPatientPay();
        break;
    case Screen::PATIENT_TOPUP:
        buildPatientTopup();
        break;

    case Screen::DOCTOR_LOGIN:
        buildDoctorLogin();
        break;
    case Screen::DOCTOR_MENU:
        buildDoctorMenu();
        break;
    case Screen::DOCTOR_TODAY:
        buildDoctorToday();
        break;
    case Screen::DOCTOR_MARK_COMPLETE:
        buildDoctorMarkComplete();
        break;
    case Screen::DOCTOR_MARK_NOSHOW:
        buildDoctorMarkNoShow();
        break;
    case Screen::DOCTOR_PRESCRIPTION:
        buildDoctorPrescription();
        break;
    case Screen::DOCTOR_PATIENT_HISTORY:
        buildDoctorPatientHistory();
        break;

    case Screen::ADMIN_LOGIN:
        buildAdminLogin();
        break;
    case Screen::ADMIN_MENU:
        buildAdminMenu();
        break;
    case Screen::ADMIN_ADD_DOCTOR:
        buildAdminAddDoctor();
        break;
    case Screen::ADMIN_REMOVE_DOCTOR:
        buildAdminRemoveDoctor();
        break;
    case Screen::ADMIN_ADD_PATIENT:
        buildAdminAddPatient();
        break;
    case Screen::ADMIN_REMOVE_PATIENT:
        buildAdminRemovePatient();
        break;
    case Screen::ADMIN_ALL_PATIENTS:
        buildAdminAllPatients();
        break;
    case Screen::ADMIN_ALL_DOCTORS:
        buildAdminAllDoctors();
        break;
    case Screen::ADMIN_ALL_APPTS:
        buildAdminAllAppts();
        break;
    case Screen::ADMIN_UNPAID_BILLS:
        buildAdminUnpaidBills();
        break;
    case Screen::ADMIN_DISCHARGE:
        buildAdminDischarge();
        break;
    case Screen::ADMIN_SECURITY_LOG:
        buildAdminSecurityLog();
        break;
    case Screen::ADMIN_DAILY_REPORT:
        buildAdminDailyReport();
        break;
    default:
        break;
    }
}

void UI_Manager::logout()
{
    loginFailCount = 0;
    loggedPatient = Patient();
    loggedDoctor = Doctor();
    goTo(Screen::STARTUP);
}

// ─────────────────────────────────────────────────────────────────────────────
// Date / sort helpers
// ─────────────────────────────────────────────────────────────────────────────
std::string UI_Manager::getTodayDate()
{
    time_t now = time(0);
    struct tm *t = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%d-%m-%Y", t);
    return std::string(buf);
}

int UI_Manager::compareDates(const std::string &d1, const std::string &d2)
{
    int dy1, mo1, yr1, dy2, mo2, yr2;
    Validator::parseDate(d1.c_str(), dy1, mo1, yr1);
    Validator::parseDate(d2.c_str(), dy2, mo2, yr2);
    if (yr1 != yr2)
        return yr1 < yr2 ? -1 : 1;
    if (mo1 != mo2)
        return mo1 < mo2 ? -1 : 1;
    if (dy1 != dy2)
        return dy1 < dy2 ? -1 : 1;
    return 0;
}

bool UI_Manager::isOverdue(const std::string &dateStr)
{
    int d, m, y;
    Validator::parseDate(dateStr.c_str(), d, m, y);
    struct tm bt = {};
    bt.tm_mday = d;
    bt.tm_mon = m - 1;
    bt.tm_year = y - 1900;
    bt.tm_isdst = -1;
    time_t bt2 = mktime(&bt);
    return difftime(time(0), bt2) > 7.0 * 86400.0;
}

void UI_Manager::sortAppsByDateAsc(std::vector<Appointment> &v)
{
    for (int i = 0; i < (int)v.size() - 1; i++)
        for (int j = 0; j < (int)v.size() - i - 1; j++)
            if (compareDates(v[j].getDate(), v[j + 1].getDate()) > 0)
                std::swap(v[j], v[j + 1]);
}

void UI_Manager::sortAppsByDateDesc(std::vector<Appointment> &v)
{
    for (int i = 0; i < (int)v.size() - 1; i++)
        for (int j = 0; j < (int)v.size() - i - 1; j++)
            if (compareDates(v[j].getDate(), v[j + 1].getDate()) < 0)
                std::swap(v[j], v[j + 1]);
}

void UI_Manager::sortPrescsByDateDesc(std::vector<Prescription> &v)
{
    for (int i = 0; i < (int)v.size() - 1; i++)
        for (int j = 0; j < (int)v.size() - i - 1; j++)
            if (compareDates(v[j].getDate(), v[j + 1].getDate()) < 0)
                std::swap(v[j], v[j + 1]);
}

std::vector<Appointment> UI_Manager::getPatientAppointments(int patID)
{
    std::vector<Appointment> v;
    for (int i = 0; i < appointments.size(); i++)
        if (appointments.get(i).getPatientID() == patID)
            v.push_back(appointments.get(i));
    return v;
}

std::vector<Bill> UI_Manager::getPatientBills(int patID, bool unpaidOnly)
{
    std::vector<Bill> v;
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (b.getPatientID() == patID)
        {
            if (!unpaidOnly || Validator::strEq(b.getStatus(), "unpaid"))
                v.push_back(b);
        }
    }
    return v;
}

std::vector<Prescription> UI_Manager::getPatientPrescriptions(int patID, int docID)
{
    std::vector<Prescription> v;
    for (int i = 0; i < prescriptions.size(); i++)
    {
        const Prescription &p = prescriptions.get(i);
        if (p.getPatientID() == patID && (docID == -1 || p.getDoctorID() == docID))
            v.push_back(p);
    }
    return v;
}

void UI_Manager::drawRoundedRect(float x, float y, float w, float h,
                                 float /*r*/, sf::Color fill, sf::Color border)
{
    // SFML 2.x doesn't have rounded rect natively — use plain rect
    sf::RectangleShape rect({w, h});
    rect.setFillColor(fill);
    rect.setPosition(x, y);
    if (border.a > 0)
    {
        rect.setOutlineColor(border);
        rect.setOutlineThickness(1.f);
    }
    window.draw(rect);
}