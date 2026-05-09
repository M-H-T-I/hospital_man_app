#include "HospitalException.hpp"


class FileNotFoundException: public HospitalException{

    public:
        FileNotFoundException(char* msg);
        char* what();

};