#pragma once

class Validator
{
public:
    static int strLen(const char *s);
    static bool strEq(const char *a, const char *b);
    static void textCpy(char *dst, const char *src, int maxLen);
    static void strCat(char *dst, const char *src, int maxLen);

    static int strToInt(const char *s);
    static float strToFloat(const char *s);
    static void intToStr(int n, char *buf, int bufSize);
    static void floatToStr(float f, char *buf, int bufSize, int decimals = 2);

    static bool isPositiveInt(const char *s);
    static bool isPositiveFloat(const char *s);
    static bool isValidID(const char *s);
    static bool isValidContact(const char *s);  
    static bool isValidPassword(const char *s); 
    static bool isValidGender(const char *s);   
    static bool isValidAge(const char *s);      
    static bool isValidMenuChoice(const char *s, int min, int max);
    static bool isValidTimeSlot(const char *s);   
    static bool isValidDate(const char *s);       
    static bool isValidDateFormat(const char *s); 

    static bool strEqCaseInsensitive(const char *a, const char *b);
    static char toLowerChar(char c);

    static void parseDate(const char *date, int &day, int &month, int &year);

    static void trim(char *s);
};
