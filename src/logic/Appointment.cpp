#include "Appointment.hpp"

Appointment::Appointment(){
    
    doctor_id= patient_id = ID = -1;

    date;

    time_slot[0] = '\0';
    
    status = 0;

}


Appointment::Appointment(int ID, int p_id, int d_id, char* status, char t_slot[6], CustDate date){

    this->ID = ID;
    this->patient_id = p_id;
    this->doctor_id = d_id;
    this->status = status;
    
    for(int i = 0; i < 6; i++){

        time_slot[i] = t_slot[i];

    }

    date = CustDate(date.day, date.month, date.year);


}

bool Appointment::operator==(Appointment& other){

    if(this->date == other.date && time_slot == other.time_slot) return true;

    return false;

}

int Appointment::getID(){
    return ID;
}

int Appointment::getDoctorId(){
    return doctor_id;
}

int Appointment::getPatientId(){
    return patient_id;
}