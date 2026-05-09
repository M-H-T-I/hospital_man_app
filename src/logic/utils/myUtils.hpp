#pragma once
#include "common.hpp"


int getLen(const char* str); // returns the length of a given character array
bool checkPassword(const char* password, const char* other);
void textCopy(char* dest, const char* src, int maxLen); // copies text into dest
int textCompare(const char* a, const char* b); // compares two text  like wo strcmp