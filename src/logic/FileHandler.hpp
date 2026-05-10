#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

class FileHandler
{
public:
    static const char *PATIENTS_FILE;
    static const char *DOCTORS_FILE;
    static const char *ADMIN_FILE;
    static const char *APPOINTMENTS_FILE;
    static const char *BILLS_FILE;
    static const char *PRESCRIPTIONS_FILE;
    static const char *SECURITY_LOG_FILE;
    static const char *DISCHARGED_FILE;

    static void loadPatients(Storage<Patient> &store);
    static void loadDoctors(Storage<Doctor> &store);
    static bool loadAdmin(Admin &admin);
    static void loadAppointments(Storage<Appointment> &store);
    static void loadBills(Storage<Bill> &store);
    static void loadPrescriptions(Storage<Prescription> &store);

    static void appendPatient(const Patient &p);
    static void appendDoctor(const Doctor &d);
    static void appendAppointment(const Appointment &a);
    static void appendBill(const Bill &b);
    static void appendPrescription(const Prescription &p);

    static void saveAllPatients(const Storage<Patient> &store);
    static void saveAllDoctors(const Storage<Doctor> &store);
    static void saveAllAppointments(const Storage<Appointment> &store);
    static void saveAllBills(const Storage<Bill> &store);
    static void saveAllPrescriptions(const Storage<Prescription> &store);

    static void updatePatient(const Patient &p, Storage<Patient> &store);
    static void updateAppointment(const Appointment &a, Storage<Appointment> &store);
    static void updateBill(const Bill &b, Storage<Bill> &store);

    static void deleteDoctor(int doctorID, Storage<Doctor> &store);
    static void deletePatient(int patientID, Storage<Patient> &store);

    static void cascadeDeletePatient(int patientID,
                                     Storage<Appointment> &appointments,
                                     Storage<Bill> &bills,
                                     Storage<Prescription> &prescriptions);

    static void dischargePatient(int patientID,
                                 Storage<Patient> &patients,
                                 Storage<Appointment> &appointments,
                                 Storage<Bill> &bills,
                                 Storage<Prescription> &prescriptions);

    static void logSecurityEvent(const char *role, const char *enteredID,
                                 const char *result);
    static void displaySecurityLog();

    static int getNextPatientID(const Storage<Patient> &store);
    static int getNextDoctorID(const Storage<Doctor> &store);
    static int getNextAppointmentID(const Storage<Appointment> &store);
    static int getNextBillID(const Storage<Bill> &store);
    static int getNextPrescriptionID(const Storage<Prescription> &store);

private:
    static int parseLine(const char *line, char fields[][512], int maxFields);
};

#endif