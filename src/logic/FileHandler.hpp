#ifndef FILEHANDLER_H
#define FILEHANDLER_H

// =============================================================================
// FileHandler.h
// Handles all file I/O: loading, appending, updating, deleting records.
// Supports cascading deletion of related patient records.
// =============================================================================
#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

class FileHandler {
public:
    // ── File paths ────────────────────────────────────────────────────────────
    static const char* PATIENTS_FILE;
    static const char* DOCTORS_FILE;
    static const char* ADMIN_FILE;
    static const char* APPOINTMENTS_FILE;
    static const char* BILLS_FILE;
    static const char* PRESCRIPTIONS_FILE;
    static const char* SECURITY_LOG_FILE;
    static const char* DISCHARGED_FILE;

    // ── Load all entities on startup ─────────────────────────────────────────
    static void loadPatients(Storage<Patient>& store);
    static void loadDoctors(Storage<Doctor>& store);
    static bool loadAdmin(Admin& admin);               // returns false if not found
    static void loadAppointments(Storage<Appointment>& store);
    static void loadBills(Storage<Bill>& store);
    static void loadPrescriptions(Storage<Prescription>& store);

    // ── Append new record ─────────────────────────────────────────────────────
    static void appendPatient(const Patient& p);
    static void appendDoctor(const Doctor& d);
    static void appendAppointment(const Appointment& a);
    static void appendBill(const Bill& b);
    static void appendPrescription(const Prescription& p);

    // ── Rewrite entire file from storage ─────────────────────────────────────
    static void saveAllPatients(const Storage<Patient>& store);
    static void saveAllDoctors(const Storage<Doctor>& store);
    static void saveAllAppointments(const Storage<Appointment>& store);
    static void saveAllBills(const Storage<Bill>& store);
    static void saveAllPrescriptions(const Storage<Prescription>& store);

    // ── Update single record (rewrites entire file) ───────────────────────────
    static void updatePatient(const Patient& p, Storage<Patient>& store);
    static void updateAppointment(const Appointment& a, Storage<Appointment>& store);
    static void updateBill(const Bill& b, Storage<Bill>& store);

    // ── Delete single record ──────────────────────────────────────────────────
    static void deleteDoctor(int doctorID, Storage<Doctor>& store);
    static void deletePatient(int patientID, Storage<Patient>& store);

    // ── Cascading deletion: removes all patient-related records ───────────────
    static void cascadeDeletePatient(int patientID,
                                     Storage<Appointment>& appointments,
                                     Storage<Bill>&        bills,
                                     Storage<Prescription>& prescriptions);

    // ── Discharge: archive + cascade delete ───────────────────────────────────
    static void dischargePatient(int patientID,
                                 Storage<Patient>&      patients,
                                 Storage<Appointment>&  appointments,
                                 Storage<Bill>&         bills,
                                 Storage<Prescription>& prescriptions);

    // ── Security log ─────────────────────────────────────────────────────────
    static void logSecurityEvent(const char* role, const char* enteredID,
                                 const char* result);
    static void displaySecurityLog();

    // ── Helpers ───────────────────────────────────────────────────────────────
    static int getNextPatientID(const Storage<Patient>& store);
    static int getNextDoctorID(const Storage<Doctor>& store);
    static int getNextAppointmentID(const Storage<Appointment>& store);
    static int getNextBillID(const Storage<Bill>& store);
    static int getNextPrescriptionID(const Storage<Prescription>& store);

private:
    // ── Internal CSV parser helpers ───────────────────────────────────────────
    // Splits a CSV line into tokens stored in fields[][].
    // Returns number of fields parsed.
    static int parseLine(const char* line, char fields[][512], int maxFields);
};

#endif // FILEHANDLER_H