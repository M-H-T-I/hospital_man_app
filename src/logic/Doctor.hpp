#pragma once

#include <iostream>
#include "Person.hpp"
using namespace std;

class Doctor : public Person
{

private:
    char specialization[51]; // max 50 chars
    double fee;

public:
    Doctor();
    Doctor(int id, const char *name, const char *specialization, const char *contact, const char *password, double fee);
    virtual ~Doctor();

    // getters
    const char *getSpecialization() const;
    double getFee() const;

    // SETTER
    void setSpecialization(const char *specialization);
    void setFee(double fee);

    // OPERATOR OVERLOAD
    bool operator==(const Doctor &other) const;
    friend ostream &operator<<(ostream &out, Doctor &obj);

    // extra
    bool matchesSpecialization(const char *query) const;

    // VIRTUAL FUNCTIONS
    void displayInfo() ;
    const char *getRole();
};
