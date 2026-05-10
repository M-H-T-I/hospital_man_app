#include "Prescription.hpp"
#include "Validator.hpp"

Prescription::Prescription()
    : prescriptionID(0), appointmentID(0), patientID(0), doctorID(0)
{
    date[0] = medicines[0] = notes[0] = '\0';
}

Prescription::Prescription(int prescID, int appID, int patID, int docID,
                           const char *d, const char *meds, const char *n)
    : prescriptionID(prescID), appointmentID(appID),
      patientID(patID), doctorID(docID)
{
    Validator::textCpy(date, d, 11);
    Validator::textCpy(medicines, meds, 500);
    Validator::textCpy(notes, n, 300);
}



void Prescription::toCSV(char *buf, int bufSize) const
{
    char prID[16], apID[16], ptID[16], drID[16];
    Validator::intToStr(prescriptionID, prID, 16);
    Validator::intToStr(appointmentID, apID, 16);
    Validator::intToStr(patientID, ptID, 16);
    Validator::intToStr(doctorID, drID, 16);

    buf[0] = '\0';
    Validator::strCat(buf, prID, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, apID, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, ptID, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, drID, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, date, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, medicines, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, notes, bufSize);
}