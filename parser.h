#include <iterator>
#include <iostream>
#include <stack>
#include <math.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <cstdlib>
#include <vector>

#include "token.h"
#include "constants.h"
#include "scanner.h"
#include "tree.h"
#include "grammerRules.h"
#include "parameters.h"
#include "standardizeTree.h"
#include "cseMachine.h"
using namespace std;


class parser
{
public:
    bool isASTFlagged;      // Flag to check if AST is to be printed
    parameters* params;      // parameters* for the parser

    parser(char characterArray[], int fileLength, bool isFlagged)
    {
        isASTFlagged = isFlagged;

        // Initialize the parameters
        params = new parameters();
        params->sizeOfFile = fileLength;
        copy(characterArray, characterArray + 10000, params->programString);
        params->index = 0;
    }

    // Start parsing
    void parse()
    {
        // Get the first token
        params->nextToken = scan(params->programString, params->sizeOfFile, params->index);
        // Repeat until a non-DELETE token is found
        while(params->nextToken.getType() == "DELETE")
            params->nextToken = scan(params->programString, params->sizeOfFile, params->index);

        // Parse starting from the start symbol
        procedure_E(params);

        if (params->index < params->sizeOfFile - 1) // Check if end of file is not reached
        {
            cout << "Unable to Parse: Expected end of file but more tokens were found" << endl;
            exit(0);
        }

        // Get the root of the tree
        tree *parsedTree = params->syntaxTree.top(); // Get the root of the tree

        // Print the abstact syntax tree if the flag is set
        if (isASTFlagged == 1) parsedTree->printTree(0);

        // Standardize the tree
        makeStandardizedTree(parsedTree);

        // Create control structures for CSE machine
        tree *controlStructuresMap[300][300];
        buildControlStructures(parsedTree, controlStructuresMap);

        // print created control structures
        for (int i = 0; i < 300; i++) {
            if (controlStructuresMap[i][0] == NULL)
                break;
            for (int j = 0; j < 300; j++)
            {
                if (controlStructuresMap[i][j] == NULL)
                    break;
                cout << "(" << controlStructuresMap[i][j]->getType() << ")" << controlStructuresMap[i][j]->getValue() << " ";
            }
            cout << endl;
        }

    }
};
