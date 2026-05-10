// =============================================================================
// FileHandler.cpp
// =============================================================================
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "HospitalException.hpp"
#include <cstdio>
#include <ctime>
#include <iostream>

// ── File paths ────────────────────────────────────────────────────────────────
const char *FileHandler::PATIENTS_FILE = "data/patients.txt";
const char *FileHandler::DOCTORS_FILE = "data/doctors.txt";
const char *FileHandler::ADMIN_FILE = "data/admin.txt";
const char *FileHandler::APPOINTMENTS_FILE = "data/appointments.txt";
const char *FileHandler::BILLS_FILE = "data/bills.txt";
const char *FileHandler::PRESCRIPTIONS_FILE = "data/prescriptions.txt";
const char *FileHandler::SECURITY_LOG_FILE = "data/security_log.txt";
const char *FileHandler::DISCHARGED_FILE = "data/discharged.txt";

// ─────────────────────────────────────────────────────────────────────────────
// parseLine: splits comma-separated line into fields array.
// Handles the special case of prescriptions where notes/medicines may
// not contain commas (per spec format).
// Returns count of fields found.
// ─────────────────────────────────────────────────────────────────────────────
int FileHandler::parseLine(const char *line, char fields[][512], int maxFields)
{
    int fieldIdx = 0;
    int charIdx = 0;
    int fieldLen = 0;

    while (line[charIdx] != '\0' && line[charIdx] != '\n' && line[charIdx] != '\r')
    {
        if (line[charIdx] == ',' && fieldIdx < maxFields - 1)
        {
            fields[fieldIdx][fieldLen] = '\0';
            fieldIdx++;
            fieldLen = 0;
        }
        else
        {
            if (fieldLen < 511)
            {
                fields[fieldIdx][fieldLen++] = line[charIdx];
            }
        }
        charIdx++;
    }
    fields[fieldIdx][fieldLen] = '\0';
    return fieldIdx + 1;
}

// ─────────────────────────────────────────────────────────────────────────────
// LOAD FUNCTIONS
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::loadPatients(Storage<Patient> &store)
{
    FILE *f = fopen(PATIENTS_FILE, "r");
    if (!f)
        return; // file may not exist yet on first run
    char line[1024];
    bool firstLine = true;
    while (fgets(line, sizeof(line), f))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        } // skip header
        Validator::trim(line);
        if (line[0] == '\0')
            continue;
        char fields[8][512];
        int n = parseLine(line, fields, 8);
        if (n < 7)
            continue;
        // patient_id,name,age,gender,contact,password,balance
        int id = Validator::strToInt(fields[0]);
        int age = Validator::strToInt(fields[2]);
        float balance = Validator::strToFloat(fields[6]);
        Patient p(id, fields[1], age, fields[3], fields[4], fields[5], balance);
        store.add(p);
    }
    fclose(f);
}

void FileHandler::loadDoctors(Storage<Doctor> &store)
{
    FILE *f = fopen(DOCTORS_FILE, "r");
    if (!f)
        return;
    char line[512];
    bool firstLine = true;
    while (fgets(line, sizeof(line), f))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        Validator::trim(line);
        if (line[0] == '\0')
            continue;
        char fields[7][512];
        int n = parseLine(line, fields, 7);
        if (n < 6)
            continue;
        // doctor_id,name,specialization,contact,password,fee
        int id = Validator::strToInt(fields[0]);
        float fee = Validator::strToFloat(fields[5]);
        Doctor d(id, fields[1], fields[2], fields[3], fields[4], fee);
        store.add(d);
    }
    fclose(f);
}

bool FileHandler::loadAdmin(Admin &admin)
{
    FILE *f = fopen(ADMIN_FILE, "r");
    if (!f)
        return false;
    char line[256];
    bool firstLine = true;
    while (fgets(line, sizeof(line), f))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        Validator::trim(line);
        if (line[0] == '\0')
            continue;
        char fields[4][512];
        int n = parseLine(line, fields, 4);
        if (n < 3)
            continue;
        // admin_id,name,password
        int id = Validator::strToInt(fields[0]);
        admin = Admin(id, fields[1], fields[2]);
        fclose(f);
        return true;
    }
    fclose(f);
    return false;
}

