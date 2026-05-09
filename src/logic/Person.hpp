#pragma once
#include "myUtils.hpp"

class Person
{

protected:
    int id;
    char name[51];     //    max 50 charac
    char contact[12];  // 11 digits contact
    char password[51]; // minimum 6 charcter password

public:
    // CONSTRUCTOIRS
    Person();
    Person(int id, const char *name, const char *contact, const char *password);
    virtual ~Person();

    // GETTER
    int getId() const;
    const char *getName() const;
    const char *getContact() const;
    const char *getPassword() const;

    // SETTER
    void setId(int id);
    void setName(const char* name);
    void setContact(const char* contact);
    void setPassword(const char* password);
 


    // virtiual functiuons
    virtual void displayInfo();
    virtual const char *getRole();
};