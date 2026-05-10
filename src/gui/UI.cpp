// #ifndef UI_MANAGER_H
// #define UI_MANAGER_H

// #include <SFML/Graphics.hpp>
// #include <string>
// #include <vector>
// #include <functional>
// #include <optional> // Required for SFML 3 pollEvent

// #include "Storage.hpp"
// #include "Patient.hpp"
// #include "Doctor.hpp"
// #include "Admin.hpp"
// #include "Appointment.hpp"
// #include "Bill.hpp"
// #include "Prescription.hpp"
// #include "FileHandler.hpp"
// #include "Validator.hpp"
// #include "HospitalException.hpp"


// namespace Theme
// {
//     // SFML 3: Use 32-bit RGBA hex format
//     inline const sf::Color BG = sf::Color(0x0D1B2AFF);    
//     inline const sf::Color PANEL = sf::Color(0x11253AFF); 
//     inline const sf::Color CARD = sf::Color(0x162D47FF);
//     inline const sf::Color BORDER = sf::Color(0x1E4D72FF);
//     inline const sf::Color ACCENT = sf::Color(0x00C2FFFF);  
//     inline const sf::Color ACCENT2 = sf::Color(0x00FFC2FF); 
//     inline const sf::Color DANGER = sf::Color(0xFF4D6DFF);
//     inline const sf::Color SUCCESS = sf::Color(0x00E596FF);
//     inline const sf::Color TEXT = sf::Color(0xE8F4FFFF);
//     // ... constants remain floats/unsigned
// // SFML 3 hex colors (RGBA format)
//     inline const sf::Color WARNING = sf::Color(0xFFBE0BFF);
//     inline const sf::Color TEXT_MUTED = sf::Color(0x7AA5C8FF);
//     inline const sf::Color TEXT_DARK = sf::Color(0x3A6A90FF);
//     inline const sf::Color BTN_HOVER = sf::Color(0x008CBFFF);
//     inline const sf::Color INPUT_BG = sf::Color(0x0A141FFF);
//     inline const sf::Color INPUT_FOCUS = sf::Color(0x1A3A55FF);
//     inline const sf::Color SCROLLBAR = sf::Color(0x1E4D72FF);


//     inline const float RADIUS = 8.f;
//     inline const unsigned FONT_MD = 16;

//     inline const float PADDING = 20.f;
//     inline const unsigned FONT_SM = 13;
//     inline const unsigned FONT_LG = 20;
//     inline const unsigned FONT_XL = 28;
//     inline const unsigned FONT_XXL = 38;
// }

// struct Button {
//     sf::RectangleShape shape;
//     sf::Text label; // SFML 3: No default constructor
//     sf::Color normalColor;
//     sf::Color hoverColor;
//     bool hovered = false;
//     bool enabled = true;
//     std::string id;

//     // Constructor to pass font reference
//     Button(const sf::Font& font) : label(font) {}

//     void setPosition(float x, float y);
//     void setSize(float w, float h);
//     void draw(sf::RenderWindow &w);
//     bool contains(sf::Vector2f p) const;
//     void update(sf::Vector2f mouse);
// };

// struct TextInput {
//     sf::RectangleShape box;
//     sf::Text text;        // Requires font
//     sf::Text placeholder; // Requires font
//     std::string value;
//     bool focused = false;
//     bool password = false; 
//     int maxLength = 50;
//     std::string id;

//     // Constructor to pass font reference
//     TextInput(const sf::Font& font) : text(font), placeholder(font) {}

//     void draw(sf::RenderWindow &w);
//     bool contains(sf::Vector2f p) const;
//     void handleTextEntered(char32_t unicode); // Updated to char32_t for SFML 3
//     void handleKey(sf::Keyboard::Key k);
// };

// struct Label
// {
//     sf::Text text;
//     void draw(sf::RenderWindow &w) { w.draw(text); }
// };

// struct Toast {
//     sf::RectangleShape box;
//     sf::Text msg; // Requires font
//     sf::Clock timer;
//     float duration = 2.5f;
//     bool active = false;
//     sf::Color color;

//     // Constructor to pass font reference
//     Toast(const sf::Font& font) : msg(font) {}

//     void show(const std::string &message, sf::Color c); // Removed font param
//     void draw(sf::RenderWindow &w);
//     bool isDone() const;
// };

// struct TableRow
// {
//     std::vector<std::string> cells;
//     sf::Color rowColor = sf::Color::Transparent;
//     bool alternate = false;
// };

