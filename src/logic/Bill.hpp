#pragma once

class Bill
{
private:
    int billID;
    int patientID;
    int appointmentID;
    float amount;
    char status[12];
    char date[11];

public:
    Bill();
    Bill(int billID, int patientID, int appointmentID,
         float amount, const char *status, const char *date);

    int getBillID() const { return billID; }
    int getPatientID() const { return patientID; }
    int getAppointmentID() const { return appointmentID; }
    float getAmount() const { return amount; }
    const char *getStatus() const { return status; }
    const char *getDate() const { return date; }

    void setStatus(const char *s);
    void setBillID(int id) { billID = id; }

    void toCSV(char *buf, int bufSize) const;
};
