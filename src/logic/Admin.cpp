#include "Admin.hpp"


Admin::Admin(){}

Admin::Admin(int id, const char* name, const char* pswd, char contact[11]): Person(id, name, pswd, contact){
}


void Admin::displayInfo(){

    cout << "Admin information: " << endl;
    cout << "ID: " << id << endl;
    cout << "Name: " << name; 
     

}