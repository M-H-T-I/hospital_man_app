#include "Person.hpp"
#include "Validator.hpp"

Person::Person() : id(0)
{
    name[0] = '\0';
    password[0] = '\0';
    contact[0] = '\0';
}

Person::Person(int id, const char *n, const char *pwd, const char *c) : id(id)
{
    Validator::textCpy(name, n, 51);
    Validator::textCpy(password, pwd, 51);
    Validator::textCpy(contact, c, 12);
}

void Person::setName(const char *n) { Validator::textCpy(name, n, 51); }
void Person::setPassword(const char *p) { Validator::textCpy(password, p, 51); }
void Person::setContact(const char *c) { Validator::textCpy(contact, c, 12); }