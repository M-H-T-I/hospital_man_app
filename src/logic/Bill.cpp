// =============================================================================
// Bill.cpp
// =============================================================================
#include "Bill.hpp"
#include "Validator.hpp"

Bill::Bill() : billID(0), patientID(0), appointmentID(0), amount(0.0f) {
    status[0] = date[0] = '\0';
}

Bill::Bill(int bID, int pID, int aID, float amt, const char* st, const char* d)
    : billID(bID), patientID(pID), appointmentID(aID), amount(amt) {
    Validator::textCpy(status, st, 12);
    Validator::textCpy(date,   d,  11);
}

void Bill::setStatus(const char* s) { Validator::textCpy(status, s, 12); }

// ── CSV ───────────────────────────────────────────────────────────────────────
// bill_id,patient_id,appointment_id,amount,status,date

void Bill::toCSV(char* buf, int bufSize) const {
    char bID[16], pID[16], aID[16], amtBuf[32];
    Validator::intToStr(billID,        bID,    16);
    Validator::intToStr(patientID,     pID,    16);
    Validator::intToStr(appointmentID, aID,    16);
    Validator::floatToStr(amount,      amtBuf, 32, 2);

    buf[0] = '\0';
    Validator::strCat(buf, bID,    bufSize);
    Validator::strCat(buf, ",",    bufSize);
    Validator::strCat(buf, pID,    bufSize);
    Validator::strCat(buf, ",",    bufSize);
    Validator::strCat(buf, aID,    bufSize);
    Validator::strCat(buf, ",",    bufSize);
    Validator::strCat(buf, amtBuf, bufSize);
    Validator::strCat(buf, ",",    bufSize);
    Validator::strCat(buf, status, bufSize);
    Validator::strCat(buf, ",",    bufSize);
    Validator::strCat(buf, date,   bufSize);
}