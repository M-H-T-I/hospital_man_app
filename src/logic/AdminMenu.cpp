


#include "AdminMenu.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "HospitalException.hpp"
#include <cstring>
#include <cstdio>
#include <ctime>




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
static void bufCatInt(char *dst, int v, int max)
{
    char tmp[24]; Validator::intToStr(v, tmp, 24); bufCat(dst, tmp, max);
}
static void bufCatFloat(char *dst, float v, int max)
{
    char tmp[32]; Validator::floatToStr(v, tmp, 32, 2); bufCat(dst, tmp, max);
}




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
    if (yr1  != yr2)  return yr1  < yr2  ? -1 : 1;
    if (mon1 != mon2) return mon1 < mon2 ? -1 : 1;
    if (day1 != day2) return day1 < day2 ? -1 : 1;
    return 0;
}

void AdminMenu::sortAppointmentsByDateDesc(Appointment *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (compareDates(arr[j].getDate(), arr[j+1].getDate()) < 0)
            { Appointment t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t; }
}

bool AdminMenu::isOverdue(const char *dateStr)
{
    int day, mon, yr;
    Validator::parseDate(dateStr, day, mon, yr);
    struct tm billTm = {0};
    billTm.tm_mday = day; billTm.tm_mon = mon - 1; billTm.tm_year = yr - 1900;
    time_t billTime = mktime(&billTm);
    time_t now = time(0);
    return difftime(now, billTime) > 7.0 * 24.0 * 3600.0;
}




bool AdminMenu::addDoctor(Storage<Doctor> &doctors,
                          const AddDoctorInput &in,
                          char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    if (in.name[0] == '\0')
    { bufCpy(outBuf, "Doctor name cannot be empty.", outBufSz); return false; }

    if (in.spec[0] == '\0')
    { bufCpy(outBuf, "Specialization cannot be empty.", outBufSz); return false; }

    if (!Validator::isValidContact(in.contact))
    { bufCpy(outBuf, "Invalid contact. Must be exactly 11 numeric digits.", outBufSz); return false; }

    if (!Validator::isValidPassword(in.password))
    { bufCpy(outBuf, "Password must be at least 6 characters.", outBufSz); return false; }

    if (!Validator::isPositiveFloat(in.fee))
    { bufCpy(outBuf, "Fee must be a positive number.", outBufSz); return false; }

    float fee = Validator::strToFloat(in.fee);
    int newID = FileHandler::getNextDoctorID(doctors);
    Doctor d(newID, in.name, in.spec, in.contact, in.password, fee);
    doctors.add(d);
    FileHandler::appendDoctor(d);

    bufCpy(outBuf, "Doctor added successfully!\nID: ", outBufSz);
    bufCatInt(outBuf, newID, outBufSz);
    bufCat(outBuf, "  |  Name: ", outBufSz);
    bufCat(outBuf, in.name, outBufSz);
    return true;
}




bool AdminMenu::removeDoctor(Storage<Doctor>            &doctors,
                              const Storage<Appointment> &appointments,
                              const RemoveDoctorInput    &in,
                              char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    
    for (int i = 0; i < appointments.size(); i++)
        if (appointments.get(i).getDoctorID() == in.doctorID &&
            Validator::strEq(appointments.get(i).getStatus(), "pending"))
        {
            bufCpy(outBuf, "Cannot remove doctor with pending appointments. "
                           "Cancel or reassign them first.", outBufSz);
            return false;
        }

    Doctor *doc = doctors.findByID(in.doctorID);
    if (!doc)
    { bufCpy(outBuf, "Doctor not found.", outBufSz); return false; }

    FileHandler::deleteDoctor(in.doctorID, doctors);
    bufCpy(outBuf, "Doctor removed successfully.", outBufSz);
    return true;
}




