#pragma once
#include "myUtils.hpp"

class Person
{
protected:
    int id;
    char name[51];
    char password[51];
    char contact[12];

public:
    Person();
    Person(int id, const char *name, const char *password, const char *contact);
    virtual ~Person();

    // getter
    int getID() const { return id; }
    const char *getName() const { return name; }
    const char *getPassword() const { return password; }
    const char *getContact() const { return contact; }

    // SETTER
    void setID(int newID) { id = newID; }
    void setName(const char *n);
    void setPassword(const char *p);
    void setContact(const char *c);

    // VIRTUAL FNCTION
    virtual void displayMenu() = 0;   
    virtual void displayProfile() = 0; 
};
