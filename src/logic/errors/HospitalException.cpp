#include "HospitalException.hpp"
#include "../myUtils.hpp"

HospitalException::HospitalException(const char* msg){

    int len = getLen(msg);

    if(len > 200){
        len = 200;
    }

    for(int i = 0;i < len; i++){
        message[i] = msg[i];
    }

}   

char* HospitalException::what(){

    return message;

}