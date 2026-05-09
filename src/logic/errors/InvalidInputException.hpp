#include "HospitalException.hpp" 


class InvalidInputException : public HospitalException{
public:
    InvalidInputException();

    InvalidInputException(const char *msg) : HospitalException(msg) {}
};