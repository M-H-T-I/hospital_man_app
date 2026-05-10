#ifndef PATIENTMENU_H
#define PATIENTMENU_H

#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"

struct BookAppointmentInput
{
    char specialization[51] = {};
    int doctorID = 0;
    char date[16] = {};
    char timeSlot[8] = {};
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
    char amount[32] = {};
};

class PatientMenu
{
public:
    static int listDoctorsBySpec(const Storage<Doctor> &doctors,
                                 const char *specialization,
                                 char *outBuf, int outBufSz);

    static int listAvailableSlots(const Storage<Appointment> &appointments,
                                  int doctorID, const char *dateStr,
                                  char *outBuf, int outBufSz);

    static bool bookAppointment(Patient &patient,
                                Storage<Doctor> &doctors,
                                Storage<Appointment> &appointments,
                                Storage<Bill> &bills,
                                Storage<Patient> &patients,
                                const BookAppointmentInput &in,
                                char *outBuf, int outBufSz);

    static bool cancelAppointment(Patient &patient,
                                  Storage<Appointment> &appointments,
                                  Storage<Bill> &bills,
                                  Storage<Doctor> &doctors,
                                  Storage<Patient> &patients,
                                  const CancelAppointmentInput &in,
                                  char *outBuf, int outBufSz);

    static bool viewMyAppointments(const Patient &patient,
                                   const Storage<Appointment> &appointments,
                                   const Storage<Doctor> &doctors,
                                   char *outBuf, int outBufSz);

    static bool viewMyMedicalRecords(const Patient &patient,
                                     const Storage<Prescription> &prescriptions,
                                     const Storage<Doctor> &doctors,
                                     char *outBuf, int outBufSz);

    static bool viewMyBills(const Patient &patient,
                            const Storage<Bill> &bills,
                            char *outBuf, int outBufSz);

    static bool payBill(Patient &patient,
                        Storage<Bill> &bills,
                        Storage<Patient> &patients,
                        const PayBillInput &in,
                        char *outBuf, int outBufSz);

    static bool topUpBalance(Patient &patient,
                             Storage<Patient> &patients,
                             const TopUpInput &in,
                             char *outBuf, int outBufSz);

    static void listPendingAppointments(const Patient &patient,
                                        const Storage<Appointment> &appointments,
                                        const Storage<Doctor> &doctors,
                                        char *outBuf, int outBufSz);

    static void listUnpaidBills(const Patient &patient,
                                const Storage<Bill> &bills,
                                char *outBuf, int outBufSz);

private:
    static void sortAppointmentsByDateAsc(Appointment *arr, int n);
    static void sortPrescriptionsByDateDesc(Prescription *arr, int n);
    static int compareDates(const char *d1, const char *d2);
};

#endif