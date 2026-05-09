#include "myUtils.hpp"

int getLen(const char *str)
{

    int count = 0;
    while (str[count] != '\0')
    {
        count++;
    }

    return count;
}

bool checkPassword(const char *password, const char *other)
{

    bool match = true; // flag to see if both passwords match

    if (password == 0 || other == 0)
        match = false;

    int otherLen = getLen(other); // length of other parameter
    int len = getLen(password);   // length of this->password

    if (len != otherLen)
        match = false;

    for (int i = 0; i < len; i++)
    {

        if (password[i] != other[i])
        {

            match = false;
            break;
        }
    }

    if (match)
        return true;

    cout << "Passwords do not match." << endl;
    return false;
}

void textCopy(char *dest, const char *src, int maxLen)
{
    if (!dest || !src)
    {
        return;
    }

    int i = 0;

    while (i < maxLen - 1 && src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

int textCompare(const char *a, const char *b)
{
    if (!a || !b)
        return -1;

    int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return (unsigned char)a[i] - (unsigned char)b[i];
        }

        i++;
    }
    
    return (unsigned char)a[i] - (unsigned char)b[i];
}