void FileHandler::loadAppointments(Storage<Appointment> &store)
{
    FILE *f = fopen(APPOINTMENTS_FILE, "r");
    if (!f)
        return;
    char line[256];
    bool firstLine = true;
    while (fgets(line, sizeof(line), f))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        Validator::trim(line);
        if (line[0] == '\0')
            continue;
        char fields[7][512];
        int n = parseLine(line, fields, 7);
        if (n < 6)
            continue;
        // appointment_id,patient_id,doctor_id,date,time_slot,status
        int appID = Validator::strToInt(fields[0]);
        int patID = Validator::strToInt(fields[1]);
        int docID = Validator::strToInt(fields[2]);
        Appointment a(appID, patID, docID, fields[3], fields[4], fields[5]);
        store.add(a);
    }
    fclose(f);
}

void FileHandler::loadBills(Storage<Bill> &store)
{
    FILE *f = fopen(BILLS_FILE, "r");
    if (!f)
        return;
    char line[256];
    bool firstLine = true;
    while (fgets(line, sizeof(line), f))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        Validator::trim(line);
        if (line[0] == '\0')
            continue;
        char fields[7][512];
        int n = parseLine(line, fields, 7);
        if (n < 6)
            continue;
        // bill_id,patient_id,appointment_id,amount,status,date
        int billID = Validator::strToInt(fields[0]);
        int patID = Validator::strToInt(fields[1]);
        int appID = Validator::strToInt(fields[2]);
        float amt = Validator::strToFloat(fields[3]);
        Bill b(billID, patID, appID, amt, fields[4], fields[5]);
        store.add(b);
    }
    fclose(f);
}

void FileHandler::loadPrescriptions(Storage<Prescription> &store)
{
    FILE *f = fopen(PRESCRIPTIONS_FILE, "r");
    if (!f)
        return;
    char line[1024];
    bool firstLine = true;
    while (fgets(line, sizeof(line), f))
    {
        if (firstLine)
        {
            firstLine = false;
            continue;
        }
        Validator::trim(line);
        if (line[0] == '\0')
            continue;
        // prescription_id,appointment_id,patient_id,doctor_id,date,medicines,notes
        // medicines and notes may contain semicolons but NOT commas per spec
        char fields[8][512];
        int n = parseLine(line, fields, 8);
        if (n < 7)
            continue;
        int prescID = Validator::strToInt(fields[0]);
        int appID = Validator::strToInt(fields[1]);
        int patID = Validator::strToInt(fields[2]);
        int docID = Validator::strToInt(fields[3]);
        Prescription p(prescID, appID, patID, docID, fields[4], fields[5], fields[6]);
        store.add(p);
    }
    fclose(f);
}

// ─────────────────────────────────────────────────────────────────────────────
// SAVE ALL (rewrites entire file)
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::saveAllPatients(const Storage<Patient> &store)
{
    FILE *f = fopen(PATIENTS_FILE, "w");
    if (!f)
        return;
    fprintf(f, "patient_id,name,age,gender,contact,password,balance\n");
    char buf[512];
    for (int i = 0; i < store.size(); i++)
    {
        store.get(i).toCSV(buf, 512);
        fprintf(f, "%s\n", buf);
    }
    fclose(f);
}

void FileHandler::saveAllDoctors(const Storage<Doctor> &store)
{
    FILE *f = fopen(DOCTORS_FILE, "w");
    if (!f)
        return;
    fprintf(f, "doctor_id,name,specialization,contact,password,fee\n");
    char buf[512];
    for (int i = 0; i < store.size(); i++)
    {
        store.get(i).toCSV(buf, 512);
        fprintf(f, "%s\n", buf);
    }
    fclose(f);
}

void FileHandler::saveAllAppointments(const Storage<Appointment> &store)
{
    FILE *f = fopen(APPOINTMENTS_FILE, "w");
    if (!f)
        return;
    fprintf(f, "appointment_id,patient_id,doctor_id,date,time_slot,status\n");
    char buf[256];
    for (int i = 0; i < store.size(); i++)
    {
        store.get(i).toCSV(buf, 256);
        fprintf(f, "%s\n", buf);
    }
    fclose(f);
}

void FileHandler::saveAllBills(const Storage<Bill> &store)
{
    FILE *f = fopen(BILLS_FILE, "w");
    if (!f)
        return;
    fprintf(f, "bill_id,patient_id,appointment_id,amount,status,date\n");
    char buf[256];
    for (int i = 0; i < store.size(); i++)
    {
        store.get(i).toCSV(buf, 256);
        fprintf(f, "%s\n", buf);
    }
    fclose(f);
}

