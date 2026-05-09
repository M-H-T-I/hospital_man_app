#include "HospitalException.hpp"

class InsufficientFundsException : public HospitalException {

    public:
        InsufficientFundsException(const char* msg);
        char* what();

};