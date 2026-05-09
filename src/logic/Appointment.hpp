#include "common.hpp"
#include "utils/CustDate.hpp"

class Appointment
{

    private:

        // Attr of appointment
        int ID;
        CustDate date;     // dd-mm-yy
        char time_slot[6]; // hh::mm
        char *status;      // length may vary so its pointer for now

        // foreign stuff
        int patient_id;
        int doctor_id;

public:
    Appointment();
    Appointment(int ID, int p_id, int d_id, char *status, char t_slot[6], CustDate date);

    // OPEREATOR OVERLOADS:
    bool operator==(Appointment &other);

    // setters and getters:
    int getID();
    CustDate getDate();
    char* getTime();
    char* getStatus();

    int getPatientId();
    int getDoctorId();
};