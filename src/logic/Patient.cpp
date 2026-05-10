// =============================================================================
// Patient.cpp
// =============================================================================
#include "Patient.hpp"
#include "Validator.hpp"
#include <iostream>

Patient::Patient() : Person(), age(0), balance(0.0f)
{
    gender[0] = '\0';
}

Patient::Patient(int id, const char *name, int age, const char *gnd,
                 const char *contact, const char *password, float balance)
    : Person(id, name, password, contact), age(age), balance(balance)
{
    Validator::textCpy(gender, gnd, 3);
}

void Patient::setGender(const char *g)
{
    Validator::textCpy(gender, g, 3);
}

// ── Operator overloads ────────────────────────────────────────────────────────

Patient &Patient::operator+=(float amount)
{
    balance += amount;
    return *this;
}

Patient &Patient::operator-=(float amount)
{
    balance -= amount;
    return *this;
}

bool Patient::operator==(const Patient &other) const
{
    return id == other.id;
}

std::ostream &operator<<(std::ostream &os, const Patient &p)
{
    char balBuf[32];
    Validator::floatToStr(p.balance, balBuf, 32, 2);
    os << "ID: " << p.id
       << " | Name: " << p.name
       << " | Age: " << p.age
       << " | Gender: " << p.gender
       << " | Contact: " << p.contact
       << " | Balance: PKR " << balBuf;
    return os;
}

// ── Pure virtual implementations ──────────────────────────────────────────────

void Patient::displayMenu()
{
    char balBuf[32];
    Validator::floatToStr(balance, balBuf, 32, 2);
    std::cout << "\nWelcome, " << name << "\n";
    std::cout << "Balance: PKR " << balBuf << "\n";
    std::cout << "========================\n";
    std::cout << "1. Book Appointment\n";
    std::cout << "2. Cancel Appointment\n";
    std::cout << "3. View My Appointments\n";
    std::cout << "4. View My Medical Records\n";
    std::cout << "5. View My Bills\n";
    std::cout << "6. Pay Bill\n";
    std::cout << "7. Top Up Balance\n";
    std::cout << "8. Logout\n";
    std::cout << "Enter choice: ";
}

void Patient::displayProfile()
{
    std::cout << *this << "\n";
}

// ── CSV serialisation ─────────────────────────────────────────────────────────
// patient_id,name,age,gender,contact,password,balance

void Patient::toCSV(char *buf, int bufSize) const
{
    char ageBuf[16], balBuf[32], idBuf[16];
    Validator::intToStr(id, idBuf, 16);
    Validator::intToStr(age, ageBuf, 16);
    Validator::floatToStr(balance, balBuf, 32, 2);

    buf[0] = '\0';
    Validator::strCat(buf, idBuf, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, name, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, ageBuf, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, gender, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, contact, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, password, bufSize);
    Validator::strCat(buf, ",", bufSize);
    Validator::strCat(buf, balBuf, bufSize);
}