#include "myUtils.hpp"

int getLen(const char* str){

    int count = 0;
    while(str[count] != '\0'){
        count++;
    }

    return count;

}

bool checkPassword(const char* password, const char* other){

    bool match = true; // flag to see if both passwords match

    if(password == 0 || other == 0) match = false;

    int otherLen = getLen(other); // length of other parameter
    int len = getLen(password); // length of this->password

    if(len != otherLen) match = false;

    
    for (int i = 0; i < len; i++){

        if(password[i] != other[i]){

            match = false;
            break;

        }

    }

    if(match) return true;

    cout << "Passwords do not match." << endl;
    return false;
    
}