#ifndef ADMINMENU_H
#define ADMINMENU_H

// =============================================================================
// AdminMenu.h
// All Admin role menu operations.
// =============================================================================
#include "Storage.hpp"
#include "Admin.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

class AdminMenu {
public:
    static void run(Admin& admin,
                    Storage<Patient>&      patients,
                    Storage<Doctor>&       doctors,
                    Storage<Appointment>&  appointments,
                    Storage<Bill>&         bills,
                    Storage<Prescription>& prescriptions);

private:
    static void addDoctor(Storage<Doctor>& doctors);
    static void removeDoctor(Storage<Doctor>& doctors,
                              const Storage<Appointment>& appointments);

    static void addPatient(Storage<Patient>& patients);
    static void removePatient(Storage<Patient>&      patients,
                               Storage<Appointment>&  appointments,
                               Storage<Bill>&         bills,
                               Storage<Prescription>& prescriptions);

    static void viewAllPatients(const Storage<Patient>& patients,
                                 const Storage<Bill>&    bills);

    static void viewAllDoctors(const Storage<Doctor>& doctors);

    static void viewAllAppointments(const Storage<Appointment>& appointments,
                                     const Storage<Patient>&     patients,
                                     const Storage<Doctor>&      doctors);

    static void viewUnpaidBills(const Storage<Bill>&    bills,
                                 const Storage<Patient>& patients);

    static void dischargePatient(Storage<Patient>&      patients,
                                  Storage<Appointment>&  appointments,
                                  Storage<Bill>&         bills,
                                  Storage<Prescription>& prescriptions);

    static void viewSecurityLog();
    static void generateDailyReport(const Storage<Appointment>&  appointments,
                                     const Storage<Bill>&         bills,
                                     const Storage<Patient>&      patients,
                                     const Storage<Doctor>&       doctors);

    // Helpers
    static void getTodayDate(char* buf);
    static int  compareDates(const char* d1, const char* d2);
    static void sortAppointmentsByDateDesc(Appointment* arr, int n);
    static bool isOverdue(const char* dateStr);  // >7 days before today
};

#endif // ADMINMENU_H