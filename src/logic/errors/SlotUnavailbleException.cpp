#include "SlotUnavailableException.hpp"

SlotUnavailableException::SlotUnavailableException() : HospitalException("Selected time slot is unavailable. Please choose another slot.") {}
SlotUnavailableException::SlotUnavailableException(const char *msg) : HospitalException(msg) {}