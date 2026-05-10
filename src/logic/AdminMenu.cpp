// =============================================================================
// AdminMenu.cpp
// =============================================================================
#include "AdminMenu.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "HospitalException.hpp"
#include <iostream>
#include <ctime>

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::getTodayDate(char *buf)
{
    time_t now = time(0);
    struct tm *t = localtime(&now);
    strftime(buf, 11, "%d-%m-%Y", t);
}

int AdminMenu::compareDates(const char *d1, const char *d2)
{
    int day1, mon1, yr1, day2, mon2, yr2;
    Validator::parseDate(d1, day1, mon1, yr1);
    Validator::parseDate(d2, day2, mon2, yr2);
    if (yr1 != yr2)
        return yr1 < yr2 ? -1 : 1;
    if (mon1 != mon2)
        return mon1 < mon2 ? -1 : 1;
    if (day1 != day2)
        return day1 < day2 ? -1 : 1;
    return 0;
}

void AdminMenu::sortAppointmentsByDateDesc(Appointment *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (compareDates(arr[j].getDate(), arr[j + 1].getDate()) < 0)
            {
                Appointment tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

bool AdminMenu::isOverdue(const char *dateStr)
{
    int day, mon, yr;
    Validator::parseDate(dateStr, day, mon, yr);

    struct tm billTm = {0};
    billTm.tm_mday = day;
    billTm.tm_mon = mon - 1;
    billTm.tm_year = yr - 1900;
    time_t billTime = mktime(&billTm);

    time_t now = time(0);
    double diff = difftime(now, billTime);
    return diff > 7.0 * 24.0 * 3600.0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Menu loop
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::run(Admin &admin,
                    Storage<Patient> &patients,
                    Storage<Doctor> &doctors,
                    Storage<Appointment> &appointments,
                    Storage<Bill> &bills,
                    Storage<Prescription> &prescriptions)
{
    char choice[8];
    while (true)
    {
        admin.displayMenu();
        std::cin.getline(choice, 8);
        Validator::trim(choice);

        if (Validator::strEq(choice, "1"))
            addDoctor(doctors);
        else if (Validator::strEq(choice, "2"))
            removeDoctor(doctors, appointments);
        else if (Validator::strEq(choice, "3"))
            addPatient(patients);
        else if (Validator::strEq(choice, "4"))
            removePatient(patients, appointments, bills, prescriptions);
        else if (Validator::strEq(choice, "5"))
            viewAllPatients(patients, bills);
        else if (Validator::strEq(choice, "6"))
            viewAllDoctors(doctors);
        else if (Validator::strEq(choice, "7"))
            viewAllAppointments(appointments, patients, doctors);
        else if (Validator::strEq(choice, "8"))
            viewUnpaidBills(bills, patients);
        else if (Validator::strEq(choice, "9"))
            dischargePatient(patients, appointments, bills, prescriptions);
        else if (Validator::strEq(choice, "10"))
            viewSecurityLog();
        else if (Validator::strEq(choice, "11"))
            generateDailyReport(appointments, bills, patients, doctors);
        else if (Validator::strEq(choice, "12"))
        {
            std::cout << "Logged out successfully.\n";
            break;
        }
        else
        {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 1. Add Doctor
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::addDoctor(Storage<Doctor> &doctors)
{
    char name[51], spec[51], contact[12], password[51], feeStr[32];

    std::cout << "Enter doctor name (max 50 chars): ";
    std::cin.getline(name, 51);
    Validator::trim(name);

    std::cout << "Enter specialization (max 50 chars): ";
    std::cin.getline(spec, 51);
    Validator::trim(spec);

    // Validate contact (11 digits)
    bool valid = false;
    while (!valid)
    {
        std::cout << "Enter contact (11 digits): ";
        std::cin.getline(contact, 12);
        Validator::trim(contact);
        if (Validator::isValidContact(contact))
        {
            valid = true;
        }
        else
            std::cout << "Invalid contact. Must be exactly 11 numeric digits.\n";
    }

    // Validate password (min 6 chars)
    valid = false;
    while (!valid)
    {
        std::cout << "Enter password (min 6 chars): ";
        std::cin.getline(password, 51);
        Validator::trim(password);
        if (Validator::isValidPassword(password))
        {
            valid = true;
        }
        else
            std::cout << "Password must be at least 6 characters.\n";
    }

    // Validate fee (positive float)
    float fee = 0.0f;
    valid = false;
    while (!valid)
    {
        std::cout << "Enter consultation fee: ";
        std::cin.getline(feeStr, 32);
        Validator::trim(feeStr);
        if (Validator::isPositiveFloat(feeStr))
        {
            fee = Validator::strToFloat(feeStr);
            valid = true;
        }
        else
        {
            std::cout << "Fee must be a positive number.\n";
        }
    }

    int newID = FileHandler::getNextDoctorID(doctors);
    Doctor d(newID, name, spec, contact, password, fee);
    doctors.add(d);
    FileHandler::appendDoctor(d);

    char idBuf[16];
    Validator::intToStr(newID, idBuf, 16);
    std::cout << "Doctor added successfully. ID: " << idBuf << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 2. Remove Doctor
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::removeDoctor(Storage<Doctor> &doctors,
                             const Storage<Appointment> &appointments)
{
    std::cout << "ID  | Name                 | Specialization    | Fee\n";
    std::cout << "------------------------------------------------------\n";
    for (int i = 0; i < doctors.size(); i++)
    {
        char feeBuf[32];
        Validator::floatToStr(doctors.get(i).getFee(), feeBuf, 32, 2);
        std::cout << doctors.get(i).getID() << " | "
                  << doctors.get(i).getName() << " | "
                  << doctors.get(i).getSpecialization() << " | PKR "
                  << feeBuf << "\n";
    }

    char idStr[16];
    std::cout << "Enter Doctor ID to remove: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int docID = Validator::strToInt(idStr);

    // Check for pending appointments
    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getDoctorID() == docID &&
            Validator::strEq(appointments.get(i).getStatus(), "pending"))
        {
            std::cout << "Cannot remove doctor with pending appointments. "
                         "Cancel or reassign them first.\n";
            return;
        }
    }

    FileHandler::deleteDoctor(docID, doctors);
    std::cout << "Doctor removed.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 3. Add Patient
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::addPatient(Storage<Patient> &patients)
{
    char name[51], ageStr[8], gender[3], contact[12], password[51], balStr[32];

    std::cout << "Enter patient name (max 50 chars): ";
    std::cin.getline(name, 51);
    Validator::trim(name);

    bool valid = false;
    while (!valid)
    {
        std::cout << "Enter age: ";
        std::cin.getline(ageStr, 8);
        Validator::trim(ageStr);
        if (Validator::isValidAge(ageStr))
            valid = true;
        else
            std::cout << "Invalid age. Must be a positive integer.\n";
    }

    valid = false;
    while (!valid)
    {
        std::cout << "Enter gender (M/F): ";
        std::cin.getline(gender, 3);
        Validator::trim(gender);
        if (Validator::isValidGender(gender))
            valid = true;
        else
            std::cout << "Invalid gender. Enter M or F.\n";
    }

    valid = false;
    while (!valid)
    {
        std::cout << "Enter contact (11 digits): ";
        std::cin.getline(contact, 12);
        Validator::trim(contact);
        if (Validator::isValidContact(contact))
            valid = true;
        else
            std::cout << "Invalid contact. Must be exactly 11 numeric digits.\n";
    }

    valid = false;
    while (!valid)
    {
        std::cout << "Enter password (min 6 chars): ";
        std::cin.getline(password, 51);
        Validator::trim(password);
        if (Validator::isValidPassword(password))
            valid = true;
        else
            std::cout << "Password must be at least 6 characters.\n";
    }

    float balance = 0.0f;
    valid = false;
    while (!valid)
    {
        std::cout << "Enter initial balance: ";
        std::cin.getline(balStr, 32);
        Validator::trim(balStr);
        if (Validator::isPositiveFloat(balStr))
        {
            balance = Validator::strToFloat(balStr);
            valid = true;
        }
        else
        {
            std::cout << "Balance must be a positive number.\n";
        }
    }

    int newID = FileHandler::getNextPatientID(patients);
    int age = Validator::strToInt(ageStr);
    Patient p(newID, name, age, gender, contact, password, balance);
    patients.add(p);
    FileHandler::appendPatient(p);

    char idBuf[16];
    Validator::intToStr(newID, idBuf, 16);
    std::cout << "Patient added successfully. ID: " << idBuf << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 4. Remove Patient
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::removePatient(Storage<Patient> &patients,
                              Storage<Appointment> &appointments,
                              Storage<Bill> &bills,
                              Storage<Prescription> &prescriptions)
{
    std::cout << "ID  | Name                 | Age | Gender | Contact      | Balance\n";
    for (int i = 0; i < patients.size(); i++)
    {
        char balBuf[32];
        Validator::floatToStr(patients.get(i).getBalance(), balBuf, 32, 2);
        std::cout << patients.get(i).getID() << " | "
                  << patients.get(i).getName() << " | "
                  << patients.get(i).getAge() << "  | "
                  << patients.get(i).getGender() << "      | "
                  << patients.get(i).getContact() << " | PKR "
                  << balBuf << "\n";
    }

    char idStr[16];
    std::cout << "Enter Patient ID to remove: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int patID = Validator::strToInt(idStr);

    // Check pending appointments
    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getPatientID() == patID &&
            Validator::strEq(appointments.get(i).getStatus(), "pending"))
        {
            std::cout << "Cannot remove patient with pending appointments.\n";
            return;
        }
    }

    // Check unpaid bills
    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.get(i).getPatientID() == patID &&
            Validator::strEq(bills.get(i).getStatus(), "unpaid"))
        {
            std::cout << "Cannot remove patient with unpaid bills.\n";
            return;
        }
    }

    FileHandler::deletePatient(patID, patients);
    FileHandler::cascadeDeletePatient(patID, appointments, bills, prescriptions);
    std::cout << "Patient removed successfully.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 5. View All Patients
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::viewAllPatients(const Storage<Patient> &patients,
                                const Storage<Bill> &bills)
{
    std::cout << "ID  | Name                 | Age | Gender | Contact      | Balance    | Unpaid Bills\n";
    std::cout << "-----------------------------------------------------------------------------------\n";
    for (int i = 0; i < patients.size(); i++)
    {
        const Patient &p = patients.get(i);
        int unpaidCount = 0;
        for (int j = 0; j < bills.size(); j++)
        {
            if (bills.get(j).getPatientID() == p.getID() &&
                Validator::strEq(bills.get(j).getStatus(), "unpaid"))
            {
                unpaidCount++;
            }
        }
        char balBuf[32];
        Validator::floatToStr(p.getBalance(), balBuf, 32, 2);
        std::cout << p.getID() << " | "
                  << p.getName() << " | "
                  << p.getAge() << "  | "
                  << p.getGender() << "      | "
                  << p.getContact() << " | PKR "
                  << balBuf << " | "
                  << unpaidCount << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 6. View All Doctors
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::viewAllDoctors(const Storage<Doctor> &doctors)
{
    std::cout << "ID  | Name                 | Specialization    | Contact      | Fee\n";
    std::cout << "-------------------------------------------------------------------\n";
    for (int i = 0; i < doctors.size(); i++)
    {
        const Doctor &d = doctors.get(i);
        char feeBuf[32];
        Validator::floatToStr(d.getFee(), feeBuf, 32, 2);
        std::cout << d.getID() << " | "
                  << d.getName() << " | "
                  << d.getSpecialization() << " | "
                  << d.getContact() << " | PKR "
                  << feeBuf << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 7. View All Appointments (sorted date descending)
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::viewAllAppointments(const Storage<Appointment> &appointments,
                                    const Storage<Patient> &patients,
                                    const Storage<Doctor> &doctors)
{
    int n = appointments.size();
    if (n == 0)
    {
        std::cout << "No appointments found.\n";
        return;
    }

    // Copy to local array for sorting
    Appointment arr[100];
    int count = (n > 100) ? 100 : n;
    for (int i = 0; i < count; i++)
        arr[i] = appointments.get(i);

    sortAppointmentsByDateDesc(arr, count);

    std::cout << "ID  | Patient Name         | Doctor Name          | Date       | Time  | Status\n";
    std::cout << "---------------------------------------------------------------------------------\n";
    for (int i = 0; i < count; i++)
    {
        const Patient *p = patients.findByID(arr[i].getPatientID());
        const Doctor *d = doctors.findByID(arr[i].getDoctorID());
        std::cout << arr[i].getAppointmentID() << " | "
                  << (p ? p->getName() : "Unknown") << " | "
                  << (d ? d->getName() : "Unknown") << " | "
                  << arr[i].getDate() << " | "
                  << arr[i].getTimeSlot() << " | "
                  << arr[i].getStatus() << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 8. View Unpaid Bills
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::viewUnpaidBills(const Storage<Bill> &bills,
                                const Storage<Patient> &patients)
{
    std::cout << "Bill ID | Patient Name         | Amount (PKR) | Date\n";
    std::cout << "-------------------------------------------------------\n";
    bool found = false;
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (Validator::strEq(b.getStatus(), "unpaid"))
        {
            const Patient *p = patients.findByID(b.getPatientID());
            char amtBuf[32];
            Validator::floatToStr(b.getAmount(), amtBuf, 32, 2);
            char dateDisp[32];
            Validator::textCpy(dateDisp, b.getDate(), 32);
            if (isOverdue(b.getDate()))
            {
                Validator::strCat(dateDisp, " [OVERDUE]", 32);
            }
            std::cout << b.getBillID() << "       | "
                      << (p ? p->getName() : "Unknown") << " | PKR "
                      << amtBuf << " | "
                      << dateDisp << "\n";
            found = true;
        }
    }
    if (!found)
        std::cout << "No unpaid bills.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 9. Discharge Patient
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::dischargePatient(Storage<Patient> &patients,
                                 Storage<Appointment> &appointments,
                                 Storage<Bill> &bills,
                                 Storage<Prescription> &prescriptions)
{
    char idStr[16];
    std::cout << "Enter Patient ID: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int patID = Validator::strToInt(idStr);

    if (!patients.findByID(patID))
    {
        std::cout << "Patient not found.\n";
        return;
    }

    // Check unpaid bills
    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.get(i).getPatientID() == patID &&
            Validator::strEq(bills.get(i).getStatus(), "unpaid"))
        {
            std::cout << "Cannot discharge patient with unpaid bills.\n";
            return;
        }
    }

    // Check pending appointments
    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getPatientID() == patID &&
            Validator::strEq(appointments.get(i).getStatus(), "pending"))
        {
            std::cout << "Cannot discharge patient with pending appointments.\n";
            return;
        }
    }

    FileHandler::dischargePatient(patID, patients, appointments, bills, prescriptions);
    std::cout << "Patient discharged and archived successfully.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 10. View Security Log
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::viewSecurityLog()
{
    FileHandler::displaySecurityLog();
}

// ─────────────────────────────────────────────────────────────────────────────
// 11. Generate Daily Report
// ─────────────────────────────────────────────────────────────────────────────
void AdminMenu::generateDailyReport(const Storage<Appointment> &appointments,
                                    const Storage<Bill> &bills,
                                    const Storage<Patient> &patients,
                                    const Storage<Doctor> &doctors)
{
    char today[11];
    getTodayDate(today);

    int totalToday = 0, pendingN = 0, completedN = 0, noshowN = 0, cancelledN = 0;
    float revenueToday = 0.0f;

    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment &ap = appointments.get(i);
        if (Validator::strEq(ap.getDate(), today))
        {
            totalToday++;
            if (Validator::strEq(ap.getStatus(), "pending"))
                pendingN++;
            else if (Validator::strEq(ap.getStatus(), "completed"))
                completedN++;
            else if (Validator::strEq(ap.getStatus(), "no-show"))
                noshowN++;
            else if (Validator::strEq(ap.getStatus(), "cancelled"))
                cancelledN++;
        }
    }

    // Revenue: paid bills dated today
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (Validator::strEq(b.getStatus(), "paid") &&
            Validator::strEq(b.getDate(), today))
        {
            revenueToday += b.getAmount();
        }
    }

    char revBuf[32];
    Validator::floatToStr(revenueToday, revBuf, 32, 2);

    std::cout << "===== Daily Report: " << today << " =====\n";
    std::cout << "Total appointments today: " << totalToday
              << " (Pending: " << pendingN
              << " Completed: " << completedN
              << " No-show: " << noshowN
              << " Cancelled: " << cancelledN << ")\n";
    std::cout << "Revenue collected today (paid bills): PKR " << revBuf << "\n";

    // Patients with outstanding unpaid bills
    std::cout << "\nPatients with outstanding unpaid bills:\n";
    std::cout << "Patient Name         | Total Owed\n";
    std::cout << "-------------------------------------\n";
    bool anyUnpaid = false;
    for (int i = 0; i < patients.size(); i++)
    {
        float owed = 0.0f;
        for (int j = 0; j < bills.size(); j++)
        {
            if (bills.get(j).getPatientID() == patients.get(i).getID() &&
                Validator::strEq(bills.get(j).getStatus(), "unpaid"))
            {
                owed += bills.get(j).getAmount();
            }
        }
        if (owed > 0.0f)
        {
            char owedBuf[32];
            Validator::floatToStr(owed, owedBuf, 32, 2);
            std::cout << patients.get(i).getName() << " | PKR " << owedBuf << "\n";
            anyUnpaid = true;
        }
    }
    if (!anyUnpaid)
        std::cout << "None\n";

    // Doctor-wise summary for today
    std::cout << "\nDoctor-wise summary for today:\n";
    std::cout << "Doctor Name          | Completed | Pending | No-show\n";
    std::cout << "-------------------------------------------------------\n";
    for (int i = 0; i < doctors.size(); i++)
    {
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appointments.size(); j++)
        {
            const Appointment &ap = appointments.get(j);
            if (ap.getDoctorID() == doctors.get(i).getID() &&
                Validator::strEq(ap.getDate(), today))
            {
                if (Validator::strEq(ap.getStatus(), "completed"))
                    dc++;
                else if (Validator::strEq(ap.getStatus(), "pending"))
                    dp++;
                else if (Validator::strEq(ap.getStatus(), "no-show"))
                    dn++;
            }
        }
        if (dc + dp + dn > 0)
        {
            std::cout << doctors.get(i).getName()
                      << " | " << dc
                      << "         | " << dp
                      << "       | " << dn << "\n";
        }
    }
}