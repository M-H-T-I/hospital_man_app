#include "CustTime.hpp"
#include <iostream>
using namespace std;


CustTime::CustTime(){

    hr = -1;
    min = -1;

}

CustTime::CustTime(int hr, int min){

    if(hr >= 0 && hr<= 24){

        this->hr = hr;

    }else {

        cerr << "Invalid value for hours was given in constructor" << endl;
        this->hr = -1; // default
        this->min = -1; 

        return;
    }

    if (min >= 0 && min <=59){
        this->min = min;

    }else {
        cerr << "Invalid value for minutes was given in contructor" << endl;
        min = -1;
        hr = -1;

        return;

    } 
}

    // OPERATOR OVERLOADS
bool CustTime::operator==(CustTime& obj){
    
    if(hr == obj.hr && min == obj.min){
        return true;
    }

    return false;
    
}
