#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>

#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "HospitalException.hpp"

// ─── Palette & layout constants ───────────────────────────────────────────────
namespace Theme
{
    // Dark clinical palette
    inline const sf::Color BG = sf::Color(0x0D, 0x1B, 0x2A);    // deep navy
    inline const sf::Color PANEL = sf::Color(0x11, 0x25, 0x3A); // slightly lighter
    inline const sf::Color CARD = sf::Color(0x16, 0x2D, 0x47);
    inline const sf::Color BORDER = sf::Color(0x1E, 0x4D, 0x72);
    inline const sf::Color ACCENT = sf::Color(0x00, 0xC2, 0xFF);  // cyan
    inline const sf::Color ACCENT2 = sf::Color(0x00, 0xFF, 0xC2); // mint
    inline const sf::Color DANGER = sf::Color(0xFF, 0x4D, 0x6D);
    inline const sf::Color SUCCESS = sf::Color(0x00, 0xE5, 0x96);
    inline const sf::Color WARNING = sf::Color(0xFF, 0xBE, 0x0B);
    inline const sf::Color TEXT = sf::Color(0xE8, 0xF4, 0xFF);
    inline const sf::Color TEXT_MUTED = sf::Color(0x7A, 0xA5, 0xC8);
    inline const sf::Color TEXT_DARK = sf::Color(0x3A, 0x6A, 0x90);
    inline const sf::Color BTN_HOVER = sf::Color(0x00, 0x8C, 0xBF);
    inline const sf::Color INPUT_BG = sf::Color(0x0A, 0x14, 0x1F);
    inline const sf::Color INPUT_FOCUS = sf::Color(0x1A, 0x3A, 0x55);
    inline const sf::Color SCROLLBAR = sf::Color(0x1E, 0x4D, 0x72);

    inline const float RADIUS = 8.f;
    inline const float PADDING = 20.f;
    inline const unsigned FONT_SM = 13;
    inline const unsigned FONT_MD = 16;
    inline const unsigned FONT_LG = 20;
    inline const unsigned FONT_XL = 28;
    inline const unsigned FONT_XXL = 38;
}

// ─── Reusable widget structs ──────────────────────────────────────────────────

struct Button
{
    sf::RectangleShape shape;
    sf::Text label;
    sf::Color normalColor;
    sf::Color hoverColor;
    bool hovered = false;
    bool enabled = true;
    std::string id; // logical identifier

    void setPosition(float x, float y);
    void setSize(float w, float h);
    void draw(sf::RenderWindow &w);
    bool contains(sf::Vector2f p) const;
    void update(sf::Vector2f mouse);
};

struct TextInput
{
    sf::RectangleShape box;
    sf::Text text;
    sf::Text placeholder;
    std::string value;
    bool focused = false;
    bool password = false; // mask with *
    int maxLength = 50;
    std::string id;

    void draw(sf::RenderWindow &w);
    bool contains(sf::Vector2f p) const;
    void handleTextEntered(int c);
    void handleKey(sf::Keyboard::Key k);
};

struct Label
{
    sf::Text text;
    void draw(sf::RenderWindow &w) { w.draw(text); }
};

struct Toast
{
    sf::RectangleShape box;
    sf::Text msg;
    sf::Clock timer;
    float duration = 2.5f;
    bool active = false;
    sf::Color color;

    void show(const std::string &message, sf::Color c, sf::Font &font);
    void draw(sf::RenderWindow &w);
    bool isDone() const;
};

struct TableRow
{
    std::vector<std::string> cells;
    sf::Color rowColor;
    bool alternate = false;
};

// ─── Screen enum ─────────────────────────────────────────────────────────────
enum class Screen
{
    STARTUP,

    // Patient
    PATIENT_LOGIN,
    PATIENT_MENU,
    PATIENT_BOOK,
    PATIENT_CANCEL,
    PATIENT_VIEW_APPTS,
    PATIENT_RECORDS,
    PATIENT_BILLS,
    PATIENT_PAY,
    PATIENT_TOPUP,

    // Doctor
    DOCTOR_LOGIN,
    DOCTOR_MENU,
    DOCTOR_TODAY,
    DOCTOR_MARK_COMPLETE,
    DOCTOR_MARK_NOSHOW,
    DOCTOR_PRESCRIPTION,
    DOCTOR_PATIENT_HISTORY,

    // Admin
    ADMIN_LOGIN,
    ADMIN_MENU,
    ADMIN_ADD_DOCTOR,
    ADMIN_REMOVE_DOCTOR,
    ADMIN_ADD_PATIENT,
    ADMIN_REMOVE_PATIENT,
    ADMIN_ALL_PATIENTS,
    ADMIN_ALL_DOCTORS,
    ADMIN_ALL_APPTS,
    ADMIN_UNPAID_BILLS,
    ADMIN_DISCHARGE,
    ADMIN_SECURITY_LOG,
    ADMIN_DAILY_REPORT
};

// ─── Main UI Manager class ────────────────────────────────────────────────────
class UI_Manager
{
public:
    UI_Manager(Storage<Patient> &patients,
               Storage<Doctor> &doctors,
               Admin &admin,
               Storage<Appointment> &appointments,
               Storage<Bill> &bills,
               Storage<Prescription> &prescriptions);

