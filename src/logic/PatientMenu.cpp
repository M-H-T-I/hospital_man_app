#include "PatientMenu.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "HospitalException.hpp"
#include "errors/InvalidInputException.hpp"
#include "errors/SlotUnavailableException.hpp"
#include "errors/InsufficientFundsException.hpp"
#include <cstring>
#include <cstdio>


static void bufCpy(char *dst, const char *src, int max)
{
    int i = 0;
    while (i < max - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}
static void bufCat(char *dst, const char *src, int max)
{
    int i = 0; while (dst[i]) i++;
    int j = 0;
    while (i < max - 1 && src[j]) dst[i++] = src[j++];
    dst[i] = '\0';
}
// Append a single int as decimal text
static void bufCatInt(char *dst, int v, int max)
{
    char tmp[24]; Validator::intToStr(v, tmp, 24);
    bufCat(dst, tmp, max);
}
// Append a float with 2 decimal places
static void bufCatFloat(char *dst, float v, int max)
{
    char tmp[32]; Validator::floatToStr(v, tmp, 32, 2);
    bufCat(dst, tmp, max);
}

// ─────────────────────────────────────────────────────────────────────────────
// Sorting helpers
// ─────────────────────────────────────────────────────────────────────────────
int PatientMenu::compareDates(const char *d1, const char *d2)
{
    int day1, mon1, yr1, day2, mon2, yr2;
    Validator::parseDate(d1, day1, mon1, yr1);
    Validator::parseDate(d2, day2, mon2, yr2);
    if (yr1  != yr2)  return yr1  < yr2  ? -1 : 1;
    if (mon1 != mon2) return mon1 < mon2 ? -1 : 1;
    if (day1 != day2) return day1 < day2 ? -1 : 1;
    return 0;
}

void PatientMenu::sortAppointmentsByDateAsc(Appointment *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (compareDates(arr[j].getDate(), arr[j+1].getDate()) > 0)
            {
                Appointment t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t;
            }
}

void PatientMenu::sortPrescriptionsByDateDesc(Prescription *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (compareDates(arr[j].getDate(), arr[j+1].getDate()) < 0)
            {
                Prescription t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t;
            }
}

// ─────────────────────────────────────────────────────────────────────────────
// listDoctorsBySpec  –  used by Book Appointment wizard (step 1 preview)
// ─────────────────────────────────────────────────────────────────────────────
int PatientMenu::listDoctorsBySpec(const Storage<Doctor> &doctors,
                                   const char *specialization,
                                   char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    int found = 0;
    for (int i = 0; i < doctors.size(); i++)
    {
        if (Validator::strEqCaseInsensitive(doctors.get(i).getSpecialization(),
                                            specialization))
        {
            bufCat(outBuf, "ID: ", outBufSz);
            bufCatInt(outBuf, doctors.get(i).getID(), outBufSz);
            bufCat(outBuf, "  |  ", outBufSz);
            bufCat(outBuf, doctors.get(i).getName(), outBufSz);
            bufCat(outBuf, "  |  Fee: PKR ", outBufSz);
            bufCatFloat(outBuf, doctors.get(i).getFee(), outBufSz);
            bufCat(outBuf, "\n", outBufSz);
            found++;
        }
    }
    if (found == 0)
        bufCat(outBuf, "No doctors available for that specialization.\n", outBufSz);
    return found;
}

// ─────────────────────────────────────────────────────────────────────────────
// listAvailableSlots  –  used by Book Appointment wizard (step 3 preview)
// ─────────────────────────────────────────────────────────────────────────────
int PatientMenu::listAvailableSlots(const Storage<Appointment> &appointments,
                                    int doctorID, const char *dateStr,
                                    char *outBuf, int outBufSz)
{
    static const char *slots[] = {
        "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00"
    };
    outBuf[0] = '\0';
    int freeCount = 0;
    for (int s = 0; s < 8; s++)
    {
        bool taken = false;
        for (int a = 0; a < appointments.size(); a++)
        {
            const Appointment &ap = appointments.get(a);
            if (ap.getDoctorID() == doctorID &&
                Validator::strEq(ap.getDate(), dateStr) &&
                Validator::strEq(ap.getTimeSlot(), slots[s]) &&
                !Validator::strEq(ap.getStatus(), "cancelled"))
            { taken = true; break; }
        }
        if (!taken)
        {
            bufCat(outBuf, slots[s], outBufSz);
            bufCat(outBuf, "\n", outBufSz);
            freeCount++;
        }
    }
    if (freeCount == 0)
        bufCat(outBuf, "No slots available on this date.\n", outBufSz);
    return freeCount;
}

// ─────────────────────────────────────────────────────────────────────────────
// listPendingAppointments  –  shown in Cancel Appointment wizard
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::listPendingAppointments(const Patient &patient,
                                          const Storage<Appointment> &appointments,
                                          const Storage<Doctor>      &doctors,
                                          char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    bool any = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment &ap = appointments.get(i);
        if (ap.getPatientID() == patient.getID() &&
            Validator::strEq(ap.getStatus(), "pending"))
        {
            const Doctor *doc = doctors.findByID(ap.getDoctorID());
            bufCat(outBuf, "ID: ", outBufSz);
            bufCatInt(outBuf, ap.getAppointmentID(), outBufSz);
            bufCat(outBuf, "  |  Dr. ", outBufSz);
            bufCat(outBuf, doc ? doc->getName() : "Unknown", outBufSz);
            bufCat(outBuf, "  |  ", outBufSz);
            bufCat(outBuf, ap.getDate(), outBufSz);
            bufCat(outBuf, "  ", outBufSz);
            bufCat(outBuf, ap.getTimeSlot(), outBufSz);
            bufCat(outBuf, "\n", outBufSz);
            any = true;
        }
    }
    if (!any)
        bufCat(outBuf, "You have no pending appointments.\n", outBufSz);
}

