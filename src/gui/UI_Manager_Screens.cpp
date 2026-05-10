// // =============================================================================
// // UI_Manager_Screens.cpp
// // Screen builders + button handlers for every role/menu.
// // =============================================================================
// #include "UI_Manager.hpp"
// #include <sstream>
// #include <iomanip>
// #include <ctime>
// #include <algorithm>

// // ─── Sidebar nav button helper ────────────────────────────────────────────────
// static void addNavItem(std::vector<Button>& btns, const sf::Font& font,
//                        const std::string& id, const std::string& label,
//                        float y, bool active = false) {
//     // SFML 3: Pass font to constructor
//     Button b(font);
//     b.id = id;
//     b.normalColor = active ? sf::Color(0x006688C8) : sf::Color::Transparent;
//     b.hoverColor  = sf::Color(0x004466B4);
//     b.shape.setSize({196.f, 40.f});
//     b.shape.setPosition({0.f, y});
//     b.shape.setFillColor(b.normalColor);

//     b.label.setString(label);
//     b.label.setCharacterSize(Theme::FONT_MD);
//     b.label.setFillColor(active ? Theme::ACCENT : sf::Color(0xAAAAAAFF));
//     b.label.setPosition({16.f, y + 10.f});
//     btns.push_back(b);
// }

// // ─── Top bar title helper ─────────────────────────────────────────────────────
// static void addTopBar(std::vector<Label>& labels, const sf::Font& font,
//                       const std::string& title, const std::string& subtitle = "") {
//     {
//         Label l(font);
//         l.text.setString(title);
//         l.text.setCharacterSize(Theme::FONT_LG);
//         l.text.setFillColor(Theme::TEXT);
//         l.text.setStyle(sf::Text::Bold);
//         l.text.setPosition({228.f, 15.f});
//         labels.push_back(l);
//     }
//     if (!subtitle.empty()) {
//         Label s(font);
//         s.text.setString(subtitle);
//         s.text.setCharacterSize(Theme::FONT_SM);
//         s.text.setFillColor(sf::Color(0xAAAAAAFF));
//         s.text.setPosition({228.f, 38.f});
//         labels.push_back(s);
//     }
// }

// // ═════════════════════════════════════════════════════════════════════════════
// // STARTUP
// // ═════════════════════════════════════════════════════════════════════════════
// void UI_Manager::buildStartup() {
//     labels.push_back(makeLabel("Welcome to", 350.f, 160.f, Theme::FONT_MD, sf::Color(0xAAAAAAFF)));
//     labels.push_back(makeLabel("MediCore HMS", 295.f, 190.f, Theme::FONT_XXL, Theme::ACCENT));
//     labels.push_back(makeLabel("Hospital Management System", 340.f, 250.f, Theme::FONT_MD, sf::Color(0xAAAAAAFF)));
//     labels.push_back(makeLabel("Login as:", 510.f, 310.f, Theme::FONT_LG, Theme::TEXT));

//     buttons.push_back(makeButton("login_patient", "Patient", 460.f, 360.f, 360.f, 52.f, sf::Color(0x005A80FF)));
//     buttons.push_back(makeButton("login_doctor", "Doctor", 460.f, 428.f, 360.f, 52.f, sf::Color(0x004E6EFF)));
//     buttons.push_back(makeButton("login_admin", "Admin", 460.f, 496.f, 360.f, 52.f, sf::Color(0x00425CFF)));
    
//     Button exitBtn = makeButton("exit_app", "Exit", 460.f, 564.f, 360.f, 44.f, sf::Color(0x3A1A1AFF));
//     exitBtn.hoverColor = Theme::DANGER;
//     buttons.push_back(exitBtn);
// }

// // ═════════════════════════════════════════════════════════════════════════════
// // LOGIN SCREENS
// // ═════════════════════════════════════════════════════════════════════════════
// static void buildLoginScreen(std::vector<Label>& labels,
//                              std::vector<TextInput>& inputs,
//                              std::vector<Button>& buttons,
//                              const sf::Font& font,
//                              const std::string& role) {
//     addTopBar(labels, font, role + " Login");

//     float cx = 440.f, y = 200.f;
//     {
//         Label l(font);
//         l.text.setString(role + " Portal");
//         l.text.setCharacterSize(Theme::FONT_XL);
//         l.text.setFillColor(Theme::ACCENT);
//         l.text.setStyle(sf::Text::Bold);
//         l.text.setPosition({cx, y});
//         labels.push_back(l);
//     }

