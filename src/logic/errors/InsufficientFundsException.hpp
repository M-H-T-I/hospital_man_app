#pragma once
#include "HospitalException.hpp"


class InsufficientFundsException : public HospitalException
{
public:
    InsufficientFundsException();

    InsufficientFundsException(const char *msg) : HospitalException(msg) {}
};
