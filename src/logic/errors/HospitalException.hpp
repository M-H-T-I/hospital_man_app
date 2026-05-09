#include <iostream>



class HospitalException{

    public:
        char message[200];


        //CONSTRUCTORS

        HospitalException(const char* msg);


        //METHODS
        virtual char* what();

};