#ifndef PATIENT_H
#define PATIENT_H

// =============================================================================
// Patient.h
// Represents a registered patient. Inherits from Person.
// Operator overloads: +=, -=, ==, <<
// =============================================================================
#include "Person.hpp"
#include <ostream>

class Patient : public Person {
private:
    int   age;
    char  gender[3];   // "M" or "F"
    float balance;

public:
    // ── Constructors ─────────────────────────────────────────────────────────
    Patient();
    Patient(int id, const char* name, int age, const char* gender,
            const char* contact, const char* password, float balance);

    // ── Accessors ─────────────────────────────────────────────────────────────
    int         getAge()     const { return age; }
    const char* getGender()  const { return gender; }
    float       getBalance() const { return balance; }

    // ── Mutators ──────────────────────────────────────────────────────────────
    void setAge(int a)            { age = a; }
    void setGender(const char* g);
    void setBalance(float b)      { balance = b; }

    // ── Operator overloads ────────────────────────────────────────────────────
    Patient& operator+=(float amount);          // add to balance
    Patient& operator-=(float amount);          // deduct from balance
    bool     operator==(const Patient& other) const; // compare by ID
    friend std::ostream& operator<<(std::ostream& os, const Patient& p);

    // ── Pure virtual implementations ──────────────────────────────────────────
    void displayMenu()    override;
    void displayProfile() override;

    // ── CSV serialisation ─────────────────────────────────────────────────────
    // Format: patient_id,name,age,gender,contact,password,balance
    void toCSV(char* buf, int bufSize) const;
};

#endif // PATIENT_H