bool AdminMenu::addPatient(Storage<Patient>      &patients,
                           const AddPatientInput &in,
                           char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    if (in.name[0] == '\0')
    { bufCpy(outBuf, "Patient name cannot be empty.", outBufSz); return false; }

    if (!Validator::isValidAge(in.age))
    { bufCpy(outBuf, "Invalid age. Must be a positive integer.", outBufSz); return false; }

    if (!Validator::isValidGender(in.gender))
    { bufCpy(outBuf, "Invalid gender. Enter M or F.", outBufSz); return false; }

    if (!Validator::isValidContact(in.contact))
    { bufCpy(outBuf, "Invalid contact. Must be exactly 11 numeric digits.", outBufSz); return false; }

    if (!Validator::isValidPassword(in.password))
    { bufCpy(outBuf, "Password must be at least 6 characters.", outBufSz); return false; }

    if (!Validator::isPositiveFloat(in.balance))
    { bufCpy(outBuf, "Balance must be a positive number.", outBufSz); return false; }

    float balance = Validator::strToFloat(in.balance);
    int   age     = Validator::strToInt(in.age);
    int   newID   = FileHandler::getNextPatientID(patients);

    Patient p(newID, in.name, age, in.gender, in.contact, in.password, balance);
    patients.add(p);
    FileHandler::appendPatient(p);

    bufCpy(outBuf, "Patient added successfully!\nID: ", outBufSz);
    bufCatInt(outBuf, newID, outBufSz);
    bufCat(outBuf, "  |  Name: ", outBufSz);
    bufCat(outBuf, in.name, outBufSz);
    return true;
}




bool AdminMenu::removePatient(Storage<Patient>      &patients,
                               Storage<Appointment>  &appointments,
                               Storage<Bill>         &bills,
                               Storage<Prescription> &prescriptions,
                               const RemovePatientInput &in,
                               char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    if (!patients.findByID(in.patientID))
    { bufCpy(outBuf, "Patient not found.", outBufSz); return false; }

    for (int i = 0; i < appointments.size(); i++)
        if (appointments.get(i).getPatientID() == in.patientID &&
            Validator::strEq(appointments.get(i).getStatus(), "pending"))
        {
            bufCpy(outBuf, "Cannot remove patient with pending appointments.", outBufSz);
            return false;
        }

    for (int i = 0; i < bills.size(); i++)
        if (bills.get(i).getPatientID() == in.patientID &&
            Validator::strEq(bills.get(i).getStatus(), "unpaid"))
        {
            bufCpy(outBuf, "Cannot remove patient with unpaid bills.", outBufSz);
            return false;
        }

    FileHandler::deletePatient(in.patientID, patients);
    FileHandler::cascadeDeletePatient(in.patientID, appointments, bills, prescriptions);
    bufCpy(outBuf, "Patient removed successfully.", outBufSz);
    return true;
}