void FileHandler::saveAllPrescriptions(const Storage<Prescription> &store)
{
    FILE *f = fopen(PRESCRIPTIONS_FILE, "w");
    if (!f)
        return;
    fprintf(f, "prescription_id,appointment_id,patient_id,doctor_id,date,medicines,notes\n");
    char buf[1024];
    for (int i = 0; i < store.size(); i++)
    {
        store.get(i).toCSV(buf, 1024);
        fprintf(f, "%s\n", buf);
    }
    fclose(f);
}

// ─────────────────────────────────────────────────────────────────────────────
// APPEND
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::appendPatient(const Patient &p)
{
    FILE *f = fopen(PATIENTS_FILE, "a");
    if (!f)
        return;
    char buf[512];
    p.toCSV(buf, 512);
    fprintf(f, "%s\n", buf);
    fclose(f);
}

void FileHandler::appendDoctor(const Doctor &d)
{
    FILE *f = fopen(DOCTORS_FILE, "a");
    if (!f)
        return;
    char buf[512];
    d.toCSV(buf, 512);
    fprintf(f, "%s\n", buf);
    fclose(f);
}

void FileHandler::appendAppointment(const Appointment &a)
{
    FILE *f = fopen(APPOINTMENTS_FILE, "a");
    if (!f)
        return;
    char buf[256];
    a.toCSV(buf, 256);
    fprintf(f, "%s\n", buf);
    fclose(f);
}

void FileHandler::appendBill(const Bill &b)
{
    FILE *f = fopen(BILLS_FILE, "a");
    if (!f)
        return;
    char buf[256];
    b.toCSV(buf, 256);
    fprintf(f, "%s\n", buf);
    fclose(f);
}

void FileHandler::appendPrescription(const Prescription &p)
{
    FILE *f = fopen(PRESCRIPTIONS_FILE, "a");
    if (!f)
        return;
    char buf[1024];
    p.toCSV(buf, 1024);
    fprintf(f, "%s\n", buf);
    fclose(f);
}

// ─────────────────────────────────────────────────────────────────────────────
// UPDATE
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::updatePatient(const Patient &p, Storage<Patient> &store)
{
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getID() == p.getID())
        {
            store.get(i) = p;
            break;
        }
    }
    saveAllPatients(store);
}

void FileHandler::updateAppointment(const Appointment &a, Storage<Appointment> &store)
{
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getAppointmentID() == a.getAppointmentID())
        {
            store.get(i) = a;
            break;
        }
    }
    saveAllAppointments(store);
}

void FileHandler::updateBill(const Bill &b, Storage<Bill> &store)
{
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getBillID() == b.getBillID())
        {
            store.get(i) = b;
            break;
        }
    }
    saveAllBills(store);
}

// ─────────────────────────────────────────────────────────────────────────────
// DELETE
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::deleteDoctor(int doctorID, Storage<Doctor> &store)
{
    store.removeByID(doctorID);
    saveAllDoctors(store);
}

void FileHandler::deletePatient(int patientID, Storage<Patient> &store)
{
    store.removeByID(patientID);
    saveAllPatients(store);
}

// ─────────────────────────────────────────────────────────────────────────────
// CASCADE DELETE
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::cascadeDeletePatient(int patientID,
                                       Storage<Appointment> &appointments,
                                       Storage<Bill> &bills,
                                       Storage<Prescription> &prescriptions)
{
    // Remove all appointments for this patient
    for (int i = appointments.size() - 1; i >= 0; i--)
    {
        if (appointments.get(i).getPatientID() == patientID)
        {
            appointments.removeAt(i);
        }
    }
    saveAllAppointments(appointments);

    // Remove all bills for this patient
    for (int i = bills.size() - 1; i >= 0; i--)
    {
        if (bills.get(i).getPatientID() == patientID)
        {
            bills.removeAt(i);
        }
    }
    saveAllBills(bills);

    // Remove all prescriptions for this patient
    for (int i = prescriptions.size() - 1; i >= 0; i--)
    {
        if (prescriptions.get(i).getPatientID() == patientID)
        {
            prescriptions.removeAt(i);
        }
    }
    saveAllPrescriptions(prescriptions);
}

