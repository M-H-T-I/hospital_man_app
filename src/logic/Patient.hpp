#include "Person.hpp"

class Patient: protected Person{


    private:
     
        int balance;
        int age; 
        char gender; // M/m or F/f

        // Appointment* appointments; will implement later

    public: 

        // CONSTTRUCTORS

        Patient();
        Patient(int id, char* name, char* pswd, int balance, int age, char gender, char contact[]); // add appointments to this when I have created that class


        //OPERATOR  OVERLOADS

        bool operator==(Patient& other);
        void operator-=(int val);
        void operator+=(int val);

        // FRIEND FUNCTIONS
        friend ostream& operator<<(ostream& out, Patient& obj);
};