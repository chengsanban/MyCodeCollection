#pragma warning(disable:4996)
#include<iostream>
#include<CString>

class String
{
public:
    String()
    :_string(new char[1])
    , _refCount(new int(1))
    {
        *_string = '\0';
    }
    String(char *str)
        :_string(new char[strlen(str)+1])
        ,_refCount(new int(1))
    {
        strcpy(_string, str);
    }
    String(String &str)
    {
        _string = str._string;
        _refCount = str._refCount;
        *_refCount+=1;
    }
    String& operator=(String &s1)
    {
        if (this != &s1)
        {
            if (--(*_refCount) == 0)
            {
                delete[] _string;
                delete _refCount;
            }
            _string = s1._string;
            _refCount = s1._refCount;
            *_refCount+=1;
        }
        return *this;
    }
    ~String()
    {
        if ((*_refCount) - 1 == 0)
        {
            delete[] _string;
            delete _refCount;
        }
    }
private:
    char *_string;
    int *_refCount;
};