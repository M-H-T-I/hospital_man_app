#include "InvalidInputException.hpp"

InvalidInputException::InvalidInputException(const char* msg): HospitalException(msg){}
char* InvalidInputException::what(){
    return message;
}