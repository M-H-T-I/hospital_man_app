#include "HospitalException.hpp"

class SlotUnavailableException: public HospitalException{

    public: 
        SlotUnavailableException(const char* msg);
        char* what();

};