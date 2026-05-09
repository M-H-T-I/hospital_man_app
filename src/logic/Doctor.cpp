#include "Doctor.hpp"

Doctor::Doctor(){

    specialization = 0;
    fee = -1;

}

Doctor::Doctor(int id, char * name, char * pswd, char contact[12], const char* specialization, int fee)
: Person(id, name, pswd, contact)
{
    this->fee = fee;
    
    for(int i = 0; i < 12;i++){

        this->contact[i] = contact[i];

    }

}