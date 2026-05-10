#ifndef DOCTORMENU_H
#define DOCTORMENU_H








#include "Storage.hpp"
#include "Doctor.hpp"
#include "Patient.hpp"
#include "Appointment.hpp"
#include "Prescription.hpp"
#include "Bill.hpp"





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





class DoctorMenu
{
public:
    
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

    

    
    static void listTodayPending(const Doctor &doctor,
                                 const Storage<Appointment> &appointments,
                                 char *outBuf, int outBufSz);

    
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

#endif 