//     auto makeInp = [&](const std::string& id, const std::string& ph, float iy, bool pwd = false) {
//         TextInput inp(font);
//         inp.id = id;
//         inp.password = pwd;
//         inp.maxLength = 50;
//         inp.box.setSize({400.f, 48.f});
//         inp.box.setPosition({cx, iy});
//         inp.box.setOutlineThickness(1.f);
//         inp.placeholder.setString(ph);
//         inp.placeholder.setCharacterSize(Theme::FONT_MD);
//         inp.placeholder.setFillColor(sf::Color(0x444444FF));
//         inp.placeholder.setPosition({cx + 12.f, iy + 13.f});
//         inp.text.setCharacterSize(Theme::FONT_MD);
//         inp.text.setFillColor(Theme::TEXT);
//         inp.text.setPosition({cx + 12.f, iy + 13.f});
//         inputs.push_back(inp);
//     };

//     makeInp("id",  role + " ID",  y + 70.f);
//     makeInp("pwd", "Password",    y + 136.f, true);

//     Button loginBtn(font);
//     loginBtn.id = "do_login";
//     loginBtn.normalColor = Theme::ACCENT;
//     loginBtn.hoverColor  = sf::Color(0x00B4E6FF);
//     loginBtn.shape.setSize({400.f, 48.f});
//     loginBtn.shape.setPosition({cx, y + 210.f});
//     loginBtn.shape.setFillColor(Theme::ACCENT);
//     loginBtn.label.setString("LOGIN");
//     loginBtn.label.setCharacterSize(Theme::FONT_MD);
//     loginBtn.label.setFillColor(sf::Color::White);
//     loginBtn.label.setStyle(sf::Text::Bold);
    
//     sf::FloatRect lb = loginBtn.label.getLocalBounds();
//     // SFML 3: Access size via .size property
//     loginBtn.label.setPosition({cx + 200.f - lb.size.x/2.f, y + 210.f + 14.f});
//     buttons.push_back(loginBtn);

//     Button backBtn(font);
//     backBtn.id = "back";
//     backBtn.normalColor = sf::Color(0x1E3246FF);
//     backBtn.hoverColor  = sf::Color(0x32506EFF);
//     backBtn.shape.setSize({400.f, 38.f});
//     backBtn.shape.setPosition({cx, y + 272.f});
//     backBtn.shape.setFillColor(backBtn.normalColor);
//     backBtn.label.setString("Back");
//     backBtn.label.setCharacterSize(Theme::FONT_MD);
//     backBtn.label.setFillColor(sf::Color(0xAAAAAAFF));
//     sf::FloatRect bb = backBtn.label.getLocalBounds();
//     backBtn.label.setPosition({cx + 200.f - bb.size.x/2.f, y + 280.f});
//     buttons.push_back(backBtn);
// }

// void UI_Manager::buildPatientLogin() { buildLoginScreen(labels, inputs, buttons, font, "Patient"); }
// void UI_Manager::buildDoctorLogin() { buildLoginScreen(labels, inputs, buttons, font, "Doctor"); }
// void UI_Manager::buildAdminLogin() { buildLoginScreen(labels, inputs, buttons, font, "Admin"); }

// // ═════════════════════════════════════════════════════════════════════════════
// // PATIENT MENU
// // ═════════════════════════════════════════════════════════════════════════════
// void UI_Manager::buildPatientMenu() {
//     char balBuf[32];
//     Validator::floatToStr(loggedPatient.getBalance(), balBuf, 32, 2);
//     std::string subtitle = std::string("Balance: PKR ") + balBuf;
//     addTopBar(labels, font, std::string("Welcome, ") + loggedPatient.getName(), subtitle);

//     struct Item { std::string id, label; sf::Color col; };
//     Item items[] = {
//         {"p_book",    "Book Appointment",      sf::Color(0x005A8CFF)},
//         {"p_cancel",  "Cancel Appointment",    sf::Color(0x004B78FF)},
//         {"p_vappts",  "View My Appointments",  sf::Color(0x005A8CFF)},
//         {"p_records", "View Medical Records",  sf::Color(0x004B78FF)},
//         {"p_bills",   "View My Bills",         sf::Color(0x005A8CFF)},
//         {"p_pay",     "Pay Bill",              sf::Color(0x004B78FF)},
//         {"p_topup",   "Top Up Balance",        sf::Color(0x00643CFF)},
//         {"logout",    "Logout",                sf::Color(0x501414FF)},
//     };

