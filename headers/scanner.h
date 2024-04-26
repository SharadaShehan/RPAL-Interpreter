#ifndef RPAL_SCANNER_H
#define RPAL_SCANNER_H
#include "typeDef.h"
using namespace std;

// Read the next token from character array
token scan(char characterArray[], int sizeOfFile, int &currentIndex)
{
    token nextToken;    // Next token to be identified and returned
    int i = currentIndex;   // currentIndex of character
    string value = "";  // Value of the token to be set
    
    // Check if end of file is reached
    if (characterArray[i] == '\0' || i == sizeOfFile)
    {
        nextToken.setType("EOF");
        nextToken.setValue("EOF");
        return nextToken;
    }

    // Read the next token from the string
    while (i < 10000 || i < sizeOfFile || characterArray[i] != '\0')
    {
        // Check if token is a number
        // first character is a digit -> token is a number
        if (isDigit(characterArray[i]))
        {
            // get rest of digits in the number
            while (isDigit(characterArray[i]))
            {
                value = value + characterArray[i];
                i++;
            }
            nextToken.setType("INT");
            nextToken.setValue(value);
            currentIndex = i;
            return nextToken;
        }
        
        // Check if next token is an identifier
        // first character is an alphabet letter -> token is an identifier
        else if (isAlpha(characterArray[i]))
        {
            // get rest of the characters in the identifier
            // valid characters are: alphabet letters, digits, and underscore
            while (isAlpha(characterArray[i]) || isDigit(characterArray[i]) || characterArray[i] == '_')
            {
                value = value + characterArray[i];
                i++;
            }
            // check if the identifier is a keyword
            if (isReservedKeyword(value))
            {
                nextToken.setType("KEYWORD");
                nextToken.setValue(value);
                currentIndex = i;   
                return nextToken;
            }
            // if identifier is not a keyword
            else
            {
                nextToken.setType("ID");
                nextToken.setValue(value);
                currentIndex = i;
                return nextToken;
            }
        }

        // Check if next token is a comment
        // first two characters are "//" -> token is a comment
        else if (characterArray[i] == '/' && characterArray[i + 1] == '/')
        {
            // get rest of the characters in the comment
            while (isCommentCharater(characterArray[i]))
            {
                // comment ends with a newline character
                if (characterArray[i] == '\n')
                {
                    i++;
                    break;
                }
                else
                {
                    value = value + characterArray[i];
                    i++;
                }
            }
            nextToken.setType("DELETE");
            nextToken.setValue(value);
            currentIndex = i;
            return nextToken;
        }
        
        // Check if next token is an operator
        // first character is an operator symbol -> token is an operator
        else if (isOperator(characterArray[i]))
        {
            // for binary operators this will loop twice
            while (isOperator(characterArray[i]))
            {
                value = value + characterArray[i];
                i++;
            }
            nextToken.setType("OPERATOR");
            nextToken.setValue(value);
            currentIndex = i;
            return nextToken;
        }

        // Check if next token is a string
        // first character is a single quote -> token is a string
        else if (characterArray[i] == '\'')
        {
            value = value + characterArray[i];
            i++;
            // get rest of the characters in the string
            while (isStringCharacter(characterArray[i]))
            {
                // ending single quote
                if (characterArray[i] == '\'')
                {
                    value = value + characterArray[i];
                    i++;
                    break;
                }
                // escape characters
                else if (characterArray[i] == '\\')
                {
                    i++;
                    if (characterArray[i] == 't' || characterArray[i] == 'n' || characterArray[i] == '\\' || characterArray[i] == '"')
                    // valid escape characters
                    {
                        value = value + characterArray[i - 1];
                        value = value + characterArray[i];
                        i++;
                    }
                    else
                    // ignore invalid escape characters
                    {
                        i++;
                    }
                }
                else
                {
                    value = value + characterArray[i];
                    i++;
                }
            }
            nextToken.setType("STR");
            nextToken.setValue(value);
            currentIndex = i;
            return nextToken;
        }

        // Check if next token is a punctuation symbol (( ) ; ,)
        else if (characterArray[i] == ')' || characterArray[i] == '(' || characterArray[i] == ';' || characterArray[i] == ',')
        {
            value = value + characterArray[i];
            nextToken.setType("PUNCTION");
            nextToken.setValue(value);

            i++;
            currentIndex = i;
            return nextToken;
        }

        // Check if next token is an empty space
        else if (isspace(characterArray[i]))
        {
            while (isspace(characterArray[i]))
            {
                value = value + characterArray[i];
                i++;
            }
            nextToken.setType("DELETE");
            nextToken.setValue(value);
            currentIndex = i;
            return nextToken;
        }

        // Check if next token is an INVALID token
        else
        {
            value = value + characterArray[i];
            nextToken.setType("UNKNOWN");
            nextToken.setValue(value);
            
            i++;
            currentIndex = i;
            return nextToken;
        }
    }

    return nextToken;
}

#endif //RPAL_SCANNER_H
