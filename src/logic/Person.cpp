#include "Person.hpp"
#include "myUtils.hpp"

// VCONSTRUCTOR

Person::Person()
    : id(0)
{
    name[0] = '\0';
    contact[0] = '\0';
    password[0] = '\0';
}

Person::Person(int id, const char *name, const char *contact, const char *password)
    : id(id)
{
    textCopy(this->name, name, 51);
    textCopy(this->contact, contact, 12);
    textCopy(this->password, password, 51);
}

Person::~Person() {}



// GETTER
int Person::getId() const
{
    return id;
}

const char *Person::getName() const
{
    return name;
}

const char *Person::getContact() const
{
    return contact;
}

const char *Person::getPassword() const
{
    return password;
}


// SETTERS
void Person::setContact(const char* newCont)
{
    textCopy(contact, newCont, 12);
}
 
void Person::setPassword(const char* newPswd)
{
    textCopy(password, newPswd, 51);
}

void Person::setId(int newId)
{
    id = newId;
}
 
void Person::setName(const char* newName)
{
    textCopy(name, newName, 51);
}
 