// enum class Screen
// {
//     STARTUP, PATIENT_LOGIN, PATIENT_MENU, PATIENT_BOOK, PATIENT_CANCEL,
//     PATIENT_VIEW_APPTS, PATIENT_RECORDS, PATIENT_BILLS, PATIENT_PAY, PATIENT_TOPUP,
//     DOCTOR_LOGIN, DOCTOR_MENU, DOCTOR_TODAY, DOCTOR_MARK_COMPLETE, DOCTOR_MARK_NOSHOW,
//     DOCTOR_PRESCRIPTION, DOCTOR_PATIENT_HISTORY,
//     ADMIN_LOGIN, ADMIN_MENU, ADMIN_ADD_DOCTOR, ADMIN_REMOVE_DOCTOR, ADMIN_ADD_PATIENT,
//     ADMIN_REMOVE_PATIENT, ADMIN_ALL_PATIENTS, ADMIN_ALL_DOCTORS, ADMIN_ALL_APPTS,
//     ADMIN_UNPAID_BILLS, ADMIN_DISCHARGE, ADMIN_SECURITY_LOG, ADMIN_DAILY_REPORT
// };

// class UI_Manager
// {
// public:
//     UI_Manager(Storage<Patient> &patients, Storage<Doctor> &doctors, Admin &admin,
//                Storage<Appointment> &appointments, Storage<Bill> &bills,
//                Storage<Prescription> &prescriptions);

//     void run();

// private:
//     sf::RenderWindow window;
//     sf::Font font;
//     Toast toast;

//     Storage<Patient> &patients;
//     Storage<Doctor> &doctors;
//     Admin &admin;
//     Storage<Appointment> &appointments;
//     Storage<Bill> &bills;
//     Storage<Prescription> &prescriptions;

//     Screen currentScreen = Screen::STARTUP;
//     Patient loggedPatient;
//     Doctor loggedDoctor;
//     int loginFailCount = 0;

//     std::vector<TextInput> inputs;
//     std::vector<Button> buttons;
//     std::vector<Label> labels;
//     std::vector<TableRow> tableRows;
//     std::vector<std::string> tableHeaders;
//     std::string formMessage;
//     sf::Color formMsgColor = Theme::DANGER;
//     int selectedRow = -1;

//     std::vector<Doctor> matchedDoctors;
//     Doctor selectedDoctor;
//     std::vector<std::string> availableSlots;
//     std::string bookingDate;

//     float scrollOffset = 0.f;
//     float maxScroll = 0.f;

//     void buildStartup(); void buildPatientLogin(); void buildPatientMenu();
//     void buildPatientBook_Step1(); void buildPatientBook_Step2(int docID, const std::string &date);
//     void buildPatientCancel(); void buildPatientViewAppts(); void buildPatientRecords();
//     void buildPatientBills(); void buildPatientPay(); void buildPatientTopup();
//     void buildDoctorLogin(); void buildDoctorMenu(); void buildDoctorToday();
//     void buildDoctorMarkComplete(); void buildDoctorMarkNoShow();
//     void buildDoctorPrescription(); void buildDoctorPatientHistory();
//     void buildAdminLogin(); void buildAdminMenu(); void buildAdminAddDoctor();
//     void buildAdminRemoveDoctor(); void buildAdminAddPatient(); void buildAdminRemovePatient();
//     void buildAdminAllPatients(); void buildAdminAllDoctors(); void buildAdminAllAppts();
//     void buildAdminUnpaidBills(); void buildAdminDischarge(); void buildAdminSecurityLog();
//     void buildAdminDailyReport();

//     void handleEvent(const sf::Event &e);
//     void handleButtonClick(const std::string &id);
//     void draw();
//     void drawBackground();
//     void drawTable(float x, float y, float w, float rowH = 36.f);

//     Button makeButton(const std::string &id, const std::string &lbl, float x, float y, float w, float h, sf::Color fill = Theme::ACCENT);
//     TextInput makeInput(const std::string &id, const std::string &placeholder, float x, float y, float w, bool password = false, int maxLen = 50);
//     Label makeLabel(const std::string &txt, float x, float y, unsigned size = Theme::FONT_MD, sf::Color color = Theme::TEXT);

//     TextInput *findInput(const std::string &id);
//     std::string getInputVal(const std::string &id);
//     void clearInputs();
//     void showToast(const std::string &msg, sf::Color c = Theme::DANGER);
//     void goTo(Screen s);
//     void logout();

//     std::string getTodayDate();
//     int compareDates(const std::string &d1, const std::string &d2);
//     bool isOverdue(const std::string &date);

//     void sortAppsByDateAsc(std::vector<Appointment> &v);
//     void sortAppsByDateDesc(std::vector<Appointment> &v);
//     void sortPrescsByDateDesc(std::vector<Prescription> &v);

//     std::vector<Appointment> getPatientAppointments(int patientID);
//     std::vector<Bill> getPatientBills(int patientID, bool unpaidOnly = false);
//     std::vector<Prescription> getPatientPrescriptions(int patientID, int doctorID = -1);

//     void drawRoundedRect(float x, float y, float w, float h, float r, sf::Color fill, sf::Color border = sf::Color::Transparent);
// };

// #endif