// ─────────────────────────────────────────────────────────────────────────────
// listUnpaidBills  –  shown in Pay Bill wizard
// ─────────────────────────────────────────────────────────────────────────────
void PatientMenu::listUnpaidBills(const Patient &patient,
                                   const Storage<Bill> &bills,
                                   char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    bool any = false;
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (b.getPatientID() == patient.getID() &&
            Validator::strEq(b.getStatus(), "unpaid"))
        {
            bufCat(outBuf, "Bill ID: ", outBufSz);
            bufCatInt(outBuf, b.getBillID(), outBufSz);
            bufCat(outBuf, "  |  PKR ", outBufSz);
            bufCatFloat(outBuf, b.getAmount(), outBufSz);
            bufCat(outBuf, "  |  ", outBufSz);
            bufCat(outBuf, b.getDate(), outBufSz);
            bufCat(outBuf, "\n", outBufSz);
            any = true;
        }
    }
    if (!any)
        bufCat(outBuf, "No unpaid bills.\n", outBufSz);
}

// ─────────────────────────────────────────────────────────────────────────────
// 1. bookAppointment
// ─────────────────────────────────────────────────────────────────────────────
bool PatientMenu::bookAppointment(Patient &patient,
                                  Storage<Doctor>      &doctors,
                                  Storage<Appointment> &appointments,
                                  Storage<Bill>        &bills,
                                  Storage<Patient>     &patients,
                                  const BookAppointmentInput &in,
                                  char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    // Validate date
    if (!Validator::isValidDate(in.date))
    {
        bufCpy(outBuf, "Invalid date. Use DD-MM-YYYY format.", outBufSz);
        return false;
    }

    // Validate doctor
    Doctor *doc = doctors.findByID(in.doctorID);
    if (!doc)
    {
        bufCpy(outBuf, "Doctor not found.", outBufSz);
        return false;
    }

    // Validate specialization match (optional safety check)
    if (!Validator::strEqCaseInsensitive(doc->getSpecialization(), in.specialization))
    {
        bufCpy(outBuf, "Doctor specialization does not match selection.", outBufSz);
        return false;
    }

    // Validate time slot format
    if (!Validator::isValidTimeSlot(in.timeSlot))
    {
        bufCpy(outBuf, "Invalid time slot.", outBufSz);
        return false;
    }

    // Check slot availability
    for (int a = 0; a < appointments.size(); a++)
    {
        const Appointment &ap = appointments.get(a);
        if (ap.getDoctorID() == in.doctorID &&
            Validator::strEq(ap.getDate(), in.date) &&
            Validator::strEq(ap.getTimeSlot(), in.timeSlot) &&
            !Validator::strEq(ap.getStatus(), "cancelled"))
        {
            bufCpy(outBuf, "That time slot is already taken. Please choose another.", outBufSz);
            return false;
        }
    }

    // Check balance
    if (patient.getBalance() < doc->getFee())
    {
        bufCpy(outBuf, "Insufficient balance. Doctor fee is PKR ", outBufSz);
        bufCatFloat(outBuf, doc->getFee(), outBufSz);
        bufCat(outBuf, ". Please top up first.", outBufSz);
        return false;
    }

    // Commit
    patient -= doc->getFee();
    int newAppID  = FileHandler::getNextAppointmentID(appointments);
    int newBillID = FileHandler::getNextBillID(bills);

    Appointment newApp(newAppID, patient.getID(), in.doctorID,
                       in.date, in.timeSlot, "pending");
    appointments.add(newApp);
    FileHandler::appendAppointment(newApp);

    Bill newBill(newBillID, patient.getID(), newAppID,
                 doc->getFee(), "unpaid", in.date);
    bills.add(newBill);
    FileHandler::appendBill(newBill);

    FileHandler::updatePatient(patient, patients);

    bufCpy(outBuf, "Appointment booked!  ID: ", outBufSz);
    bufCatInt(outBuf, newAppID, outBufSz);
    bufCat(outBuf, "\nWith Dr. ", outBufSz);
    bufCat(outBuf, doc->getName(), outBufSz);
    bufCat(outBuf, " on ", outBufSz);
    bufCat(outBuf, in.date, outBufSz);
    bufCat(outBuf, " at ", outBufSz);
    bufCat(outBuf, in.timeSlot, outBufSz);
    bufCat(outBuf, "\nFee deducted: PKR ", outBufSz);
    bufCatFloat(outBuf, doc->getFee(), outBufSz);
    bufCat(outBuf, "\nNew balance:  PKR ", outBufSz);
    bufCatFloat(outBuf, patient.getBalance(), outBufSz);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// 2. cancelAppointment
// ─────────────────────────────────────────────────────────────────────────────
bool PatientMenu::cancelAppointment(Patient &patient,
                                    Storage<Appointment> &appointments,
                                    Storage<Bill>        &bills,
                                    Storage<Doctor>      &doctors,
                                    Storage<Patient>     &patients,
                                    const CancelAppointmentInput &in,
                                    char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    Appointment *app = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getAppointmentID() == in.appointmentID &&
            ap.getPatientID()     == patient.getID() &&
            Validator::strEq(ap.getStatus(), "pending"))
        { app = &ap; break; }
    }
    if (!app)
    {
        bufCpy(outBuf, "Appointment not found or is not cancellable.", outBufSz);
        return false;
    }

    Doctor *doc = doctors.findByID(app->getDoctorID());
    float refund = doc ? doc->getFee() : 0.0f;

    app->setStatus("cancelled");
    FileHandler::saveAllAppointments(appointments);

    patient += refund;
    FileHandler::updatePatient(patient, patients);

    for (int i = 0; i < bills.size(); i++)
        if (bills.get(i).getAppointmentID() == in.appointmentID)
        { bills.get(i).setStatus("cancelled"); break; }
    FileHandler::saveAllBills(bills);

    bufCpy(outBuf, "Appointment cancelled. Refund: PKR ", outBufSz);
    bufCatFloat(outBuf, refund, outBufSz);
    bufCat(outBuf, "\nNew balance: PKR ", outBufSz);
    bufCatFloat(outBuf, patient.getBalance(), outBufSz);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// 3. viewMyAppointments
