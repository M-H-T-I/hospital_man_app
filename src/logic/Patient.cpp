#include "Patient.hpp"
#include "errors/InsufficientFundsException.hpp"

//CONSTRUCTORS

Patient::Patient(){

    // default values for rest of the attributes from my Person constructor
    // name = 0;
    // password = 0
    // id = -1
    
    balance = 0;

}

Patient::Patient(int id, char* name, char* pswd, int balance, int age, char gender, char contact[]): Person(id, name, pswd, contact){

    this->balance = balance;
    // add appointments laterrr
    this->age = age;
    this->gender = gender;

}


// OPERATOR OVERLOADS


void Patient::operator+=(int val){

    balance += val;

}

bool Patient::operator==(Patient& other){

    if(other.id == this->id){
        return true;
    }

    return false;
}

void Patient::operator-=(int val){ // catch in upoper scope where it is called
    
    if( (this->balance - val) < 0){

        char msg[] = "Insufficient funds";
        throw InsufficientFundsException(msg);
        
    }else {
        balance-= val;
    }

}


// FRIEND FUNCTIOSN

ostream& operator<<(ostream& out, Patient& obj){

    out << "ID: " << obj.id << " | " ;

}