#pragma once
#include "utils/CustDate.hpp"

class Validator{

    public:


        Validator();
        // checks if given date is valid
        static bool validDate(const CustDate& date);
        static bool validDate(int day, int month, int year);

};