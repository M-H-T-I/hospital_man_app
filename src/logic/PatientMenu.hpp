#ifndef PATIENTMENU_H
#define PATIENTMENU_H

// =============================================================================
// PatientMenu.hpp  –  SFML-compatible (no cin / cout)
//
// Every public action method is stateless:
//   • All data arrives via parameters.
//   • Human-readable results are written into `outBuf` (size `outBufSz`).
//   • Return value: true = success, false = error/validation failure.
//
// "Wizard" actions that need more than one field collect those fields from
// the caller via dedicated plain-struct input bundles (defined below).
// =============================================================================

#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

// ---------------------------------------------------------------------------
// Input bundles – filled in by the UI before calling the action
// ---------------------------------------------------------------------------

struct BookAppointmentInput
{
    char specialization[51] = {};   // step 1: filter doctors
    int  doctorID           = 0;    // step 2: chosen doctor
    char date[16]           = {};   // step 3: DD-MM-YYYY
    char timeSlot[8]        = {};   // step 4: HH:MM
};

struct CancelAppointmentInput
{
    int appointmentID = 0;
};

struct PayBillInput
{
    int billID = 0;
};

struct TopUpInput
{
    char amount[32] = {};           // positive float string
};

// ---------------------------------------------------------------------------
// PatientMenu
// ---------------------------------------------------------------------------

class PatientMenu
{
public:
    // ── Multi-step helpers used by the UI to populate wizard state ──────────

    // Returns a newline-separated list of doctors matching `specialization`.
    // outBuf is filled; returns number of matches found.
    static int listDoctorsBySpec(const Storage<Doctor> &doctors,
                                 const char *specialization,
                                 char *outBuf, int outBufSz);

    // Returns available time slots for doctorID on dateStr as a
    // newline-separated string. Returns slot count still free.
    static int listAvailableSlots(const Storage<Appointment> &appointments,
                                  int doctorID, const char *dateStr,
                                  char *outBuf, int outBufSz);

    // ── Actions (each writes human-readable result into outBuf) ─────────────

    static bool bookAppointment(Patient &patient,
                                Storage<Doctor>      &doctors,
                                Storage<Appointment> &appointments,
                                Storage<Bill>        &bills,
                                Storage<Patient>     &patients,
                                const BookAppointmentInput &in,
                                char *outBuf, int outBufSz);

    static bool cancelAppointment(Patient &patient,
                                  Storage<Appointment> &appointments,
                                  Storage<Bill>        &bills,
                                  Storage<Doctor>      &doctors,
                                  Storage<Patient>     &patients,
                                  const CancelAppointmentInput &in,
                                  char *outBuf, int outBufSz);

    // Fills outBuf with a formatted table; always returns true.
    static bool viewMyAppointments(const Patient &patient,
                                   const Storage<Appointment> &appointments,
                                   const Storage<Doctor>      &doctors,
                                   char *outBuf, int outBufSz);

    static bool viewMyMedicalRecords(const Patient &patient,
                                     const Storage<Prescription> &prescriptions,
                                     const Storage<Doctor>       &doctors,
                                     char *outBuf, int outBufSz);

    static bool viewMyBills(const Patient &patient,
                            const Storage<Bill> &bills,
                            char *outBuf, int outBufSz);

    static bool payBill(Patient &patient,
                        Storage<Bill>    &bills,
                        Storage<Patient> &patients,
                        const PayBillInput &in,
                        char *outBuf, int outBufSz);

    static bool topUpBalance(Patient &patient,
                             Storage<Patient> &patients,
                             const TopUpInput &in,
                             char *outBuf, int outBufSz);

    // ── List helpers (used by UI to populate combo / selection panels) ───────

    // Fills outBuf with pending appointments for display in Cancel wizard.
    static void listPendingAppointments(const Patient &patient,
                                        const Storage<Appointment> &appointments,
                                        const Storage<Doctor>      &doctors,
                                        char *outBuf, int outBufSz);

    // Fills outBuf with unpaid bills for display in Pay Bill wizard.
    static void listUnpaidBills(const Patient &patient,
                                const Storage<Bill> &bills,
                                char *outBuf, int outBufSz);

private:
    static void sortAppointmentsByDateAsc(Appointment *arr, int n);
    static void sortPrescriptionsByDateDesc(Prescription *arr, int n);
    static int  compareDates(const char *d1, const char *d2);
};

#endif // PATIENTMENU_H