// ─────────────────────────────────────────────────────────────────────────────
// DISCHARGE PATIENT
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::dischargePatient(int patientID,
                                   Storage<Patient> &patients,
                                   Storage<Appointment> &appointments,
                                   Storage<Bill> &bills,
                                   Storage<Prescription> &prescriptions)
{
    // Find and archive patient record
    Patient *p = patients.findByID(patientID);
    if (!p)
        return;

    FILE *df = fopen(DISCHARGED_FILE, "a");
    if (df)
    {
        char buf[512];
        p->toCSV(buf, 512);
        fprintf(df, "%s\n", buf);
        fclose(df);
    }

    // Archive all appointments
    FILE *af = fopen(DISCHARGED_FILE, "a");
    if (af)
    {
        char buf[256];
        for (int i = 0; i < appointments.size(); i++)
        {
            if (appointments.get(i).getPatientID() == patientID)
            {
                appointments.get(i).toCSV(buf, 256);
                fprintf(af, "%s\n", buf);
            }
        }
        fclose(af);
    }

    // Archive all bills
    FILE *bf = fopen(DISCHARGED_FILE, "a");
    if (bf)
    {
        char buf[256];
        for (int i = 0; i < bills.size(); i++)
        {
            if (bills.get(i).getPatientID() == patientID)
            {
                bills.get(i).toCSV(buf, 256);
                fprintf(bf, "%s\n", buf);
            }
        }
        fclose(bf);
    }

    // Archive all prescriptions
    FILE *pf = fopen(DISCHARGED_FILE, "a");
    if (pf)
    {
        char buf[1024];
        for (int i = 0; i < prescriptions.size(); i++)
        {
            if (prescriptions.get(i).getPatientID() == patientID)
            {
                prescriptions.get(i).toCSV(buf, 1024);
                fprintf(pf, "%s\n", buf);
            }
        }
        fclose(pf);
    }

    // Now cascade delete from live files
    deletePatient(patientID, patients);
    cascadeDeletePatient(patientID, appointments, bills, prescriptions);
}

// ─────────────────────────────────────────────────────────────────────────────
// SECURITY LOG
// ─────────────────────────────────────────────────────────────────────────────

void FileHandler::logSecurityEvent(const char *role,
                                   const char *enteredID,
                                   const char *result)
{
    FILE *f = fopen(SECURITY_LOG_FILE, "a");
    if (!f)
        return;

    time_t now = time(0);
    struct tm *t = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%d-%m-%Y %H:%M:%S", t);

    fprintf(f, "%s,%s,%s,%s\n", timestamp, role, enteredID, result);
    fclose(f);
}

void FileHandler::displaySecurityLog()
{
    FILE *f = fopen(SECURITY_LOG_FILE, "r");
    if (!f)
    {
        std::cout << "No security events logged.\n";
        return;
    }
    char line[256];
    bool hasContent = false;
    // Skip header if present (first line check)
    while (fgets(line, sizeof(line), f))
    {
        Validator::trim(line);
        if (line[0] == '\0')
            continue;
        // skip header line
        if (Validator::strEq(line, "timestamp,role,entered_id,result"))
            continue;
        std::cout << line << "\n";
        hasContent = true;
    }
    if (!hasContent)
        std::cout << "No security events logged.\n";
    fclose(f);
}

// ─────────────────────────────────────────────────────────────────────────────
// ID GENERATORS
// ─────────────────────────────────────────────────────────────────────────────

int FileHandler::getNextPatientID(const Storage<Patient> &store)
{
    int maxID = 0;
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getID() > maxID)
            maxID = store.get(i).getID();
    }
    return maxID + 1;
}

int FileHandler::getNextDoctorID(const Storage<Doctor> &store)
{
    int maxID = 0;
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getID() > maxID)
            maxID = store.get(i).getID();
    }
    return maxID + 1;
}

int FileHandler::getNextAppointmentID(const Storage<Appointment> &store)
{
    int maxID = 0;
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getAppointmentID() > maxID)
            maxID = store.get(i).getAppointmentID();
    }
    return maxID + 1;
}

int FileHandler::getNextBillID(const Storage<Bill> &store)
{
    int maxID = 0;
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getBillID() > maxID)
            maxID = store.get(i).getBillID();
    }
    return maxID + 1;
}

int FileHandler::getNextPrescriptionID(const Storage<Prescription> &store)
{
    int maxID = 0;
    for (int i = 0; i < store.size(); i++)
    {
        if (store.get(i).getPrescriptionID() > maxID)
            maxID = store.get(i).getPrescriptionID();
    }
    return maxID + 1;
}