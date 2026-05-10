#include "DoctorMenu.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include <cstring>
#include <cstdio>
#include <ctime>

static void bufCpy(char *dst, const char *src, int max)
{
    int i = 0;
    while (i < max - 1 && src[i])
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}
static void bufCat(char *dst, const char *src, int max)
{
    int i = 0;
    while (dst[i])
        i++;
    int j = 0;
    while (i < max - 1 && src[j])
        dst[i++] = src[j++];
    dst[i] = '\0';
}
static void bufCatInt(char *dst, int v, int max)
{
    char tmp[24];
    Validator::intToStr(v, tmp, 24);
    bufCat(dst, tmp, max);
}

void DoctorMenu::getTodayDate(char *buf)
{
    time_t now = time(0);
    struct tm *t = localtime(&now);
    strftime(buf, 11, "%d-%m-%Y", t);
}

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

void DoctorMenu::sortByTimeSlotAsc(Appointment *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
        {
            const char *t1 = arr[j].getTimeSlot();
            const char *t2 = arr[j + 1].getTimeSlot();
            int k = 0;
            bool swap = false;
            while (t1[k] && t2[k])
            {
                if (t1[k] > t2[k])
                {
                    swap = true;
                    break;
                }
                if (t1[k] < t2[k])
                    break;
                k++;
            }
            if (swap)
            {
                Appointment t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
        }
}

void DoctorMenu::sortPrescsByDateDesc(Prescription *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (compareDates(arr[j].getDate(), arr[j + 1].getDate()) < 0)
            {
                Prescription t = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = t;
            }
}

void DoctorMenu::listTodayPending(const Doctor &doctor,
                                  const Storage<Appointment> &appointments,
                                  char *outBuf, int outBufSz)
{
    char today[11];
    getTodayDate(today);
    outBuf[0] = '\0';

    Appointment pending[100];
    int count = 0;
    for (int i = 0; i < appointments.size() && count < 100; i++)
    {
        const Appointment &ap = appointments.get(i);
        if (ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getDate(), today) &&
            Validator::strEq(ap.getStatus(), "pending"))
            pending[count++] = ap;
    }

    if (count == 0)
    {
        bufCpy(outBuf, "No pending appointments for today.", outBufSz);
        return;
    }

    sortByTimeSlotAsc(pending, count);

    bufCpy(outBuf, "Today's pending appointments:\n", outBufSz);
    bufCat(outBuf, "Appt ID  |  Time   |  Patient ID\n", outBufSz);
    bufCat(outBuf, "────────────────────────────────\n", outBufSz);
    for (int i = 0; i < count; i++)
    {
        bufCatInt(outBuf, pending[i].getAppointmentID(), outBufSz);
        bufCat(outBuf, "        |  ", outBufSz);
        bufCat(outBuf, pending[i].getTimeSlot(), outBufSz);
        bufCat(outBuf, "  |  Patient #", outBufSz);
        bufCatInt(outBuf, pending[i].getPatientID(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
}

void DoctorMenu::listCompletedWithoutPrescription(
    const Doctor &doctor,
    const Storage<Appointment> &appointments,
    const Storage<Prescription> &prescriptions,
    char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    bool any = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment &ap = appointments.get(i);
        if (ap.getDoctorID() != doctor.getID())
            continue;
        if (!Validator::strEq(ap.getStatus(), "completed"))
            continue;

        bool hasPrescription = false;
        for (int j = 0; j < prescriptions.size(); j++)
            if (prescriptions.get(j).getAppointmentID() == ap.getAppointmentID())
            {
                hasPrescription = true;
                break;
            }

        if (!hasPrescription)
        {
            if (!any)
            {

                bufCpy(outBuf, "Completed appointments awaiting prescription:\n", outBufSz);
                bufCat(outBuf, "Appt ID  |  Patient ID  |  Date\n", outBufSz);
                bufCat(outBuf, "───────────────────────────────────────\n", outBufSz);
            }
            bufCatInt(outBuf, ap.getAppointmentID(), outBufSz);
            bufCat(outBuf, "        |  ", outBufSz);
            bufCatInt(outBuf, ap.getPatientID(), outBufSz);
            bufCat(outBuf, "           |  ", outBufSz);
            bufCat(outBuf, ap.getDate(), outBufSz);
            bufCat(outBuf, "\n", outBufSz);
            any = true;
        }
    }
    if (!any)
        bufCpy(outBuf, "No completed appointments awaiting a prescription.", outBufSz);
}

bool DoctorMenu::viewTodayAppointments(const Doctor &doctor,
                                       const Storage<Appointment> &appointments,
                                       const Storage<Patient> &patients,
                                       char *outBuf, int outBufSz)
{
    char today[11];
    getTodayDate(today);
    outBuf[0] = '\0';

    Appointment todayApps[100];
    int count = 0;
    for (int i = 0; i < appointments.size() && count < 100; i++)
    {
        const Appointment &ap = appointments.get(i);
        if (ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getDate(), today))
            todayApps[count++] = ap;
    }

    if (count == 0)
    {
        bufCpy(outBuf, "No appointments scheduled for today.", outBufSz);
        return true;
    }

    sortByTimeSlotAsc(todayApps, count);

    bufCpy(outBuf, "Today's appointments (", outBufSz);
    bufCat(outBuf, today, outBufSz);
    bufCat(outBuf, "):\n", outBufSz);
    bufCat(outBuf,
           "ID    | Patient Name         | Time  | Status\n"
           "───────────────────────────────────────────────\n",
           outBufSz);

    for (int i = 0; i < count; i++)
    {
        const Patient *p = patients.findByID(todayApps[i].getPatientID());
        bufCatInt(outBuf, todayApps[i].getAppointmentID(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, p ? p->getName() : "Unknown", outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, todayApps[i].getTimeSlot(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, todayApps[i].getStatus(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
    return true;
}

bool DoctorMenu::markComplete(const Doctor &doctor,
                              Storage<Appointment> &appointments,
                              const MarkAppointmentInput &in,
                              char *outBuf, int outBufSz)
{
    char today[11];
    getTodayDate(today);
    outBuf[0] = '\0';

    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getAppointmentID() == in.appointmentID &&
            ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getStatus(), "pending") &&
            Validator::strEq(ap.getDate(), today))
        {
            ap.setStatus("completed");
            FileHandler::saveAllAppointments(appointments);
            bufCpy(outBuf, "Appointment marked as completed.", outBufSz);
            return true;
        }
    }

    bufCpy(outBuf, "Appointment not found or not eligible to mark complete.", outBufSz);
    return false;
}

bool DoctorMenu::markNoShow(const Doctor &doctor,
                            Storage<Appointment> &appointments,
                            Storage<Bill> &bills,
                            const MarkAppointmentInput &in,
                            char *outBuf, int outBufSz)
{
    char today[11];
    getTodayDate(today);
    outBuf[0] = '\0';

    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getAppointmentID() == in.appointmentID &&
            ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getStatus(), "pending") &&
            Validator::strEq(ap.getDate(), today))
        {
            ap.setStatus("no-show");
            FileHandler::saveAllAppointments(appointments);

            for (int b = 0; b < bills.size(); b++)
                if (bills.get(b).getAppointmentID() == in.appointmentID)
                {
                    bills.get(b).setStatus("cancelled");
                    break;
                }
            FileHandler::saveAllBills(bills);

            bufCpy(outBuf, "Appointment marked as no-show.", outBufSz);
            return true;
        }
    }

    bufCpy(outBuf, "Appointment not found or not eligible to mark as no-show.", outBufSz);
    return false;
}

