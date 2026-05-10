#pragma once

#include "Person.hpp"

class Admin : public Person
{
public:
    Admin();
    Admin(int id, const char *name, const char *password);

    void displayMenu() override;
    void displayProfile() override;
};
