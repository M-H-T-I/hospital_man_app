#include "Patient.hpp"
#include "errors/InsufficientFundsException.hpp"

// CONSTRUCTORS AND DESTRUCTOR:
Patient::Patient() : Person(), age(0), balance(0.0)
{
    gender = 'M';
}

Patient::Patient(int id, const char *name, int age, char gender, const char *contact, const char *password, double balance)
    : Person(id, name, contact, password), age(age), balance(balance)
{
    this->gender = gender;
}

Patient::~Patient() {}

// GETTER

int Patient::getAge() const
{
    return age;
}

char Patient::getGender() const
{
    return gender;
}

double Patient::getBalance() const
{
    return balance;
}

// setter
void Patient::setAge(int newAge)
{
    age = newAge;
}

void Patient::setGender(char newGender)
{
    gender = newGender;
}

void Patient::setBalance(double newBalance)
{
    balance = newBalance;
}

// OPERATOR OVERLOADS
Patient &Patient::operator+=(double amount)
{
    balance += amount;
    return *this;
}

bool Patient::operator==(const Patient &other) const
{
    return id == other.id;
}

Patient &Patient::operator-=(double amount)
{ // catch in upoper scope where it is called

    if ((this->balance - amount) < 0)
    {

        char msg[] = "Insufficient funds";
        throw InsufficientFundsException(msg);
    }
    else
    {
        balance -= amount;
    }
}

// FRIEND FUNCTIONs
// Format:  ID | Name | Age | Gender | Contact | Balance
std::ostream &operator<<(ostream &out, Patient &p)
{
    out << "ID: " << p.id
        << " | Name: " << p.name
        << " | Age: " << p.age
        << " | Gender: " << p.gender
        << " | Contact: " << p.contact
        << " | Balance: PKR " << p.balance;

    return out;
}

// virtual functions:
void Patient::displayInfo()
{
    cout << *this << "\n";
}

const char *Patient::getRole()
{
    return "Patient";
}
