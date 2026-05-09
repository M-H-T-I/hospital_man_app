#pragma once
#include "myUtils.hpp"


class Person{

    protected:

        int id;
        char * name;
        char * password;

        char contact[12]; // 11 digits

    public:

        Person();
        Person(int id, const char * name, const char * pswd, char contact[]);

        virtual void displayInfo();
        virtual char* getRole();


};