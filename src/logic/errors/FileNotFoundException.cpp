#include "FileNotFoundException.hpp"

FileNotFoundException::FileNotFoundException(const char *filename)
{
    const char *prefix = "File not found: ";
    int i = 0;
    while (prefix[i] != '\0' && i < 199)
    {
        message[i] = prefix[i];
        i++;
    }
    int j = 0;
    while (filename[j] != '\0' && i < 199)
    {
        message[i++] = filename[j++];
    }
    message[i] = '\0';
}