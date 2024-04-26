#ifndef RPAL_TYPEDEF_H
#define RPAL_TYPEDEF_H

#include "constants.h"
#include <string.h>
using namespace std;

// Checks if the given string is a keyword
bool isReservedKeyword(string str)
{
    int i = 0;
    for (i = 0; i < 20; i++)
    {
        if (str == keywords[i])
            return true;
    }
    return false;
}

// Checks if input character is an operator symbol
bool isOperator(char ch)
{
    for (int i = 0; i < 25; i++)
    {
        if (ch == operators[i])
            return true;
    }
    return false;
}

// Checks if input character is an alphabet letter (a-z, A-Z)
bool isAlpha(char ch)
{
    if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
        return true;
    return false;
}

// Checks if input character is a digit
bool isDigit(char ch)
{
    if (ch >= 48 && ch <= 57)
        return true;
    return false;
}

// Checks if the given string is a binary operator
bool isBinaryOperator(string op)
{
    for (int i = 0; i < 18; i++)
    {
        if (op == binaryOperators[i])
            return true;
    }
    return false;
}

// Checks if the given string is a number
bool isNumber(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

// Checks if the given character is a comment character
bool isCommentCharater(char ch)
{
    if (ch == '\'' || ch == '\\' || ch == '\\' || ch == '\\' || ch == '(' || ch == ')' || ch == ';' || ch == ',' || ch == ' ' || ch == '\t' || isAlpha(ch) || isDigit(ch) || isOperator(ch))
        return true;
    return false;
}

// Checks if the given character is a string character
bool isStringCharacter(char ch)
{
    if (ch == '\t' || ch == '\n' || ch == '\\' || ch == '\'' || ch == '(' || ch == ')' || ch == ';' || ch == ',' || ch == ' ' || ch == '_' || ch == '?' || isAlpha(ch) || isDigit(ch) || isOperator(ch))
        return true;
    return false;
}

#endif //RPAL_TYPEDEF_H
