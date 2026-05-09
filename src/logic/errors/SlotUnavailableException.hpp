#pragma once

#include "HospitalException.hpp"

class SlotUnavailableException : public HospitalException
{
public:
    SlotUnavailableException();
    SlotUnavailableException(const char *msg);
};