#include "Validator.hpp"
#include <ctime>

int Validator::strLen(const char *s)
{
    int n = 0;
    while (s[n] != '\0')
        n++;
    return n;
}

bool Validator::strEq(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
            return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

void Validator::textCpy(char *dst, const char *src, int maxLen)
{
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1)
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

void Validator::strCat(char *dst, const char *src, int maxLen)
{
    int dLen = strLen(dst);
    int i = 0;
    while (src[i] != '\0' && dLen + i < maxLen - 1)
    {
        dst[dLen + i] = src[i];
        i++;
    }
    dst[dLen + i] = '\0';
}

int Validator::strToInt(const char *s)
{
    int result = 0;
    bool negative = false;
    int i = 0;
    if (s[i] == '-')
    {
        negative = true;
        i++;
    }
    while (s[i] >= '0' && s[i] <= '9')
    {
        result = result * 10 + (s[i] - '0');
        i++;
    }
    return negative ? -result : result;
}

float Validator::strToFloat(const char *s)
{
    float result = 0.0f;
    bool negative = false;
    int i = 0;
    if (s[i] == '-')
    {
        negative = true;
        i++;
    }
    while (s[i] >= '0' && s[i] <= '9')
    {
        result = result * 10.0f + (s[i] - '0');
        i++;
    }
    if (s[i] == '.')
    {
        i++;
        float frac = 0.1f;
        while (s[i] >= '0' && s[i] <= '9')
        {
            result += (s[i] - '0') * frac;
            frac *= 0.1f;
            i++;
        }
    }
    return negative ? -result : result;
}

void Validator::intToStr(int n, char *temp, int tempSize)
{
    if (tempSize <= 0)
        return;
    if (n == 0)
    {
        temp[0] = '0';
        temp[1] = '\0';
        return;
    }
    bool negative = n < 0;
    if (negative)
        n = -n;
    char tmp[32];
    int idx = 0;
    while (n > 0)
    {
        tmp[idx++] = '0' + (n % 10);
        n /= 10;
    }
    if (negative)
        tmp[idx++] = '-';

    int out = 0;
    for (int k = idx - 1; k >= 0 && out < tempSize - 1; k--)
    {
        temp[out++] = tmp[k];
    }
    temp[out] = '\0';
}

void Validator::floatToStr(float f, char *temp, int tempSize, int decimals)
{
    if (tempSize <= 0)
        return;
    bool negative = f < 0.0f;
    if (negative)
        f = -f;

    int intPart = (int)f;
    float fracPart = f - (float)intPart;

    float multiplier = 1.0f;
    for (int i = 0; i < decimals; i++)
        multiplier *= 10.0f;
    int fracInt = (int)(fracPart * multiplier + 0.5f);

    char tmp[64];
    int idx = 0;
    if (negative)
        tmp[idx++] = '-';

    char inttemp[32];
    intToStr(intPart, inttemp, 32);
    int k = 0;
    while (inttemp[k] != '\0' && idx < 62)
        tmp[idx++] = inttemp[k++];

    if (decimals > 0)
    {
        tmp[idx++] = '.';

        char fractemp[32];
        intToStr(fracInt, fractemp, 32);
        int fracLen = strLen(fractemp);
        for (int z = fracLen; z < decimals && idx < 62; z++)
            tmp[idx++] = '0';
        k = 0;
        while (fractemp[k] != '\0' && idx < 62)
            tmp[idx++] = fractemp[k++];
    }
    tmp[idx] = '\0';

    textCpy(temp, tmp, tempSize);
}

bool Validator::isPositiveInt(const char *s)
{
    if (s[0] == '\0')
        return false;
    for (int i = 0; s[i] != '\0'; i++)
    {
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    int v = strToInt(s);
    return v > 0;
}

bool Validator::isPositiveFloat(const char *s)
{
    if (s[0] == '\0')
        return false;
    int dots = 0, i = 0;
    for (; s[i] != '\0'; i++)
    {
        if (s[i] == '.')
        {
            dots++;
            if (dots > 1)
                return false;
        }
        else if (s[i] < '0' || s[i] > '9')
            return false;
    }
    float v = strToFloat(s);
    return v > 0.0f;
}

bool Validator::isValidID(const char *s)
{
    return isPositiveInt(s);
}

bool Validator::isValidContact(const char *s)
{
    if (strLen(s) != 11)
        return false;
    for (int i = 0; i < 11; i++)
    {
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    return true;
}

bool Validator::isValidPassword(const char *s)
{
    return strLen(s) >= 6;
}

bool Validator::isValidGender(const char *s)
{
    return (s[0] == 'M' || s[0] == 'F') && s[1] == '\0';
}

bool Validator::isValidAge(const char *s)
{
    if (!isPositiveInt(s))
        return false;
    int age = strToInt(s);
    return age >= 1 && age <= 150;
}

bool Validator::isValidMenuChoice(const char *s, int min, int max)
{
    if (!isPositiveInt(s) && !(s[0] >= '0' && s[0] <= '9'))
        return false;

    for (int i = 0; s[i] != '\0'; i++)
    {
        if (s[i] < '0' || s[i] > '9')
            return false;
    }
    int v = strToInt(s);
    return v >= min && v <= max;
}

bool Validator::isValidTimeSlot(const char *s)
{
    const char *slots[] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"};
    for (int i = 0; i < 8; i++)
    {
        if (strEq(s, slots[i]))
            return true;
    }
    return false;
}

void Validator::parseDate(const char *date, int &day, int &month, int &year)
{

    char d[3] = {0}, m[3] = {0}, y[5] = {0};
    d[0] = date[0];
    d[1] = date[1];
    d[2] = '\0';
    m[0] = date[3];
    m[1] = date[4];
    m[2] = '\0';
    y[0] = date[6];
    y[1] = date[7];
    y[2] = date[8];
    y[3] = date[9];
    y[4] = '\0';
    day = strToInt(d);
    month = strToInt(m);
    year = strToInt(y);
}

bool Validator::isValidDateFormat(const char *s)
{

    if (strLen(s) != 10)
        return false;
    for (int i = 0; i < 10; i++)
    {
        if (i == 2 || i == 5)
        {
            if (s[i] != '-')
                return false;
        }
        else
        {
            if (s[i] < '0' || s[i] > '9')
                return false;
        }
    }
    return true;
}

bool Validator::isValidDate(const char *s)
{
    if (!isValidDateFormat(s))
        return false;
    int day, month, year;
    parseDate(s, day, month, year);
    if (day < 1 || day > 31)
        return false;
    if (month < 1 || month > 12)
        return false;

    time_t now = time(0);
    struct tm *t = localtime(&now);
    int currentYear = t->tm_year + 1900;
    if (year < currentYear)
        return false;

    return true;
}

char Validator::toLowerChar(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c + 32;
    return c;
}

bool Validator::strEqCaseInsensitive(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (toLowerChar(a[i]) != toLowerChar(b[i]))
            return false;
        i++;
    }
    return a[i] == '\0' && b[i] == '\0';
}

void Validator::trim(char *s)
{
    int len = strLen(s);

    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\n' || s[len - 1] == '\r' || s[len - 1] == '\t'))
    {
        s[--len] = '\0';
    }

    int start = 0;
    while (s[start] == ' ' || s[start] == '\t')
        start++;
    if (start > 0)
    {
        int i = 0;
        while (s[start + i] != '\0')
        {
            s[i] = s[start + i];
            i++;
        }
        s[i] = '\0';
    }
}