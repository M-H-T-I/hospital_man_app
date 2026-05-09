#include <iostream>
#include "Person.hpp"
using namespace std;


class Admin: protected Person{


    public: 
        Admin();
        Admin(int id, const char* name, const char* pswd, char contact[11]);


        void displayInfo();
        char* getRole();


};