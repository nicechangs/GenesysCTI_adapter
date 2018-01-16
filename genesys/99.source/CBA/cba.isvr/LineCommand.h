#pragma once

#include <string>
#include <map>
using namespace std;

#ifndef null 
#define null 0
#endif

class CLineCommand  
{
public:
    class CCommandToken {
    private:
        std::string _sName;
        std::string _sValue;

    protected:
        void setName(const char* cpName);
        void setValue(const char* cpValue);

        void set(const char* cpName, const char* cpValue);

    public:
        CCommandToken() { _sName ="";  _sValue=""; };
        const char* getName(void);
        const char* getValue(void);
        friend class CLineCommand;
    };

private:
    typedef std::map<int, CCommandToken*> TOKENLIST;
    TOKENLIST _lstToken;
    unsigned int _nCount;
    
    typedef std::map<string, CCommandToken*> TOKENLIST2;
    TOKENLIST2 _lstToken2;


public:
    CLineCommand();
    ~CLineCommand();

    CLineCommand& operator= (char* cpCommand);
    CCommandToken& operator[] (int nIndex);
    CCommandToken& operator[] (char* szName);

    bool IsName(const char* szName);
    inline unsigned int getCount(void) { return _nCount; };

    // add - kimcg : 2014.11.04
    void setKeyValue(const char* cpKey, const char* cpValue, char* pBuff);
    // add - end


    void removeAll(void);

private:
    char* getName(const char* cpCommand, std::string& sName);
    char* getValue(const char* cpCommand, std::string& sValue);
};


