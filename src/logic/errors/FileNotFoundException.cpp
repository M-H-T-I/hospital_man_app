#include "FileNotFoundException.hpp"

char* FileNotFoundException::what(){

    return message;

}

FileNotFoundException::FileNotFoundException(char* msg): HospitalException(msg){}