    void run(); // main loop

private:
    // ── Window & rendering ───────────────────────────────────────────────────
    sf::RenderWindow window;
    sf::Font font;
    Toast toast;

    // ── Data references ──────────────────────────────────────────────────────
    Storage<Patient> &patients;
    Storage<Doctor> &doctors;
    Admin &admin;
    Storage<Appointment> &appointments;
    Storage<Bill> &bills;
    Storage<Prescription> &prescriptions;

    // ── State ────────────────────────────────────────────────────────────────
    Screen currentScreen = Screen::STARTUP;
    Patient loggedPatient;
    Doctor loggedDoctor;
    int loginFailCount = 0;

    // ── Transient form state (reused across screens) ──────────────────────────
    std::vector<TextInput> inputs;
    std::vector<Button> buttons;
    std::vector<Label> labels;
    std::vector<TableRow> tableRows;
    std::vector<std::string> tableHeaders;
    std::string formMessage;
    sf::Color formMsgColor = Theme::DANGER;
    int selectedRow = -1;

    // Doctor booking sub-state
    std::vector<Doctor> matchedDoctors;
    Doctor selectedDoctor;
    std::vector<std::string> availableSlots;
    std::string bookingDate;

    // scrolling
    float scrollOffset = 0.f;
    float maxScroll = 0.f;

    // ── Screen builders ──────────────────────────────────────────────────────
    void buildStartup();
    void buildPatientLogin();
    void buildPatientMenu();
    void buildPatientBook_Step1();
    void buildPatientBook_Step2(int docID, const std::string &date);
    void buildPatientCancel();
    void buildPatientViewAppts();
    void buildPatientRecords();
    void buildPatientBills();
    void buildPatientPay();
    void buildPatientTopup();

    void buildDoctorLogin();
    void buildDoctorMenu();
    void buildDoctorToday();
    void buildDoctorMarkComplete();
    void buildDoctorMarkNoShow();
    void buildDoctorPrescription();
    void buildDoctorPatientHistory();

    void buildAdminLogin();
    void buildAdminMenu();
    void buildAdminAddDoctor();
    void buildAdminRemoveDoctor();
    void buildAdminAddPatient();
    void buildAdminRemovePatient();
    void buildAdminAllPatients();
    void buildAdminAllDoctors();
    void buildAdminAllAppts();
    void buildAdminUnpaidBills();
    void buildAdminDischarge();
    void buildAdminSecurityLog();
    void buildAdminDailyReport();

    // ── Event handlers ───────────────────────────────────────────────────────
    void handleEvent(const sf::Event &e);
    void handleButtonClick(const std::string &id);
    void handleStartupClick(const std::string &id);
    void handleLoginClick(const std::string &id, Screen loginScreen);
    void handlePatientMenuClick(const std::string &id);
    void handleDoctorMenuClick(const std::string &id);
    void handleAdminMenuClick(const std::string &id);
    void handleFormAction(const std::string &id);

    // ── Drawing ──────────────────────────────────────────────────────────────
    void draw();
    void drawBackground();
    void drawSidebar();
    void drawTopBar(const std::string &title, const std::string &subtitle = "");
    void drawTable(float x, float y, float w, float rowH = 36.f);
    void drawForm(float x, float y, float w);
    void drawScrollbar(float x, float y, float h);

    // ── Widget factories ─────────────────────────────────────────────────────
    Button makeButton(const std::string &id, const std::string &lbl,
                      float x, float y, float w, float h,
                      sf::Color fill = Theme::ACCENT);
    TextInput makeInput(const std::string &id, const std::string &placeholder,
                        float x, float y, float w,
                        bool password = false, int maxLen = 50);
    Label makeLabel(const std::string &txt, float x, float y,
                    unsigned size = Theme::FONT_MD,
                    sf::Color color = Theme::TEXT);

    // ── Logic helpers ────────────────────────────────────────────────────────
    TextInput *findInput(const std::string &id);
    std::string getInputVal(const std::string &id);
    void clearInputs();
    void showToast(const std::string &msg, sf::Color c = Theme::DANGER);
    void goTo(Screen s);
    void logout();

    std::string getTodayDate();
    int compareDates(const std::string &d1, const std::string &d2);
    bool isOverdue(const std::string &date);

    // Sorting (manual, no std::sort for non-SFML data)
    void sortAppsByDateAsc(std::vector<Appointment> &v);
    void sortAppsByDateDesc(std::vector<Appointment> &v);
    void sortPrescsByDateDesc(std::vector<Prescription> &v);

    std::vector<Appointment> getPatientAppointments(int patientID);
    std::vector<Bill> getPatientBills(int patientID, bool unpaidOnly = false);
    std::vector<Prescription> getPatientPrescriptions(int patientID, int doctorID = -1);

    void drawRoundedRect(float x, float y, float w, float h,
                         float r, sf::Color fill, sf::Color border = sf::Color::Transparent);
    void drawPulsingAccent(float x, float y, float w);
};

#endif // UI_MANAGER_H