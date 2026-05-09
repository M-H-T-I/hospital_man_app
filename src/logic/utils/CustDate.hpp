#pragma once

#include <iostream>
using namespace std;


class CustDate{

    public:
        int day;
        int month;
        int year;

        char dmy[11];

        CustDate(); //     day = 0  month = 0 year = 0 dmy[0] = '\0';
        CustDate(int day, int month, int year);
        CustDate(char DMY[11]);
        bool operator==(CustDate& obj);
};
