#pragma once

class Bill {
private:
    int   billID;
    int   patientID;
    int   appointmentID;
    float amount;
    char  status[12];   // unpaid | paid | cancelled
    char  date[11];     // DD-MM-YYYY

public:
    // ── Constructors ─────────────────────────────────────────────────────────
    Bill();
    Bill(int billID, int patientID, int appointmentID,
         float amount, const char* status, const char* date);

    // ── Accessors ─────────────────────────────────────────────────────────────
    int         getBillID()         const { return billID; }
    int         getPatientID()      const { return patientID; }
    int         getAppointmentID()  const { return appointmentID; }
    float       getAmount()         const { return amount; }
    const char* getStatus()         const { return status; }
    const char* getDate()           const { return date; }

    // ── Mutators ──────────────────────────────────────────────────────────────
    void setStatus(const char* s);
    void setBillID(int id)          { billID = id; }

    // ── CSV serialisation ─────────────────────────────────────────────────────
    // bill_id,patient_id,appointment_id,amount,status,date
    void toCSV(char* buf, int bufSize) const;
};