//     float bx = 240.f, by = 90.f;
//     float bw = 240.f, bh = 54.f, gap = 8.f;
//     for (int i = 0; i < 8; i++) {
//         float col = (float)(i % 3);
//         float row = (float)(i / 3);
//         buttons.push_back(makeButton(items[i].id, items[i].label,
//             bx + col * (bw + gap), by + 70.f + row * (bh + gap),
//             bw, bh, items[i].col));
//     }
// }

// // ═════════════════════════════════════════════════════════════════════════════
// // PATIENT — BOOK APPOINTMENT
// // ═════════════════════════════════════════════════════════════════════════════
// void UI_Manager::buildPatientBook_Step1() {
//     addTopBar(labels, font, "Book Appointment", "Search by specialization");

//     float cx = 240.f, y = 80.f;
//     labels.push_back(makeLabel("Specialization:", cx, y + 10.f, Theme::FONT_MD, sf::Color(0xAAAAAAFF)));
//     inputs.push_back(makeInput("spec", "e.g. Cardiology", cx + 170.f, y, 340.f));
//     buttons.push_back(makeButton("search_spec", "Search", cx + 530.f, y, 120.f, 40.f, Theme::ACCENT));
//     buttons.push_back(makeButton("back", "Back", cx, y + 60.f, 100.f, 36.f, sf::Color(0x283C50FF)));

//     tableHeaders = {"ID", "Name", "Specialization", "Fee (PKR)"};
//     for (int i = 0; i < doctors.size(); i++) {
//         const Doctor& d = doctors.get(i);
//         char feeBuf[32]; Validator::floatToStr(d.getFee(), feeBuf, 32, 2);
//         char idBuf[16];  Validator::intToStr(d.getID(), idBuf, 16);
//         TableRow r;
//         r.cells = {idBuf, d.getName(), d.getSpecialization(), feeBuf};
//         r.rowColor = Theme::TEXT;
//         tableRows.push_back(r);
//     }

//     labels.push_back(makeLabel("Select a doctor row then fill details below:", cx, 490.f, Theme::FONT_SM, sf::Color(0xAAAAAAFF)));
//     labels.push_back(makeLabel("Date (DD-MM-YYYY):", cx, 520.f, Theme::FONT_MD, sf::Color(0xAAAAAAFF)));
//     inputs.push_back(makeInput("date", "e.g. 20-06-2026", cx + 200.f, 512.f, 220.f));
//     buttons.push_back(makeButton("book_confirm", "Next: Pick Slot", cx + 440.f, 512.f, 200.f, 40.f, Theme::ACCENT));
// }

// void UI_Manager::buildPatientBook_Step2(int docID, const std::string& date) {
//     inputs.clear(); buttons.clear(); labels.clear(); tableRows.clear(); tableHeaders.clear();

//     addTopBar(labels, font, "Choose Time Slot", 
//               string("Dr. ") + string((doctors.findByID(docID) ? doctors.findByID(docID)->getName() : "")) + string(" | ") + string(date));

//     const char* slots[] = {"09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00"};
    
//     std::vector<std::string> taken;
//     for (int i = 0; i < appointments.size(); i++) {
//         const Appointment& ap = appointments.get(i);
//         if (ap.getDoctorID() == docID && Validator::strEq(ap.getDate(), date.c_str()) && !Validator::strEq(ap.getStatus(), "cancelled"))
//             taken.push_back(ap.getTimeSlot());
//     }

//     float sx = 280.f, sy = 120.f;
//     for (int i = 0; i < 8; i++) {
//         bool isTaken = false;
//         for (const auto& t : taken) if (t == slots[i]) { isTaken = true; break; }

//         float bx = sx + (i % 4) * 190.f;
//         float by = sy + (i / 4) * 90.f;
//         sf::Color c = isTaken ? sf::Color(0x3C1E1EFF) : sf::Color(0x005A8CFF);
        
//         Button b = makeButton(std::string("slot_") + slots[i],
//                               isTaken ? std::string(slots[i]) + "\nTaken" : slots[i],
//                               bx, by, 170.f, 68.f, c);
//         b.enabled = !isTaken;
//         if (isTaken) {
//             b.label.setFillColor(sf::Color(0x444444FF));
//             b.label.setStyle(sf::Text::Italic);
//         }
//         buttons.push_back(b);
//     }
//     buttons.push_back(makeButton("back", "Back", 280.f, 320.f, 100.f, 36.f, sf::Color(0x283C50FF)));
// }

