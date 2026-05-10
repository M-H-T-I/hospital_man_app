#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <ostream>

class Appointment
{
private:
    int appointmentID;
    int patientID;
    int doctorID;
    char date[11];
    char timeSlot[6];
    char status[12];

public:
    Appointment();
    Appointment(int appID, int patID, int docID,
                const char *date, const char *timeSlot, const char *status);

    int getAppointmentID() const { return appointmentID; }
    int getPatientID() const { return patientID; }
    int getDoctorID() const { return doctorID; }
    const char *getDate() const { return date; }
    const char *getTimeSlot() const { return timeSlot; }
    const char *getStatus() const { return status; }

    void setStatus(const char *s);
    void setDate(const char *d);
    void setTimeSlot(const char *t);

    bool operator==(const Appointment &other) const;
    friend std::ostream &operator<<(std::ostream &os, const Appointment &a);

    void toCSV(char *buf, int bufSize) const;
};

#endif