bool AdminMenu::viewAllPatients(const Storage<Patient> &patients,
                                const Storage<Bill>    &bills,
                                char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    bufCpy(outBuf,
           "ID  | Name                 | Age | Gender | Contact     | Balance     | Unpaid Bills\n"
           "──────────────────────────────────────────────────────────────────────────────────────\n",
           outBufSz);

    for (int i = 0; i < patients.size(); i++)
    {
        const Patient &p = patients.get(i);
        int unpaid = 0;
        for (int j = 0; j < bills.size(); j++)
            if (bills.get(j).getPatientID() == p.getID() &&
                Validator::strEq(bills.get(j).getStatus(), "unpaid"))
                unpaid++;

        bufCatInt(outBuf, p.getID(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, p.getName(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCatInt(outBuf, p.getAge(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, p.getGender(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, p.getContact(), outBufSz);
        bufCat(outBuf, "  |  PKR ", outBufSz);
        bufCatFloat(outBuf, p.getBalance(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCatInt(outBuf, unpaid, outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
    if (patients.size() == 0)
        bufCat(outBuf, "(no patients)\n", outBufSz);
    return true;
}




bool AdminMenu::viewAllDoctors(const Storage<Doctor> &doctors,
                               char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    bufCpy(outBuf,
           "ID  | Name                 | Specialization    | Contact     | Fee\n"
           "────────────────────────────────────────────────────────────────────\n",
           outBufSz);

    for (int i = 0; i < doctors.size(); i++)
    {
        const Doctor &d = doctors.get(i);
        bufCatInt(outBuf, d.getID(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, d.getName(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, d.getSpecialization(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, d.getContact(), outBufSz);
        bufCat(outBuf, "  |  PKR ", outBufSz);
        bufCatFloat(outBuf, d.getFee(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
    if (doctors.size() == 0)
        bufCat(outBuf, "(no doctors)\n", outBufSz);
    return true;
}




bool AdminMenu::viewAllAppointments(const Storage<Appointment> &appointments,
                                    const Storage<Patient>     &patients,
                                    const Storage<Doctor>      &doctors,
                                    char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    int n = appointments.size();
    if (n == 0)
    { bufCpy(outBuf, "No appointments found.", outBufSz); return true; }

    Appointment arr[100];
    int count = (n > 100) ? 100 : n;
    for (int i = 0; i < count; i++) arr[i] = appointments.get(i);
    sortAppointmentsByDateDesc(arr, count);

    bufCpy(outBuf,
           "ID  | Patient              | Doctor               | Date       | Time  | Status\n"
           "──────────────────────────────────────────────────────────────────────────────────\n",
           outBufSz);

    for (int i = 0; i < count; i++)
    {
        const Patient *p = patients.findByID(arr[i].getPatientID());
        const Doctor  *d = doctors.findByID(arr[i].getDoctorID());
        bufCatInt(outBuf, arr[i].getAppointmentID(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, p ? p->getName() : "Unknown", outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, d ? d->getName() : "Unknown", outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, arr[i].getDate(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, arr[i].getTimeSlot(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, arr[i].getStatus(), outBufSz);
        bufCat(outBuf, "\n", outBufSz);
    }
    return true;
}




bool AdminMenu::viewUnpaidBills(const Storage<Bill>    &bills,
                                const Storage<Patient> &patients,
                                char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';
    bufCpy(outBuf,
           "Bill ID  | Patient Name         | Amount (PKR)  | Date\n"
           "──────────────────────────────────────────────────────────\n",
           outBufSz);

    bool found = false;
    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (!Validator::strEq(b.getStatus(), "unpaid")) continue;

        const Patient *p = patients.findByID(b.getPatientID());
        bufCatInt(outBuf, b.getBillID(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, p ? p->getName() : "Unknown", outBufSz);
        bufCat(outBuf, "  |  PKR ", outBufSz);
        bufCatFloat(outBuf, b.getAmount(), outBufSz);
        bufCat(outBuf, "  |  ", outBufSz);
        bufCat(outBuf, b.getDate(), outBufSz);
        if (isOverdue(b.getDate()))
            bufCat(outBuf, "  [OVERDUE]", outBufSz);
        bufCat(outBuf, "\n", outBufSz);
        found = true;
    }
    if (!found)
        bufCat(outBuf, "No unpaid bills.\n", outBufSz);
    return true;
}




bool AdminMenu::dischargePatient(Storage<Patient>      &patients,
                                  Storage<Appointment>  &appointments,
                                  Storage<Bill>         &bills,
                                  Storage<Prescription> &prescriptions,
                                  const DischargePatientInput &in,
                                  char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    if (!patients.findByID(in.patientID))
    { bufCpy(outBuf, "Patient not found.", outBufSz); return false; }

    for (int i = 0; i < bills.size(); i++)
        if (bills.get(i).getPatientID() == in.patientID &&
            Validator::strEq(bills.get(i).getStatus(), "unpaid"))
        {
            bufCpy(outBuf, "Cannot discharge patient with unpaid bills.", outBufSz);
            return false;
        }

    for (int i = 0; i < appointments.size(); i++)
        if (appointments.get(i).getPatientID() == in.patientID &&
            Validator::strEq(appointments.get(i).getStatus(), "pending"))
        {
            bufCpy(outBuf, "Cannot discharge patient with pending appointments.", outBufSz);
            return false;
        }

    FileHandler::dischargePatient(in.patientID, patients, appointments,
                                  bills, prescriptions);
    bufCpy(outBuf, "Patient discharged and archived successfully.", outBufSz);
    return true;
}




bool AdminMenu::viewSecurityLog(char *outBuf, int outBufSz)
{
    outBuf[0] = '\0';

    
    
    FILE *f = fopen("data/security_log.txt", "r");
    if (!f)
    {
        bufCpy(outBuf, "Security log file not found.", outBufSz);
        return false;
    }
    int pos = 0;
    int c;
    while ((c = fgetc(f)) != EOF && pos < outBufSz - 1)
        outBuf[pos++] = (char)c;
    outBuf[pos] = '\0';
    fclose(f);

    if (pos == 0)
        bufCpy(outBuf, "Security log is empty.", outBufSz);
    return true;
}




bool AdminMenu::generateDailyReport(const Storage<Appointment> &appointments,
                                    const Storage<Bill>        &bills,
                                    const Storage<Patient>     &patients,
                                    const Storage<Doctor>      &doctors,
                                    char *outBuf, int outBufSz)
{
    char today[11]; getTodayDate(today);
    outBuf[0] = '\0';

    int totalToday = 0, pendingN = 0, completedN = 0, noshowN = 0, cancelledN = 0;
    float revenueToday = 0.0f;

    for (int i = 0; i < appointments.size(); i++)
    {
        const Appointment &ap = appointments.get(i);
        if (!Validator::strEq(ap.getDate(), today)) continue;
        totalToday++;
        if      (Validator::strEq(ap.getStatus(), "pending"))   pendingN++;
        else if (Validator::strEq(ap.getStatus(), "completed")) completedN++;
        else if (Validator::strEq(ap.getStatus(), "no-show"))   noshowN++;
        else if (Validator::strEq(ap.getStatus(), "cancelled")) cancelledN++;
    }

    for (int i = 0; i < bills.size(); i++)
    {
        const Bill &b = bills.get(i);
        if (Validator::strEq(b.getStatus(), "paid") &&
            Validator::strEq(b.getDate(),   today))
            revenueToday += b.getAmount();
    }

    bufCpy(outBuf, "══════  Daily Report: ", outBufSz);
    bufCat(outBuf, today, outBufSz);
    bufCat(outBuf, "  ══════\n\n", outBufSz);

    bufCat(outBuf, "Appointments today: ", outBufSz); bufCatInt(outBuf, totalToday, outBufSz);
    bufCat(outBuf, "\n  Pending:   ", outBufSz);      bufCatInt(outBuf, pendingN, outBufSz);
    bufCat(outBuf, "\n  Completed: ", outBufSz);      bufCatInt(outBuf, completedN, outBufSz);
    bufCat(outBuf, "\n  No-show:   ", outBufSz);      bufCatInt(outBuf, noshowN, outBufSz);
    bufCat(outBuf, "\n  Cancelled: ", outBufSz);      bufCatInt(outBuf, cancelledN, outBufSz);
    bufCat(outBuf, "\n\nRevenue collected today (paid bills): PKR ", outBufSz);
    bufCatFloat(outBuf, revenueToday, outBufSz);
    bufCat(outBuf, "\n\nPatients with outstanding unpaid bills:\n", outBufSz);
    bufCat(outBuf,
           "Patient Name         | Total Owed\n"
           "──────────────────────────────────\n",
           outBufSz);

    bool anyUnpaid = false;
    for (int i = 0; i < patients.size(); i++)
    {
        float owed = 0.0f;
        for (int j = 0; j < bills.size(); j++)
            if (bills.get(j).getPatientID() == patients.get(i).getID() &&
                Validator::strEq(bills.get(j).getStatus(), "unpaid"))
                owed += bills.get(j).getAmount();
        if (owed > 0.0f)
        {
            bufCat(outBuf, patients.get(i).getName(), outBufSz);
            bufCat(outBuf, "  |  PKR ", outBufSz);
            bufCatFloat(outBuf, owed, outBufSz);
            bufCat(outBuf, "\n", outBufSz);
            anyUnpaid = true;
        }
    }
    if (!anyUnpaid)
        bufCat(outBuf, "None\n", outBufSz);

    bufCat(outBuf, "\nDoctor-wise summary for today:\n", outBufSz);
    bufCat(outBuf,
           "Doctor Name          | Completed | Pending | No-show\n"
           "──────────────────────────────────────────────────────\n",
           outBufSz);

    for (int i = 0; i < doctors.size(); i++)
    {
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < appointments.size(); j++)
        {
            const Appointment &ap = appointments.get(j);
            if (ap.getDoctorID() != doctors.get(i).getID()) continue;
            if (!Validator::strEq(ap.getDate(), today)) continue;
            if      (Validator::strEq(ap.getStatus(), "completed")) dc++;
            else if (Validator::strEq(ap.getStatus(), "pending"))   dp++;
            else if (Validator::strEq(ap.getStatus(), "no-show"))   dn++;
        }
        if (dc + dp + dn > 0)
        {
            bufCat(outBuf, doctors.get(i).getName(), outBufSz);
            bufCat(outBuf, "  |  ", outBufSz); bufCatInt(outBuf, dc, outBufSz);
            bufCat(outBuf, "  |  ", outBufSz); bufCatInt(outBuf, dp, outBufSz);
            bufCat(outBuf, "  |  ", outBufSz); bufCatInt(outBuf, dn, outBufSz);
            bufCat(outBuf, "\n", outBufSz);
        }
    }
    return true;
}