#include "Validator.hpp"


Validator::Validator(){}

bool Validator::validDate(const CustDate& date){

    if(date.day < 1 || date.day > 31 ) return false;
    if(date.month < 1 || date.month > 12) return false;
    if (date.year < 1970 || date.year > 2100 ) return false; // assuming that system does not need  


    bool leapYearCheck = (date.year % 4 == 0 && date.year % 100 != 0) || (date.year % 400 == 0);

    const int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

 
    if( date.day <= daysOfMonth[date.month - 1]){

        return true;


    }else {

        // checking february for 29 warna ghalat ha 

        if(date.month == 2 && date.day == 29 && leapYearCheck) return true;

        return false;


    }

}

bool Validator::validDate(int day, int month, int year){

    if(day < 1 || day > 31 ) return false;
    if(month < 1 || month > 12) return false;
    if (year < 1970 || year > 2100 ) return false; // assuming that system does not need  


    bool leapYearCheck = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

    const int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

 
    if( day <= daysOfMonth[month - 1]){

        return true;


    }else {

        // checking february for 29 warna ghalat ha 

        if(month == 2 && day == 29 && leapYearCheck) return true;

        return false;


    }

}