

class CustTime{

    public:

        int hr; //0-24 
        int min; // 0-59

        CustTime();
        CustTime(int hr, int min);
        bool operator==(CustTime& obj);

};