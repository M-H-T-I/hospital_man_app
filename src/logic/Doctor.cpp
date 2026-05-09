#include "Doctor.hpp"
#include "myUtils.hpp"

// CONSTRUCTOR AND DESTRUCTOR

Doctor::Doctor() : Person(), fee(0.0)
{
    specialization[0] = '\0';
}

Doctor::Doctor(int id, const char *name, const char *specialization, const char *contact, const char *password, double fee)
    : Person(id, name, contact, password)
{
    this->fee = fee;
    textCopy(this->specialization, specialization, 51);
}

Doctor::~Doctor() {}

// GETTER

const char *Doctor::getSpecialization() const
{
    return specialization;
}

double Doctor::getFee() const
{
    return fee;
}

// SETTER
void Doctor::setFee(double newFee)
{
    fee = newFee;
}

void Doctor::setSpecialization(const char *newSpec)
{
    textCopy(specialization, newSpec, 51);
}

// operator overload
bool Doctor::operator==(const Doctor &other) const
{
    return id == other.id;
}

ostream &operator<<(std::ostream &out, Doctor &obj)
{
    out << "ID: " << obj.id
        << " | Name: " << obj.name
        << " | Specialization: " << obj.specialization
        << " | Contact: " << obj.contact
        << " | Fee: PKR " << obj.fee;
    return out;
}

// VIRTUAL FUNCTIONS
void Doctor::displayInfo()
{
    cout << *this << "\n";
}

const char *Doctor::getRole()
{
    return "Doctor";
}
