// =============================================================================
// PatientMenu.cpp
// =============================================================================
#include "PatientMenu.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "HospitalException.hpp"
#include "errors/InvalidInputException.hpp"
#include "errors/SlotUnavailableException.hpp"
#include "errors/InsufficientFundsException.hpp"
#include <iostream>
#include <cstdio>

// ─────────────────────────────────────────────────────────────────────────────
// Date comparison: DD-MM-YYYY — returns -1 if d1 < d2, 0 if equal, 1 if d1 > d2
// ─────────────────────────────────────────────────────────────────────────────
int PatientMenu::compareDates(const char *d1, const char *d2)
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
// Manual bubble sorts
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::sortAppointmentsByDateAsc(Appointment *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (compareDates(arr[j].getDate(), arr[j + 1].getDate()) > 0)
            {
                Appointment tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

void PatientMenu::sortPrescriptionsByDateDesc(Prescription *arr, int n)
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
// Run patient menu loop
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::run(Patient &patient,
                      Storage<Doctor> &doctors,
                      Storage<Appointment> &appointments,
                      Storage<Bill> &bills,
                      Storage<Prescription> &prescriptions,
                      Storage<Patient> &patients)
{
    char choice[8];
    while (true)
    {
        patient.displayMenu();
        std::cin.getline(choice, 8);
        Validator::trim(choice);

        if (Validator::strEq(choice, "1"))
            bookAppointment(patient, doctors, appointments, bills, patients);
        else if (Validator::strEq(choice, "2"))
            cancelAppointment(patient, appointments, bills, doctors, patients);
        else if (Validator::strEq(choice, "3"))
            viewMyAppointments(patient, appointments, doctors);
        else if (Validator::strEq(choice, "4"))
            viewMyMedicalRecords(patient, prescriptions, doctors, appointments);
        else if (Validator::strEq(choice, "5"))
            viewMyBills(patient, bills);
        else if (Validator::strEq(choice, "6"))
            payBill(patient, bills, patients);
        else if (Validator::strEq(choice, "7"))
            topUpBalance(patient, patients);
        else if (Validator::strEq(choice, "8"))
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
// 1. Book Appointment
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::bookAppointment(Patient &patient,
                                  Storage<Doctor> &doctors,
                                  Storage<Appointment> &appointments,
                                  Storage<Bill> &bills,
                                  Storage<Patient> &patients)
{
    char spec[51];
    std::cout << "Enter specialization to search (e.g. Cardiology): ";
    std::cin.getline(spec, 51);
    Validator::trim(spec);

    // Display doctors with matching specialization (case-insensitive)
    int matchCount = 0;
    for (int i = 0; i < doctors.size(); i++)
    {
        if (Validator::strEqCaseInsensitive(doctors.get(i).getSpecialization(), spec))
        {
            char feeBuf[32];
            Validator::floatToStr(doctors.get(i).getFee(), feeBuf, 32, 2);
            std::cout << "  ID: " << doctors.get(i).getID()
                      << " | Name: " << doctors.get(i).getName()
                      << " | Fee: PKR " << feeBuf << "\n";
            matchCount++;
        }
    }
    if (matchCount == 0)
    {
        std::cout << "No doctors available for that specialization.\n";
        return;
    }

    // Enter Doctor ID
    char docIDStr[16];
    std::cout << "Enter Doctor ID: ";
    std::cin.getline(docIDStr, 16);
    Validator::trim(docIDStr);
    int docID = Validator::strToInt(docIDStr);
    Doctor *doc = doctors.findByID(docID);
    if (!doc)
    {
        std::cout << "Doctor not found.\n";
        return;
    }

    // Validate date (up to 3 attempts)
    char dateStr[16];
    bool validDate = false;
    for (int attempt = 0; attempt < 3; attempt++)
    {
        std::cout << "Enter date (DD-MM-YYYY): ";
        std::cin.getline(dateStr, 16);
        Validator::trim(dateStr);
        if (Validator::isValidDate(dateStr))
        {
            validDate = true;
            break;
        }
        else
        {
            std::cout << "Invalid date. Use format DD-MM-YYYY.\n";
        }
    }
    if (!validDate)
        return;

    // Display available slots
    const char *slots[] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"};

    auto showAvailableSlots = [&]()
    {
        std::cout << "Available time slots:\n";
        for (int s = 0; s < 8; s++)
        {
            bool taken = false;
            for (int a = 0; a < appointments.size(); a++)
            {
                Appointment &ap = appointments.get(a);
                if (ap.getDoctorID() == docID &&
                    Validator::strEq(ap.getDate(), dateStr) &&
                    Validator::strEq(ap.getTimeSlot(), slots[s]) &&
                    !Validator::strEq(ap.getStatus(), "cancelled"))
                {
                    taken = true;
                    break;
                }
            }
            if (!taken)
                std::cout << "  " << slots[s] << "\n";
        }
    };

    showAvailableSlots();

    // Enter time slot (loop on SlotUnavailableException)
    char slotStr[8];
    bool slotBooked = false;
    while (!slotBooked)
    {
        std::cout << "Enter time slot (e.g. 09:00): ";
        std::cin.getline(slotStr, 8);
        Validator::trim(slotStr);

        if (!Validator::isValidTimeSlot(slotStr))
        {
            std::cout << "Invalid time slot.\n";
            continue;
        }

        try
        {
            // Check if slot is taken
            for (int a = 0; a < appointments.size(); a++)
            {
                Appointment &ap = appointments.get(a);
                if (ap.getDoctorID() == docID &&
                    Validator::strEq(ap.getDate(), dateStr) &&
                    Validator::strEq(ap.getTimeSlot(), slotStr) &&
                    !Validator::strEq(ap.getStatus(), "cancelled"))
                {
                    throw SlotUnavailableException();
                }
            }
            slotBooked = true;
        }
        catch (SlotUnavailableException &e)
        {
            std::cout << e.what() << "\n";
            showAvailableSlots();
        }
    }

    // Check balance
    try
    {
        if (patient.getBalance() < doc->getFee())
        {
            char feeBuf[32];
            Validator::floatToStr(doc->getFee(), feeBuf, 32, 2);
            char msg[200] = "Insufficient funds. Doctor fee is PKR ";
            Validator::strCat(msg, feeBuf, 200);
            throw InsufficientFundsException(msg);
        }
    }
    catch (InsufficientFundsException &e)
    {
        std::cout << e.what() << "\n";
        return;
    }

    // Deduct fee and create appointment + bill
    patient -= doc->getFee();

    int newAppID = FileHandler::getNextAppointmentID(appointments);
    int newBillID = FileHandler::getNextBillID(bills);

    Appointment newApp(newAppID, patient.getID(), docID, dateStr, slotStr, "pending");
    appointments.add(newApp);
    FileHandler::appendAppointment(newApp);

    Bill newBill(newBillID, patient.getID(), newAppID, doc->getFee(), "unpaid", dateStr);
    bills.add(newBill);
    FileHandler::appendBill(newBill);

    // Update patient balance in file
    FileHandler::updatePatient(patient, patients);

    char appIDBuf[16];
    Validator::intToStr(newAppID, appIDBuf, 16);
    std::cout << "Appointment booked successfully. Appointment ID: " << appIDBuf << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 2. Cancel Appointment
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::cancelAppointment(Patient &patient,
                                    Storage<Appointment> &appointments,
                                    Storage<Bill> &bills,
                                    Storage<Doctor> &doctors,
                                    Storage<Patient> &patients)
{
    // Show pending appointments
    bool hasPending = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getPatientID() == patient.getID() &&
            Validator::strEq(ap.getStatus(), "pending"))
        {
            Doctor *doc = doctors.findByID(ap.getDoctorID());
            const char *docName = doc ? doc->getName() : "Unknown";
            std::cout << "ID: " << ap.getAppointmentID()
                      << " | Doctor: " << docName
                      << " | Date: " << ap.getDate()
                      << " | Time: " << ap.getTimeSlot() << "\n";
            hasPending = true;
        }
    }
    if (!hasPending)
    {
        std::cout << "You have no pending appointments.\n";
        return;
    }

    char idStr[16];
    std::cout << "Enter Appointment ID to cancel: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int appID = Validator::strToInt(idStr);

    // Validate
    Appointment *app = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        if (appointments.get(i).getAppointmentID() == appID &&
            appointments.get(i).getPatientID() == patient.getID() &&
            Validator::strEq(appointments.get(i).getStatus(), "pending"))
        {
            app = &appointments.get(i);
            break;
        }
    }
    if (!app)
    {
        std::cout << "Invalid appointment ID.\n";
        return;
    }

    // Get fee for refund
    Doctor *doc = doctors.findByID(app->getDoctorID());
    float refund = doc ? doc->getFee() : 0.0f;

    // Update appointment status
    app->setStatus("cancelled");
    FileHandler::saveAllAppointments(appointments);

    // Refund balance
    patient += refund;
    FileHandler::updatePatient(patient, patients);

    // Update bill status to cancelled
    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.get(i).getAppointmentID() == appID)
        {
            bills.get(i).setStatus("cancelled");
            break;
        }
    }
    FileHandler::saveAllBills(bills);

    char refBuf[32];
    Validator::floatToStr(refund, refBuf, 32, 2);
    std::cout << "Appointment cancelled. PKR " << refBuf
              << " refunded to your balance.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 3. View My Appointments
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::viewMyAppointments(const Patient &patient,
                                     const Storage<Appointment> &appointments,
                                     const Storage<Doctor> &doctors)
{
    // Collect patient's appointments into local array for sorting
    Appointment myApps[100];
    int count = 0;
    for (int i = 0; i < appointments.size() && count < 100; i++)
    {
        if (appointments.get(i).getPatientID() == patient.getID())
        {
            myApps[count++] = appointments.get(i);
        }
    }
    if (count == 0)
    {
        std::cout << "No appointments found.\n";
        return;
    }

    sortAppointmentsByDateAsc(myApps, count);

    std::cout << "-------------------------------------------------------------------\n";
    std::cout << "ID  | Doctor Name          | Specialization    | Date       | Time  | Status\n";
    std::cout << "-------------------------------------------------------------------\n";
    for (int i = 0; i < count; i++)
    {
        const Doctor *doc = doctors.findByID(myApps[i].getDoctorID());
        const char *docName = doc ? doc->getName() : "Unknown";
        const char *docSpec = doc ? doc->getSpecialization() : "Unknown";
        std::cout << myApps[i].getAppointmentID() << " | "
                  << docName << " | "
                  << docSpec << " | "
                  << myApps[i].getDate() << " | "
                  << myApps[i].getTimeSlot() << " | "
                  << myApps[i].getStatus() << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 4. View My Medical Records
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::viewMyMedicalRecords(const Patient &patient,
                                       const Storage<Prescription> &prescriptions,
                                       const Storage<Doctor> &doctors,
                                       const Storage<Appointment> & /*appointments*/)
{
    Prescription myPrx[100];
    int count = 0;
    for (int i = 0; i < prescriptions.size() && count < 100; i++)
    {
        if (prescriptions.get(i).getPatientID() == patient.getID())
        {
            myPrx[count++] = prescriptions.get(i);
        }
    }
    if (count == 0)
    {
        std::cout << "No medical records found.\n";
        return;
    }

    sortPrescriptionsByDateDesc(myPrx, count);

    for (int i = 0; i < count; i++)
    {
        const Doctor *doc = doctors.findByID(myPrx[i].getDoctorID());
        std::cout << "---\n";
        std::cout << "Date: " << myPrx[i].getDate() << "\n";
        std::cout << "Doctor: " << (doc ? doc->getName() : "Unknown") << "\n";
        std::cout << "Medicines: " << myPrx[i].getMedicines() << "\n";
        std::cout << "Notes: " << myPrx[i].getNotes() << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 5. View My Bills
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::viewMyBills(const Patient &patient,
                              const Storage<Bill> &bills)
{
    bool found = false;
    float totalUnpaid = 0.0f;

    std::cout << "Bill ID | Appointment ID | Amount (PKR) | Status     | Date\n";
    std::cout << "-------------------------------------------------------------\n";

    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (b.getPatientID() == patient.getID())
        {
            char amtBuf[32];
            Validator::floatToStr(b.getAmount(), amtBuf, 32, 2);
            std::cout << b.getBillID() << " | "
                      << b.getAppointmentID() << "              | "
                      << amtBuf << "       | "
                      << b.getStatus() << " | "
                      << b.getDate() << "\n";
            found = true;
            if (Validator::strEq(b.getStatus(), "unpaid"))
                totalUnpaid += b.getAmount();
        }
    }
    if (!found)
    {
        std::cout << "No bills found.\n";
        return;
    }
    char totBuf[32];
    Validator::floatToStr(totalUnpaid, totBuf, 32, 2);
    std::cout << "Total Outstanding Unpaid: PKR " << totBuf << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 6. Pay Bill
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::payBill(Patient &patient,
                          Storage<Bill> &bills,
                          Storage<Patient> &patients)
{
    // Show unpaid bills
    bool hasUnpaid = false;
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (b.getPatientID() == patient.getID() &&
            Validator::strEq(b.getStatus(), "unpaid"))
        {
            char amtBuf[32];
            Validator::floatToStr(b.getAmount(), amtBuf, 32, 2);
            std::cout << "Bill ID: " << b.getBillID()
                      << " | Amount: PKR " << amtBuf
                      << " | Date: " << b.getDate() << "\n";
            hasUnpaid = true;
        }
    }
    if (!hasUnpaid)
    {
        std::cout << "No unpaid bills.\n";
        return;
    }

    char idStr[16];
    std::cout << "Enter Bill ID to pay: ";
    std::cin.getline(idStr, 16);
    Validator::trim(idStr);
    int billID = Validator::strToInt(idStr);

    // Validate
    Bill *bill = nullptr;
    for (int i = 0; i < bills.size(); i++)
    {
        if (bills.get(i).getBillID() == billID &&
            bills.get(i).getPatientID() == patient.getID() &&
            Validator::strEq(bills.get(i).getStatus(), "unpaid"))
        {
            bill = &bills.get(i);
            break;
        }
    }
    if (!bill)
    {
        std::cout << "Invalid bill ID.\n";
        return;
    }

    try
    {
        if (patient.getBalance() < bill->getAmount())
        {
            throw InsufficientFundsException(
                "Insufficient funds. Please top up your balance.");
        }
    }
    catch (InsufficientFundsException &e)
    {
        std::cout << e.what() << "\n";
        return;
    }

    patient -= bill->getAmount();
    bill->setStatus("paid");

    FileHandler::saveAllBills(bills);
    FileHandler::updatePatient(patient, patients);

    char balBuf[32];
    Validator::floatToStr(patient.getBalance(), balBuf, 32, 2);
    std::cout << "Bill paid successfully. Remaining balance: PKR " << balBuf << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// 7. Top Up Balance
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::topUpBalance(Patient &patient,
                               Storage<Patient> &patients)
{
    char amtStr[32];
    for (int attempt = 0; attempt < 3; attempt++)
    {
        std::cout << "Enter amount to add (PKR): ";
        std::cin.getline(amtStr, 32);
        Validator::trim(amtStr);
        try
        {
            if (!Validator::isPositiveFloat(amtStr))
            {
                throw InvalidInputException("Amount must be a positive number.");
            }
            float amt = Validator::strToFloat(amtStr);
            patient += amt;
            FileHandler::updatePatient(patient, patients);

            char balBuf[32];
            Validator::floatToStr(patient.getBalance(), balBuf, 32, 2);
            std::cout << "Balance updated. New balance: PKR " << balBuf << "\n";
            return;
        }
        catch (InvalidInputException &e)
        {
            std::cout << e.what() << "\n";
        }
    }
    std::cout << "Too many invalid attempts. Returning to menu.\n";
}