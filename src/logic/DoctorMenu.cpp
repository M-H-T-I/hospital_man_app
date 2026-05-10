// =============================================================================
// DoctorMenu.cpp
// =============================================================================
#include "DoctorMenu.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include <iostream>
#include <ctime>

// ─────────────────────────────────────────────────────────────────────────────
// Get today's date as DD-MM-YYYY using ctime
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::getTodayDate(char *buf)
{
    time_t now = time(0);
    struct tm *t = localtime(&now);
    strftime(buf, 11, "%d-%m-%Y", t);
}

// ─────────────────────────────────────────────────────────────────────────────
// Date compare helper
// ─────────────────────────────────────────────────────────────────────────────
int DoctorMenu::compareDates(const char *d1, const char *d2)
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

// ─────────────────────────────────────────────────────────────────────────────
// Sort appointments by time slot ascending (bubble sort, no library)
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::sortByTimeSlotAsc(Appointment *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            // Compare HH:MM strings lexicographically — valid since fixed format
            bool swap = false;
            const char *t1 = arr[j].getTimeSlot();
            const char *t2 = arr[j + 1].getTimeSlot();
            int k = 0;
            while (t1[k] != '\0' && t2[k] != '\0')
            {
                if (t1[k] > t2[k])
                {
                    swap = true;
                    break;
                }
                if (t1[k] < t2[k])
                {
                    break;
                }
                k++;
            }
            if (swap)
            {
                Appointment tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

void DoctorMenu::sortPrescsByDateDesc(Prescription *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (compareDates(arr[j].getDate(), arr[j + 1].getDate()) < 0)
            {
                Prescription tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Run doctor menu loop
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::run(Doctor &doctor,
                     Storage<Appointment> &appointments,
                     Storage<Patient> &patients,
                     Storage<Prescription> &prescriptions,
                     Storage<Bill> &bills)
{
    char choice[8];
    while (true)
    {
        doctor.displayMenu();
        std::cin.getline(choice, 8);
        Validator::trim(choice);

        if (Validator::strEq(choice, "1"))
            viewTodayAppointments(doctor, appointments, patients);
        else if (Validator::strEq(choice, "2"))
            markComplete(doctor, appointments);
        else if (Validator::strEq(choice, "3"))
            markNoShow(doctor, appointments, bills);
        else if (Validator::strEq(choice, "4"))
            writePrescription(doctor, appointments, prescriptions);
        else if (Validator::strEq(choice, "5"))
            viewPatientHistory(doctor, patients, appointments, prescriptions);
        else if (Validator::strEq(choice, "6"))
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
// 1. View Today's Appointments
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::viewTodayAppointments(const Doctor &doctor, const Storage<Appointment> &appointments,
                                       const Storage<Patient> &patients)
{
    char today[11];
    getTodayDate(today);

    Appointment todayApps[100];
    int count = 0;

    for (int i = 0; i < appointments.size() && count < 100; i++)
    {
        const Appointment &ap = appointments.get(i);
        if (ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getDate(), today))
        {
            todayApps[count++] = ap;
        }
    }

    if (count == 0)
    {
        std::cout << "No appointments scheduled for today.\n";
        return;
    }

    sortByTimeSlotAsc(todayApps, count);

    std::cout << "Today's appointments (" << today << "):\n";
    std::cout << "ID  | Patient Name         | Time  | Status\n";
    std::cout << "--------------------------------------------\n";
    for (int i = 0; i < count; i++)
    {
        const Patient *p = patients.findByID(todayApps[i].getPatientID());
        std::cout << todayApps[i].getAppointmentID() << " | "
                  << (p ? p->getName() : "Unknown") << " | "
                  << todayApps[i].getTimeSlot() << " | "
                  << todayApps[i].getStatus() << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 2. Mark Appointment Complete
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::markComplete(const Doctor &doctor,
                              Storage<Appointment> &appointments)
{
    char today[11];
    getTodayDate(today);

    // Show today's pending
    bool hasPending = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getDate(), today) &&
            Validator::strEq(ap.getStatus(), "pending"))
        {
            std::cout << "ID: " << ap.getAppointmentID()
                      << " | Time: " << ap.getTimeSlot() << "\n";
            hasPending = true;
        }
    }
    if (!hasPending)
    {
        std::cout << "No pending appointments for today.\n";
        return;
    }

    char idStr[16];
    std::cout << "Enter Appointment ID: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int appID = Validator::strToInt(idStr);

    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getAppointmentID() == appID &&
            ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getStatus(), "pending") &&
            Validator::strEq(ap.getDate(), today))
        {
            ap.setStatus("completed");
            FileHandler::saveAllAppointments(appointments);
            std::cout << "Appointment marked as completed.\n";
            return;
        }
    }
    std::cout << "Invalid appointment ID.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 3. Mark Appointment No-Show
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::markNoShow(const Doctor &doctor,
                            Storage<Appointment> &appointments,
                            Storage<Bill> &bills)
{
    char today[11];
    getTodayDate(today);

    bool hasPending = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getDate(), today) &&
            Validator::strEq(ap.getStatus(), "pending"))
        {
            std::cout << "ID: " << ap.getAppointmentID()
                      << " | Time: " << ap.getTimeSlot() << "\n";
            hasPending = true;
        }
    }
    if (!hasPending)
    {
        std::cout << "No pending appointments for today.\n";
        return;
    }

    char idStr[16];
    std::cout << "Enter Appointment ID: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int appID = Validator::strToInt(idStr);

    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getAppointmentID() == appID &&
            ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getStatus(), "pending") &&
            Validator::strEq(ap.getDate(), today))
        {
            ap.setStatus("no-show");
            FileHandler::saveAllAppointments(appointments);

            // Cancel corresponding bill
            for (int b = 0; b < bills.size(); b++)
            {
                if (bills.get(b).getAppointmentID() == appID)
                {
                    bills.get(b).setStatus("cancelled");
                    break;
                }
            }
            FileHandler::saveAllBills(bills);

            std::cout << "Appointment marked as no-show.\n";
            return;
        }
    }
    std::cout << "Invalid appointment ID.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 4. Write Prescription
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::writePrescription(const Doctor &doctor,
                                   Storage<Appointment> &appointments,
                                   Storage<Prescription> &prescriptions)
{
    char idStr[16];
    std::cout << "Enter Appointment ID: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int appID = Validator::strToInt(idStr);

    // Validate: belongs to this doctor and is completed
    Appointment *app = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getAppointmentID() == appID &&
            appointments.get(i).getDoctorID() == doctor.getID() &&
            Validator::strEq(appointments.get(i).getStatus(), "completed"))
        {
            app = &appointments.get(i);
            break;
        }
    }
    if (!app)
    {
        std::cout << "Invalid appointment ID or appointment not completed.\n";
        return;
    }

    // Check if prescription already exists
    for (int i = 0; i < prescriptions.size(); i++)
    {
        if (prescriptions.get(i).getAppointmentID() == appID)
        {
            std::cout << "Prescription already written for this appointment.\n";
            return;
        }
    }

    char medicines[500];
    char notes[300];

    std::cout << "Enter medicines (format: MedicineName Dosage; e.g. Paracetamol 500mg;Amoxicillin 250mg): ";
    std::cin.getline(medicines, 500);
    Validator::trim(medicines);

    std::cout << "Enter notes (max 300 chars): ";
    std::cin.getline(notes, 300);
    Validator::trim(notes);

    int newPrescID = FileHandler::getNextPrescriptionID(prescriptions);

    Prescription presc(newPrescID, appID, app->getPatientID(), doctor.getID(),
                       app->getDate(), medicines, notes);
    prescriptions.add(presc);
    FileHandler::appendPrescription(presc);

    std::cout << "Prescription saved.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 5. View Patient Medical History
