#include "Doctor.hpp"
#include "Validator.hpp"
#include <iostream>

Doctor::Doctor() : Person(), fee(0.0f)
{
    specialization[0] = '\0';
}

Doctor::Doctor(int id, const char *name, const char *spec, const char *contact, const char *password, float f)
    : Person(id, name, password, contact), fee(f)
{
    Validator::textCpy(specialization, spec, 51);
}

void Doctor::setSpecialization(const char *s)
{
    Validator::textCpy(specialization, s, 51);
}



void Doctor::toCSV(char *t, int tSize) const
{
    char idBuf[16], feeBuf[32];
    Validator::intToStr(id, idBuf, 16);
    Validator::floatToStr(fee, feeBuf, 32, 2);

    t[0] = '\0';
    Validator::strCat(t, idBuf, tSize);
    Validator::strCat(t, ",", tSize);
    Validator::strCat(t, name, tSize);
    Validator::strCat(t, ",", tSize);
    Validator::strCat(t, specialization, tSize);
    Validator::strCat(t, ",", tSize);
    Validator::strCat(t, contact, tSize);
    Validator::strCat(t, ",", tSize);
    Validator::strCat(t, password, tSize);
    Validator::strCat(t, ",", tSize);
    Validator::strCat(t, feeBuf, tSize);
}






//Operator overloads

bool Doctor::operator==(const Doctor &other) const
{
    return id == other.id;
}

ostream &operator<<(ostream &os, const Doctor &d)
{
    char feeBuf[32];
    Validator::floatToStr(d.fee, feeBuf, 32, 2);
    os << "ID: " << d.id
       << " | Name: " << d.name
       << " | Specialization: " << d.specialization
       << " | Contact: " << d.contact
       << " | Fee: PKR " << feeBuf;
    return os;
}


void Doctor::displayMenu()
{
    cout << "\nWelcome, Dr. " << name
              << " | Specialization: " << specialization << "\n";
    cout << "===============================================\n";
    cout << "1. View Today's Appointments\n";
    cout << "2. Mark Appointment Complete\n";
    cout << "3. Mark Appointment No-Show\n";
    cout << "4. Write Prescription\n";
    cout << "5. View Patient Medical History\n";
    cout << "6. Logout\n";
    cout << "Enter choice: ";
}

void Doctor::displayProfile()
{
    cout << *this << "\n";
}