// ─────────────────────────────────────────────────────────────────────────────
bool PatientMenu::viewMyAppointments(const Patient &patient,
                                     const Storage<Appointment> &appointments,
                                     const Storage<Doctor>      &doctors,
                                     char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    Appointment myApps[100];
    int count = 0;
    for (int i = 0; i < appointments.size() && count < 100; i++)
        if (appointments.get(i).getPatientID() == patient.getID())
            myApps[count++] = appointments.get(i);

    if (count == 0)
    {
        bufCpy(outBuf, "No appointments found.", outBufSz);
        return true;
    }

    sortAppointmentsByDateAsc(myApps, count);

    bufCpy(outBuf,
           "App ID  | Doctor               | Specialization    | Date       | Time  | Status\n"
           "---------------------------------------------------------------------------------\n",
           outBufSz);

    for (int i = 0; i < count; i++)
    {
        const Doctor *doc = doctors.findByID(myApps[i].getDoctorID());
        bufCatInt(outBuf, myApps[i].getAppointmentID(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, doc ? doc->getName() : "Unknown", outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, doc ? doc->getSpecialization() : "Unknown", outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, myApps[i].getDate(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, myApps[i].getTimeSlot(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, myApps[i].getStatus(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// 4. viewMyMedicalRecords
// ─────────────────────────────────────────────────────────────────────────────
bool PatientMenu::viewMyMedicalRecords(const Patient &patient,
                                       const Storage<Prescription> &prescriptions,
                                       const Storage<Doctor>       &doctors,
                                       char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    Prescription myPrx[100];
    int count = 0;
    for (int i = 0; i < prescriptions.size() && count < 100; i++)
        if (prescriptions.get(i).getPatientID() == patient.getID())
            myPrx[count++] = prescriptions.get(i);

    if (count == 0)
    {
        bufCpy(outBuf, "No medical records found.", outBufSz);
        return true;
    }

    sortPrescriptionsByDateDesc(myPrx, count);

    for (int i = 0; i < count; i++)
    {
        const Doctor *doc = doctors.findByID(myPrx[i].getDoctorID());
        bufCat(outBuf, "─────────────────────────────\n", outBufSz);
        bufCat(outBuf, "Date:      ", outBufSz);
        bufCat(outBuf, myPrx[i].getDate(), outBufSz);
        bufCat(outBuf, "\nDoctor:    ", outBufSz);
        bufCat(outBuf, doc ? doc->getName() : "Unknown", outBufSz);
        bufCat(outBuf, "\nMedicines: ", outBufSz);
        bufCat(outBuf, myPrx[i].getMedicines(), outBufSz);
        bufCat(outBuf, "\nNotes:     ", outBufSz);
        bufCat(outBuf, myPrx[i].getNotes(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// 5. viewMyBills
// ─────────────────────────────────────────────────────────────────────────────
bool PatientMenu::viewMyBills(const Patient &patient,
                              const Storage<Bill> &bills,
                              char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    bufCpy(outBuf,
           "Bill ID  | App ID  | Amount (PKR)  | Status     | Date\n"
           "────────────────────────────────────────────────────────\n",
           outBufSz);

    bool found = false;
    float totalUnpaid = 0.0f;

    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (b.getPatientID() != patient.getID()) continue;

        bufCatInt(outBuf, b.getBillID(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCatInt(outBuf, b.getAppointmentID(), outBufSz);
        bufCat(outBuf, "  |  PKR ", outBufSz);
        bufCatFloat(outBuf, b.getAmount(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, b.getStatus(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, b.getDate(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
        found = true;

        if (Validator::strEq(b.getStatus(), "unpaid"))
            totalUnpaid += b.getAmount();
    }

    if (!found)
    {
        bufCpy(outBuf, "No bills found.", outBufSz);
        return true;
    }

    bufCat(outBuf, "────────────────────────────────────────────────────────\n", outBufSz);
    bufCat(outBuf, "Total outstanding unpaid: PKR ", outBufSz);
    bufCatFloat(outBuf, totalUnpaid, outBufSz);
    bufCat(outBuf, "\n", outBufSz);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// 6. payBill
// ─────────────────────────────────────────────────────────────────────────────
bool PatientMenu::payBill(Patient &patient,
                          Storage<Bill>    &bills,
                          Storage<Patient> &patients,
                          const PayBillInput &in,
                          char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    Bill *bill = nullptr;
    for (int i = 0; i < bills.size(); i++)
    {
        Bill &b = bills.get(i);
        if (b.getBillID()        == in.billID &&
            b.getPatientID()     == patient.getID() &&
            Validator::strEq(b.getStatus(), "unpaid"))
        { bill = &b; break; }
    }
    if (!bill)
    {
        bufCpy(outBuf, "Bill not found or already paid.", outBufSz);
        return false;
    }

    if (patient.getBalance() < bill->getAmount())
    {
        bufCpy(outBuf, "Insufficient balance. Bill amount: PKR ", outBufSz);
        bufCatFloat(outBuf, bill->getAmount(), outBufSz);
        bufCat(outBuf, "  |  Your balance: PKR ", outBufSz);
        bufCatFloat(outBuf, patient.getBalance(), outBufSz);
        bufCat(outBuf, "\nPlease top up first.", outBufSz);
        return false;
    }

    patient -= bill->getAmount();
    bill->setStatus("paid");
    FileHandler::saveAllBills(bills);
    FileHandler::updatePatient(patient, patients);

    bufCpy(outBuf, "Bill paid successfully!\nRemaining balance: PKR ", outBufSz);
    bufCatFloat(outBuf, patient.getBalance(), outBufSz);
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// 7. topUpBalance
// ─────────────────────────────────────────────────────────────────────────────
bool PatientMenu::topUpBalance(Patient &patient,
                               Storage<Patient> &patients,
                               const TopUpInput &in,
                               char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    if (!Validator::isPositiveFloat(in.amount))
    {
        bufCpy(outBuf, "Invalid amount. Please enter a positive number.", outBufSz);
        return false;
    }

    float amt = Validator::strToFloat(in.amount);
    patient += amt;
    FileHandler::updatePatient(patient, patients);

    bufCpy(outBuf, "Balance updated!\nNew balance: PKR ", outBufSz);
    bufCatFloat(outBuf, patient.getBalance(), outBufSz);
    return true;
}