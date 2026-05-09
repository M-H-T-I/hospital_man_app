#include "InsufficientFundsException.hpp"

InsufficientFundsException::InsufficientFundsException() : HospitalException("Insufficient funds. Please top up your balance.") {}
InsufficientFundsException::InsufficientFundsException(const char *msg) : HospitalException(msg) {}