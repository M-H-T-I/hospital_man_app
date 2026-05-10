#ifndef APPOINTMENT_H
#define APPOINTMENT_H

// =============================================================================
// Appointment.h
// Represents a single appointment.
// Operator overloads: == (conflict detection), <<
// =============================================================================
#include <ostream>

class Appointment {
private:
    int  appointmentID;
    int  patientID;
    int  doctorID;
    char date[11];      // DD-MM-YYYY
    char timeSlot[6];   // HH:MM
    char status[12];    // pending | completed | cancelled | no-show

public:
    // ── Constructors ─────────────────────────────────────────────────────────
    Appointment();
    Appointment(int appID, int patID, int docID,
                const char* date, const char* timeSlot, const char* status);

    // ── Accessors ─────────────────────────────────────────────────────────────
    int         getAppointmentID() const { return appointmentID; }
    int         getPatientID()     const { return patientID; }
    int         getDoctorID()      const { return doctorID; }
    const char* getDate()          const { return date; }
    const char* getTimeSlot()      const { return timeSlot; }
    const char* getStatus()        const { return status; }

    // ── Mutators ──────────────────────────────────────────────────────────────
    void setStatus(const char* s);
    void setDate(const char* d);
    void setTimeSlot(const char* t);

    // ── Operator overloads ────────────────────────────────────────────────────
    // True if same doctor, same date, same time slot, and neither is cancelled
    bool operator==(const Appointment& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Appointment& a);

    // ── CSV serialisation ─────────────────────────────────────────────────────
    // appointment_id,patient_id,doctor_id,date,time_slot,status
    void toCSV(char* buf, int bufSize) const;
};

#endif // APPOINTMENT_H