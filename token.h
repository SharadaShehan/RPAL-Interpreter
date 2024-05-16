#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
using namespace std;

// Token class for storing each token and its type
class token
{

private:
    string type;
    string val;

public:
    void setType(const string &sts) // Set type of token
    {
        type = sts;
    }

    void setValue(const string &str)  // Set value of token
    {
        val = str;
    }

    string getType()    // Get type of token
    {
        return type;
    }

    string getValue()    // Get value of token
    {
        return val;
    }

    bool operator!=(token t);
    
};

#endif // TOKEN_H
