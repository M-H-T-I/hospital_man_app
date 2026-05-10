#ifndef ADMINMENU_H
#define ADMINMENU_H








#include "Storage.hpp"
#include "Admin.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"





struct AddDoctorInput
{
    char name[51]     = {};
    char spec[51]     = {};
    char contact[12]  = {};
    char password[51] = {};
    char fee[32]      = {};     
};

struct RemoveDoctorInput
{
    int doctorID = 0;
};

struct AddPatientInput
{
    char name[51]     = {};
    char age[8]       = {};     
    char gender[3]    = {};     
    char contact[12]  = {};
    char password[51] = {};
    char balance[32]  = {};     
};

struct RemovePatientInput
{
    int patientID = 0;
};

struct DischargePatientInput
{
    int patientID = 0;
};





class AdminMenu
{
public:
    static bool addDoctor(Storage<Doctor> &doctors,
                          const AddDoctorInput &in,
                          char *outBuf, int outBufSz);

    static bool removeDoctor(Storage<Doctor>             &doctors,
                             const Storage<Appointment>  &appointments,
                             const RemoveDoctorInput     &in,
                             char *outBuf, int outBufSz);

    static bool addPatient(Storage<Patient>      &patients,
                           const AddPatientInput &in,
                           char *outBuf, int outBufSz);

    static bool removePatient(Storage<Patient>      &patients,
                              Storage<Appointment>  &appointments,
                              Storage<Bill>         &bills,
                              Storage<Prescription> &prescriptions,
                              const RemovePatientInput &in,
                              char *outBuf, int outBufSz);

    
    static bool viewAllPatients(const Storage<Patient> &patients,
                                const Storage<Bill>    &bills,
                                char *outBuf, int outBufSz);

    static bool viewAllDoctors(const Storage<Doctor> &doctors,
                               char *outBuf, int outBufSz);

    static bool viewAllAppointments(const Storage<Appointment> &appointments,
                                    const Storage<Patient>     &patients,
                                    const Storage<Doctor>      &doctors,
                                    char *outBuf, int outBufSz);

    static bool viewUnpaidBills(const Storage<Bill>    &bills,
                                const Storage<Patient> &patients,
                                char *outBuf, int outBufSz);

    static bool dischargePatient(Storage<Patient>      &patients,
                                 Storage<Appointment>  &appointments,
                                 Storage<Bill>         &bills,
                                 Storage<Prescription> &prescriptions,
                                 const DischargePatientInput &in,
                                 char *outBuf, int outBufSz);

    
    static bool viewSecurityLog(char *outBuf, int outBufSz);

    
    static bool generateDailyReport(const Storage<Appointment> &appointments,
                                    const Storage<Bill>        &bills,
                                    const Storage<Patient>     &patients,
                                    const Storage<Doctor>      &doctors,
                                    char *outBuf, int outBufSz);

private:
    static void getTodayDate(char *buf);
    static int  compareDates(const char *d1, const char *d2);
    static void sortAppointmentsByDateDesc(Appointment *arr, int n);
    static bool isOverdue(const char *dateStr);
};

#endif 