// ─────────────────────────────────────────────────────────────────────────────
void DoctorMenu::viewPatientHistory(const Doctor &doctor,
                                    const Storage<Patient> &patients,
                                    const Storage<Appointment> &appointments,
                                    const Storage<Prescription> &prescriptions)
{
    char idStr[16];
    std::cout << "Enter Patient ID: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int patID = Validator::strToInt(idStr);

    // Validate patient exists
    const Patient *patient = patients.findByID(patID);
    if (!patient)
    {
        std::cout << "Access denied. You can only view records of your own patients.\n";
        return;
    }

    // Check doctor has at least one completed appointment with this patient
    bool hasCompleted = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment &ap = appointments.get(i);
        if (ap.getPatientID() == patID &&
            ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getStatus(), "completed"))
        {
            hasCompleted = true;
            break;
        }
    }
    if (!hasCompleted)
    {
        std::cout << "Access denied. You can only view records of your own patients.\n";
        return;
    }

    // Collect and sort prescriptions
    Prescription myPrx[100];
    int count = 0;
    for (int i = 0; i < prescriptions.size() && count < 100; i++)
    {
        if (prescriptions.get(i).getPatientID() == patID &&
            prescriptions.get(i).getDoctorID() == doctor.getID())
        {
            myPrx[count++] = prescriptions.get(i);
        }
    }
    if (count == 0)
    {
        std::cout << "No prescriptions found for this patient.\n";
        return;
    }

    sortPrescsByDateDesc(myPrx, count);

    std::cout << "Medical history for patient: " << patient->getName() << "\n";
    for (int i = 0; i < count; i++)
    {
        std::cout << "---\n";
        std::cout << "Date: " << myPrx[i].getDate() << "\n";
        std::cout << "Medicines: " << myPrx[i].getMedicines() << "\n";
        std::cout << "Notes: " << myPrx[i].getNotes() << "\n";
    }
}