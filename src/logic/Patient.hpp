#pragma once
#include "Person.hpp"

class Patient : public Person
{

private:
    int age;
    char gender; // 'M' or 'F'
    double balance;

    // Appointment* appointments; will implement later

public:
    // CONSTTRUCTORS AND DESTRUCTOR
    Patient();
    Patient(int id, const char *name, int age, char gender, const char *contact, const char *password, double balance);
    virtual ~Patient();

    // GETTERS
    int getAge() const;
    char getGender() const;
    double getBalance() const;

    // setters
    void setAge(int age);
    void setGender(char gender);
    void setBalance(double balance);

    // OPERATOR  OVERLOADS
    bool operator==(const Patient &other) const;
    Patient &operator-=(double amount);
    Patient &operator+=(double amount);

    // FRIEND FUNCTIONS
    friend ostream &operator<<(ostream &out, Patient &obj);

    // virtual overloads:
    void displayInfo()  override;
    const char *getRole() override;
};
