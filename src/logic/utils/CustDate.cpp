#include "CustDate.hpp"
#include "Validator.hpp"

CustDate::CustDate(){
    day = -1;
    month = -1;
    year = -1;
    dmy[0] = '\0';
}


CustDate::CustDate(int day, int month, int year){
    


    if (!Validator::validDate(day, month, year)){

        day = -1;
        month = -1;
        year = -1;

    }

    this->day = day;

    this->month = month;
    this->year = year;
}

CustDate::CustDate(char DMY[11]){

    for(int  i = 0; i < 11; i++){   
        dmy[i] = DMY[i];    
    }

        

}


bool CustDate::operator==(CustDate& obj){

    if(day == -1 || obj.day == -1 ||month == -1 || obj.month == -1 || year == -1 || obj.year == -1){
        
        return false;

    }

    if(day == obj.day && month == obj.month && year == obj.year){
        
        return true;

    }else {
        return false;
    }

}
