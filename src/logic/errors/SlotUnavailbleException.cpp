#include "SlotUnavailableException.hpp"

SlotUnavailableException::SlotUnavailableException(const char* msg): HospitalException(msg){}

char* SlotUnavailableException::what(){
    return message;
}