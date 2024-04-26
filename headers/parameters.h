#ifndef RPAL_PARAMETERS_H
#define RPAL_PARAMETERS_H

#include <iostream>
#include "token.h"
#include "tree.h"
using namespace std;

struct parameters
    {
        int sizeOfFile;
        char programString[10000];
        int index;
        token nextToken;
        stack<tree *> syntaxTree;
    };


#endif // RPAL_PARAMETERS_H
