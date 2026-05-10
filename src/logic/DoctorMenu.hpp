#ifndef DOCTORMENU_H
#define DOCTORMENU_H

// =============================================================================
// DoctorMenu.hpp  –  SFML-compatible (no cin / cout)
//
// Same contract as PatientMenu: every action is stateless, returns a
// human-readable result string via `outBuf`, and returns true/false.
// =============================================================================

#include "Storage.hpp"
#include "Doctor.hpp"
#include "Patient.hpp"
#include "Appointment.hpp"
#include "Prescription.hpp"
#include "Bill.hpp"

// ---------------------------------------------------------------------------
// Input bundles
// ---------------------------------------------------------------------------

struct MarkAppointmentInput
{
    int appointmentID = 0;
};

struct WritePrescriptionInput
{
    int  appointmentID  = 0;
    char medicines[500] = {};
    char notes[300]     = {};
};

struct ViewPatientHistoryInput
{
    int patientID = 0;
};

// ---------------------------------------------------------------------------
// DoctorMenu
// ---------------------------------------------------------------------------

class DoctorMenu
{
public:
    // Always fills outBuf with today's appointment table; true if any found.
    static bool viewTodayAppointments(const Doctor &doctor,
                                      const Storage<Appointment> &appointments,
                                      const Storage<Patient>     &patients,
                                      char *outBuf, int outBufSz);

    static bool markComplete(const Doctor &doctor,
                             Storage<Appointment> &appointments,
                             const MarkAppointmentInput &in,
                             char *outBuf, int outBufSz);

    static bool markNoShow(const Doctor &doctor,
                           Storage<Appointment> &appointments,
                           Storage<Bill>        &bills,
                           const MarkAppointmentInput &in,
                           char *outBuf, int outBufSz);

    static bool writePrescription(const Doctor &doctor,
                                  Storage<Appointment>  &appointments,
                                  Storage<Prescription> &prescriptions,
                                  const WritePrescriptionInput &in,
                                  char *outBuf, int outBufSz);

    static bool viewPatientHistory(const Doctor &doctor,
                                   const Storage<Patient>      &patients,
                                   const Storage<Appointment>  &appointments,
                                   const Storage<Prescription> &prescriptions,
                                   const ViewPatientHistoryInput &in,
                                   char *outBuf, int outBufSz);

    // ── List helpers for UI wizard panels ────────────────────────────────────

    // Today's pending appointments for Mark Complete / No-Show picker.
    static void listTodayPending(const Doctor &doctor,
                                 const Storage<Appointment> &appointments,
                                 char *outBuf, int outBufSz);

    // All completed appointments for this doctor (for Write Prescription).
    static void listCompletedWithoutPrescription(
                                 const Doctor &doctor,
                                 const Storage<Appointment>  &appointments,
                                 const Storage<Prescription> &prescriptions,
                                 char *outBuf, int outBufSz);

private:
    static void sortByTimeSlotAsc(Appointment *arr, int n);
    static void sortPrescsByDateDesc(Prescription *arr, int n);
    static int  compareDates(const char *d1, const char *d2);
    static void getTodayDate(char *buf);
};

#endif // DOCTORMENU_H