#pragma once
#include "HospitalException.hpp"

HospitalException::HospitalException() { message[0] = '\0'; }

HospitalException::HospitalException(const char *msg)
{
    int i = 0;
    while (msg[i] != '\0' && i < 199)
    {
        message[i] = msg[i];
        i++;
    }
    message[i] = '\0';
}

const char* HospitalException::what() const
{
    return message;
}

HospitalException::~HospitalException() {}
