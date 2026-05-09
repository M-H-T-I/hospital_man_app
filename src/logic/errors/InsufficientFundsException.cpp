#include "InsufficientFundsException.hpp"



InsufficientFundsException::InsufficientFundsException(const char* msg): HospitalException(msg){}


char* InsufficientFundsException::what(){
    return message;
}