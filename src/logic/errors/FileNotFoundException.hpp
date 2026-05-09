#include "HospitalException.hpp"

class FileNotFoundException : public HospitalException
{

public:
    FileNotFoundException(const char *filename);
};