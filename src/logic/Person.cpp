#include "Person.hpp"
#include "myUtils.hpp"


Person::Person(){

    
    id = -1; // my placeholder for null 
    password = name = 0;
    
    contact[0] = '\0';



    // name = 0
    // password = 0
    // contact = "\0..."
}

Person::Person(int id, const char* name, const char* pswd, char contact[11]){

    this->id = id;
    this-> name = name;
    this->password = pswd;

    

    for (int i = 0; i < 11; i++){

        this->contact[i] = contact[i];

    }
}