// // ═════════════════════════════════════════════════════════════════════════════
// // DOCTOR MENU
// // ═════════════════════════════════════════════════════════════════════════════
// void UI_Manager::buildDoctorMenu() {
//     addTopBar(labels, font, string("Dr. ") + string(loggedDoctor.getName()), string(loggedDoctor.getSpecialization()));

//     struct Item { std::string id, label; sf::Color col; };
//     Item items[] = {
//         {"d_today",   "Today's Appointments",    sf::Color(0x005A8CFF)},
//         {"d_complete","Mark Complete",            sf::Color(0x006E46FF)},
//         {"d_noshow",  "Mark No-Show",             sf::Color(0x643C00FF)},
//         {"d_presc",   "Write Prescription",       sf::Color(0x004B78FF)},
//         {"d_history", "Patient History",          sf::Color(0x003C5AFF)},
//         {"logout",    "Logout",                   sf::Color(0x501414FF)},
//     };

//     float bx = 260.f, by = 160.f, bw = 260.f, bh = 60.f, gap = 12.f;
//     for (int i = 0; i < 6; i++) {
//         float col = (float)(i % 3), row = (float)(i / 3);
//         buttons.push_back(makeButton(items[i].id, items[i].label,
//             bx + col*(bw+gap), by + row*(bh+gap), bw, bh, items[i].col));
//     }
// }

// // ═════════════════════════════════════════════════════════════════════════════
// // ADMIN MENU
// // ═════════════════════════════════════════════════════════════════════════════
// void UI_Manager::buildAdminMenu() {
//     addTopBar(labels, font, "Admin Panel", "MediCore");

//     struct Item { std::string id, label; sf::Color col; };
//     Item items[] = {
//         {"a_adddoc",   "Add Doctor",           sf::Color(0x005A8CFF)},
//         {"a_remdoc",   "Remove Doctor",         sf::Color(0x642828FF)},
//         {"a_addpat",   "Add Patient",           sf::Color(0x005A8CFF)},
//         {"a_rempat",   "Remove Patient",        sf::Color(0x642828FF)},
//         {"a_allpat",   "All Patients",          sf::Color(0x004B78FF)},
//         {"a_alldoc",   "All Doctors",           sf::Color(0x004B78FF)},
//         {"a_allappts", "All Appointments",      sf::Color(0x004B78FF)},
//         {"a_unpaid",   "Unpaid Bills",          sf::Color(0x644600FF)},
//         {"a_discharge","Discharge Patient",     sf::Color(0x003C28FF)},
//         {"a_seclog",   "Security Log",          sf::Color(0x3C1450FF)},
//         {"a_report",   "Daily Report",          sf::Color(0x00503CFF)},
//         {"logout",     "Logout",                sf::Color(0x501414FF)},
//     };

//     float bx = 228.f, by = 80.f, bw = 240.f, bh = 52.f, gap = 8.f;
//     for (int i = 0; i < 12; i++) {
//         float col = (float)(i % 4), row = (float)(i / 4);
//         buttons.push_back(makeButton(items[i].id, items[i].label,
//             bx + col*(bw+gap), by + 70.f + row*(bh+gap), bw, bh, items[i].col));
//     }
// }

// void UI_Manager::buildAdminAddDoctor() {
//     addTopBar(labels, font, "Add Doctor");
//     float cx = 360.f, y = 80.f, gap = 56.f;
//     struct F { std::string id, ph; bool pwd; int max; };
//     F fields[] = {
//         {"name",    "Doctor Name (max 50)",      false, 50},
//         {"spec",    "Specialization (max 50)",   false, 50},
//         {"contact", "Contact (11 digits)",        false, 11},
//         {"pwd",     "Password (min 6 chars)",     true,  50},
//         {"fee",     "Consultation Fee (PKR)",     false, 20},
//     };
//     for (int i = 0; i < 5; i++) {
//         labels.push_back(makeLabel(fields[i].ph + ":", cx - 20.f, y + 70.f + i*gap + 8.f, Theme::FONT_SM, sf::Color(0xAAAAAAFF)));
//         inputs.push_back(makeInput(fields[i].id, fields[i].ph, cx + 220.f, y + 70.f + i*gap, 360.f, fields[i].pwd, fields[i].max));
//     }
//     buttons.push_back(makeButton("do_add_doctor", "Add Doctor", cx + 220.f, y + 70.f + 5*gap, 200.f, 48.f, Theme::SUCCESS));
//     buttons.push_back(makeButton("back", "Back", cx + 440.f, y + 70.f + 5*gap, 100.f, 48.f, sf::Color(0x283C50FF)));
// }