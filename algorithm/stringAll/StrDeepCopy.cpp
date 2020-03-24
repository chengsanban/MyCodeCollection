#include<iostream>
#include<CString>
#pragma warning(disable:4996)

class String
{
public:
    String()
        :_string(new char[1])
    {
        *_string = '\0';
    }
    String(char *str)
        :_string(new char[strlen(str)+1])
    {
        strcpy(_string,str);
    }
    String(String &s1)
        :_string(new char[strlen(s1._string)+1])
    {
        strcpy(_string, s1._string);
    }
    String& operator=(String &s1)
    {
        if (this != &s1)
        {
            char *temp = new char[strlen(s1._string) + 1];
            strcpy(temp,s1._string);
            delete[] _string;
            _string = temp;
        }
        return *this;
    }
    ~String()
    {
        if (_string)
        {
            delete[] _string;
        }
    }
private:
    char *_string;
};