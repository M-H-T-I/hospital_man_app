#pragma once


class Prescription {
private:
    int  prescriptionID;
    int  appointmentID;
    int  patientID;
    int  doctorID;
    char date[11];         
    char medicines[500];
    char notes[300];

public:
    
    Prescription();
    Prescription(int prescID, int appID, int patID, int docID,
                 const char* date, const char* medicines, const char* notes);

    
    int         getPrescriptionID() const { return prescriptionID; }
    int         getAppointmentID()  const { return appointmentID; }
    int         getPatientID()      const { return patientID; }
    int         getDoctorID()       const { return doctorID; }
    const char* getDate()           const { return date; }
    const char* getMedicines()      const { return medicines; }
    const char* getNotes()          const { return notes; }

    void setPrescriptionID(int id)  { prescriptionID = id; }

    
    
    void toCSV(char* buf, int bufSize) const;
};
