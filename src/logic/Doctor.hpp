#include <iostream> 
#include "Person.hpp"
using namespace std;

class Doctor: protected Person{

    private:
        char* specialization;
        int fee;

    public:
        Doctor();
        Doctor(int id, char * name, char * pswd, char contact[12], const char* specialization, int fee);


        //getters
        int getFee();
        const char* getSpecialization();





};

