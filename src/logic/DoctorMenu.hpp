#ifndef DOCTORMENU_H
#define DOCTORMENU_H

// =============================================================================
// DoctorMenu.h
// All Doctor role menu operations.
// =============================================================================
#include "Storage.hpp"
#include "Doctor.hpp"
#include "Patient.hpp"
#include "Appointment.hpp"
#include "Prescription.hpp"
#include "Bill.hpp"

class DoctorMenu
{
public:
    static void run(Doctor &doctor,
                    Storage<Appointment> &appointments,
                    Storage<Patient> &patients,
                    Storage<Prescription> &prescriptions,
                    Storage<Bill> &bills);

private:
    static void viewTodayAppointments(const Doctor &doctor,
                                      const Storage<Appointment> &appointments,
                                      const Storage<Patient> &patients);

    static void markComplete(const Doctor &doctor,
                             Storage<Appointment> &appointments);

    static void markNoShow(const Doctor &doctor,
                           Storage<Appointment> &appointments,
                           Storage<Bill> &bills);

    static void writePrescription(const Doctor &doctor,
                                  Storage<Appointment> &appointments,
                                  Storage<Prescription> &prescriptions);

    static void viewPatientHistory(const Doctor &doctor,
                                   const Storage<Patient> &patients,
                                   const Storage<Appointment> &appointments,
                                   const Storage<Prescription> &prescriptions);

    // Sort appointments by time slot ascending (manual)
    static void sortByTimeSlotAsc(Appointment *arr, int n);
    static void sortPrescsByDateDesc(Prescription *arr, int n);
    static int compareDates(const char *d1, const char *d2);
    static void getTodayDate(char *buf);
};

#endif // DOCTORMENU_H