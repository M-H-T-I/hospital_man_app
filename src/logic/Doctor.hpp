#pragma once

#include "Person.hpp"
#include <iostream>

using namespace std;

class Doctor: public Person{
private:
    char specialization[51];
    float fee;

public:
    
    Doctor();
    Doctor(int id, const char *name, const char *spec,
           const char *contact, const char *password, float fee);

    
    const char *getSpecialization() const { return specialization; }
    float getFee() const { return fee; }

    
    void setSpecialization(const char *s);
    void setFee(float f) { fee = f; }

    
    bool operator==(const Doctor &other) const; 
    friend ostream &operator<<(ostream &os, const Doctor &d);

    
    void displayMenu() override;
    void displayProfile() override;

    void toCSV(char *buf, int bufSize) const;
};
