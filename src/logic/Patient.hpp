#ifndef PATIENT_H
#define PATIENT_H






#include "Person.hpp"
#include <ostream>

class Patient : public Person {
private:
    int   age;
    char  gender[3];   
    float balance;

public:
    
    Patient();
    Patient(int id, const char* name, int age, const char* gender,
            const char* contact, const char* password, float balance);

    
    int         getAge()     const { return age; }
    const char* getGender()  const { return gender; }
    float       getBalance() const { return balance; }

    
    void setAge(int a)            { age = a; }
    void setGender(const char* g);
    void setBalance(float b)      { balance = b; }

    
    Patient& operator+=(float amount);          
    Patient& operator-=(float amount);          
    bool     operator==(const Patient& other) const; 
    friend std::ostream& operator<<(std::ostream& os, const Patient& p);

    
    void displayMenu()    override;
    void displayProfile() override;

    
    
    void toCSV(char* buf, int bufSize) const;
};

#endif 