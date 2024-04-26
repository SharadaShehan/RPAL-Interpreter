#ifndef B3D9C1A3_658F_4A0D_ABA4_F03E93B4F12E
#define B3D9C1A3_658F_4A0D_ABA4_F03E93B4F12E
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


#endif /* B3D9C1A3_658F_4A0D_ABA4_F03E93B4F12E */
