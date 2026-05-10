#ifndef PATIENTMENU_H
#define PATIENTMENU_H

// =============================================================================
// PatientMenu.h
// All Patient role menu operations.
// =============================================================================
#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

class PatientMenu {
public:
    static void run(Patient& patient,
                    Storage<Doctor>&       doctors,
                    Storage<Appointment>&  appointments,
                    Storage<Bill>&         bills,
                    Storage<Prescription>& prescriptions,
                    Storage<Patient>&      patients);

private:
    static void bookAppointment(Patient& patient,
                                Storage<Doctor>&      doctors,
                                Storage<Appointment>& appointments,
                                Storage<Bill>&        bills,
                                Storage<Patient>&     patients);

    static void cancelAppointment(Patient& patient,
                                   Storage<Appointment>& appointments,
                                   Storage<Bill>&        bills,
                                   Storage<Doctor>&      doctors,
                                   Storage<Patient>&     patients);

    static void viewMyAppointments(const Patient& patient,
                                    const Storage<Appointment>& appointments,
                                    const Storage<Doctor>&      doctors);

    static void viewMyMedicalRecords(const Patient& patient,
                                      const Storage<Prescription>& prescriptions,
                                      const Storage<Doctor>&       doctors,
                                      const Storage<Appointment>&  appointments);

    static void viewMyBills(const Patient& patient,
                             const Storage<Bill>& bills);

    static void payBill(Patient& patient,
                        Storage<Bill>&    bills,
                        Storage<Patient>& patients);

    static void topUpBalance(Patient& patient,
                              Storage<Patient>& patients);

    // Sorting helpers (manual — no library sort)
    static void sortAppointmentsByDateAsc(Appointment* arr, int n);
    static void sortPrescriptionsByDateDesc(Prescription* arr, int n);

    // Date comparison returns -1, 0, 1 for <, =, >
    static int compareDates(const char* d1, const char* d2);
};

#endif // PATIENTMENU_H