bool DoctorMenu::writePrescription(const Doctor &doctor,
                                   Storage<Appointment> &appointments,
                                   Storage<Prescription> &prescriptions,
                                   const WritePrescriptionInput &in,
                                   char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    if (in.medicines[0] == '\0')
    {
        bufCpy(outBuf, "Medicines field cannot be empty.", outBufSz);
        return false;
    }

    Appointment *app = nullptr;
    for (int i = 0; i < appointments.size(); i++)
    {
        Appointment &ap = appointments.get(i);
        if (ap.getAppointmentID() == in.appointmentID &&
            ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getStatus(), "completed"))
        {
            app = &ap;
            break;
        }
    }
    if (!app)
    {
        bufCpy(outBuf, "Appointment not found or not yet completed.", outBufSz);
        return false;
    }

    for (int i = 0; i < prescriptions.size(); i++)
        if (prescriptions.get(i).getAppointmentID() == in.appointmentID)
        {
            bufCpy(outBuf, "A prescription already exists for this appointment.", outBufSz);
            return false;
        }

    int newPrescID = FileHandler::getNextPrescriptionID(prescriptions);
    Prescription presc(newPrescID, in.appointmentID, app->getPatientID(),
                       doctor.getID(), app->getDate(),
                       in.medicines, in.notes);
    prescriptions.add(presc);
    FileHandler::appendPrescription(presc);

    bufCpy(outBuf, "Prescription saved. Prescription ID: ", outBufSz);
    bufCatInt(outBuf, newPrescID, outBufSz);
    return true;
}

bool DoctorMenu::viewPatientHistory(const Doctor &doctor, const Storage<Patient> &patients, const Storage<Appointment> &appointments,
                                    const Storage<Prescription> &prescriptions,
                                    const ViewPatientHistoryInput &in,
                                    char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    const Patient *patient = patients.findByID(in.patientID);
    if (!patient)
    {
        bufCpy(outBuf, "Patient not found.", outBufSz);
        return false;
    }

    bool hasCompleted = false;
    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment &ap = appointments.get(i);
        if (ap.getPatientID() == in.patientID &&
            ap.getDoctorID() == doctor.getID() &&
            Validator::strEq(ap.getStatus(), "completed"))
        {
            hasCompleted = true;
            break;
        }
    }
    if (!hasCompleted)
    {
        bufCpy(outBuf, "Access denied. You can only view records for your own patients.", outBufSz);
        return false;
    }

    Prescription myPrx[100];
    int count = 0;
    for (int i = 0; i < prescriptions.size() && count < 100; i++)
        if (prescriptions.get(i).getPatientID() == in.patientID &&
            prescriptions.get(i).getDoctorID() == doctor.getID())
            myPrx[count++] = prescriptions.get(i);

    if (count == 0)
    {
        bufCpy(outBuf, "No prescriptions found for this patient.", outBufSz);
        return true;
    }

    sortPrescsByDateDesc(myPrx, count);

    bufCpy(outBuf, "Medical history for: ", outBufSz);
    bufCat(outBuf, patient->getName(), outBufSz);
    bufCat(outBuf, "\n", outBufSz);

    for (int i = 0; i < count; i++)
    {
        bufCat(outBuf, "─────────────────────────────\n", outBufSz);
        bufCat(outBuf, "Date:      ", outBufSz);
        bufCat(outBuf, myPrx[i].getDate(), outBufSz);
        bufCat(outBuf, "\nMedicines: ", outBufSz);
        bufCat(outBuf, myPrx[i].getMedicines(), outBufSz);
        bufCat(outBuf, "\nNotes:     ", outBufSz);
        bufCat(outBuf, myPrx[i].getNotes(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
    return true;
}