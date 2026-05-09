#include "HospitalException.hpp"

class InvalidInputException: public HospitalException{

    public:
        InvalidInputException(const char* msg);
        char* what();
};