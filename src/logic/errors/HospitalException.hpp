#pragma once

class HospitalException
{
protected:

    char message[200];

public:
    HospitalException();

    HospitalException(const char *msg);

    virtual const char *what() const;

    virtual ~HospitalException();
};




