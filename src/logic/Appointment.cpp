// =============================================================================
// Appointment.cpp
// =============================================================================
#include "Appointment.hpp"
#include "Validator.hpp"
#include <iostream>

Appointment::Appointment()
    : appointmentID(0), patientID(0), doctorID(0)
{
    date[0] = timeSlot[0] = status[0] = '\0';
}

Appointment::Appointment(int appID, int patID, int docID, const char *d, const char *ts, const char *st)
    : appointmentID(appID), patientID(patID), doctorID(docID)
{
    Validator::textCpy(date, d, 11);
    Validator::textCpy(timeSlot, ts, 6);
    Validator::textCpy(status, st, 12);
}

void Appointment::setStatus(const char *s) { Validator::textCpy(status, s, 12); }
void Appointment::setDate(const char *d) { Validator::textCpy(date, d, 11); }
void Appointment::setTimeSlot(const char *t) { Validator::textCpy(timeSlot, t, 6); }

// ── Operator == ───────────────────────────────────────────────────────────────
// Conflict: same doctor, same date, same time slot, neither is cancelled

bool Appointment::operator==(const Appointment &other) const
{
    if (doctorID != other.doctorID)
        return false;
    if (!Validator::strEq(date, other.date))
        return false;
    if (!Validator::strEq(timeSlot, other.timeSlot))
        return false;
    if (Validator::strEq(status, "cancelled"))
        return false;
    if (Validator::strEq(other.status, "cancelled"))
        return false;
    return true;
}

std::ostream &operator<<(std::ostream &os, const Appointment &a)
{
    os << "Appointment ID: " << a.appointmentID
       << " | Patient ID: " << a.patientID
       << " | Doctor ID: " << a.doctorID
       << " | Date: " << a.date
       << " | Time: " << a.timeSlot
       << " | Status: " << a.status;
    return os;
}

// ── CSV ───────────────────────────────────────────────────────────────────────
// appointment_id,patient_id,doctor_id,date,time_slot,status

void Appointment::toCSV(char *buf, int bufSize) const
{
    char aID[16], pID[16], dID[16];
    Validator::intToStr(appointmentID, aID, 16);
    Validator::intToStr(patientID, pID, 16);
    Validator::intToStr(doctorID, dID, 16);

    buf[0] = '\0';
    Validator::strCat(buf, aID, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, pID, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, dID, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, date, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, timeSlot, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, status, bufSize);
}