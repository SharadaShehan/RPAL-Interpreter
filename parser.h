#include <iterator>
#include <iostream>
#include <stack>
#include <math.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <cstdlib>
#include <vector>


// #include "token.h"
// #include "tree.h"
// #include "constants.h"
// #include "typeDef.h"
using namespace std;

// stack<tree *> st; // Stack for syntax tree

class parser
{
public:
    int sizeOfFile;      // Length of file
    bool isASTFlagged;      // Flag to check if AST is to be printed
    char programString[5000];  // Array to store the Program
    int index;           // Current index of the program
    // token nextToken;     // Next token to be read

    parser(char characterArray[], int fileLength, bool isASTFlagged)
    {
        copy(characterArray, characterArray + 5000, programString);
        index = 0;
        sizeOfFile = fileLength;
        isASTFlagged = isASTFlagged;
    }

    // Start parsing
    void parse()
    {
        // nextToken = getToken(readnew);          // Get the first token
        // while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
        //     nextToken = getToken(readnew);

        // procedure_E(); // Start parsing from E

        // while (nextToken.getType() == "DELETE") // Ignore all DELETE tokens
        //     nextToken = getToken(readnew);

        // if (index >= sizeOfFile - 1) // Check if end of file is reached
        // {
        //     tree *t = st.top(); // Get the root of the tree

        //     // Print the abstact syntax tree if the flag is set
        //     if (astFlag == 1)
        //         t->print_tree(0);

        //     // Make the ST
        //     makeST(t);

        //     // Print the standardized tree if the flag is set
        //     if (astFlag == 2)
        //         t->print_tree(0);

        // }
    }

    // // Helper function for makeStandardTree
    // void makeST(tree *t)
    // {
    //     makeStandardTree(t);
    // }

    // // Makes the tree standard
    // tree *makeStandardTree(tree *t)
    // {
    //     if (t == NULL)
    //         return NULL;
    //     makeStandardTree(t->left);
    //     makeStandardTree(t->right);

    //     if (t->getVal() == "let")
    //     {
    //         if (t->left->getVal() == "=")
    //         {
    //             t->setVal("gamma");
    //             t->setType("KEYWORD");
    //             tree *P = createNode(t->left->right);
    //             tree *X = createNode(t->left->left);
    //             tree *E = createNode(t->left->left->right);
    //             t->left = createNode("lambda", "KEYWORD");
    //             t->left->right = E;
    //             tree *lambda = t->left;
    //             lambda->left = X;
    //             lambda->left->right = P;
    //         }
    //     }
    //     else if (t->getVal() == "and" && t->left->getVal() == "=")
    //     {
    //         tree *equal = t->left;
    //         t->setVal("=");
    //         t->setType("KEYWORD");
    //         t->left = createNode(",", "PUNCTION");
    //         tree *comma = t->left;
    //         comma->left = createNode(equal->left);
    //         t->left->right = createNode("tau", "KEYWORD");
    //         tree *tau = t->left->right;

    //         tau->left = createNode(equal->left->right);
    //         tau = tau->left;
    //         comma = comma->left;
    //         equal = equal->right;

    //         while (equal != NULL)
    //         {
    //             comma->right = createNode(equal->left);
    //             comma = comma->right;
    //             tau->right = createNode(equal->left->right);
    //             tau = tau->right;

    //             equal = equal->right;
    //         }
    //     }
    //     else if (t->getVal() == "where")
    //     {
    //         t->setVal("gamma");
    //         t->setType("KEYWORD");
    //         if (t->left->right->getVal() == "=")
    //         {
    //             tree *P = createNode(t->left);
    //             tree *X = createNode(t->left->right->left);
    //             tree *E = createNode(t->left->right->left->right);
    //             t->left = createNode("lambda", "KEYWORD");
    //             t->left->right = E;
    //             t->left->left = X;
    //             t->left->left->right = P;
    //         }
    //     }
    //     else if (t->getVal() == "within")
    //     {
    //         if (t->left->getVal() == "=" && t->left->right->getVal() == "=")
    //         {
    //             tree *X1 = createNode(t->left->left);
    //             tree *E1 = createNode(t->left->left->right);
    //             tree *X2 = createNode(t->left->right->left);
    //             tree *E2 = createNode(t->left->right->left->right);
    //             t->setVal("=");
    //             t->setType("KEYWORD");
    //             t->left = X2;
    //             t->left->right = createNode("gamma", "KEYWORD");
    //             tree *temp = t->left->right;
    //             temp->left = createNode("lambda", "KEYWORD");
    //             temp->left->right = E1;
    //             temp = temp->left;
    //             temp->left = X1;
    //             temp->left->right = E2;
    //         }
    //     }
    //     else if (t->getVal() == "rec" && t->left->getVal() == "=")
    //     {
    //         tree *X = createNode(t->left->left);
    //         tree *E = createNode(t->left->left->right);

    //         t->setVal("=");
    //         t->setType("KEYWORD");
    //         t->left = X;
    //         t->left->right = createNode("gamma", "KEYWORD");
    //         t->left->right->left = createNode("YSTAR", "KEYWORD");
    //         tree *ystar = t->left->right->left;

    //         ystar->right = createNode("lambda", "KEYWORD");

    //         ystar->right->left = createNode(X);
    //         ystar->right->left->right = createNode(E);
    //     }
    //     else if (t->getVal() == "function_form")
    //     {
    //         tree *P = createNode(t->left);
    //         tree *V = t->left->right;

    //         t->setVal("=");
    //         t->setType("KEYWORD");
    //         t->left = P;

    //         tree *temp = t;
    //         while (V->right->right != NULL)
    //         {
    //             temp->left->right = createNode("lambda", "KEYWORD");
    //             temp = temp->left->right;
    //             temp->left = createNode(V);
    //             V = V->right;
    //         }

    //         temp->left->right = createNode("lambda", "KEYWORD");
    //         temp = temp->left->right;

    //         temp->left = createNode(V);
    //         temp->left->right = V->right;
    //     }
    //     else if (t->getVal() == "lambda")
    //     {
    //         if (t->left != NULL)
    //         {
    //             tree *V = t->left;
    //             tree *temp = t;
    //             if (V->right != NULL && V->right->right != NULL)
    //             {
    //                 while (V->right->right != NULL)
    //                 {
    //                     temp->left->right = createNode("lambda", "KEYWORD");
    //                     temp = temp->left->right;
    //                     temp->left = createNode(V);
    //                     V = V->right;
    //                 }

    //                 temp->left->right = createNode("lambda", "KEYWORD");
    //                 temp = temp->left->right;
    //                 temp->left = createNode(V);
    //                 temp->left->right = V->right;
    //             }
    //         }
    //     }
    //     else if (t->getVal() == "@")
    //     {
    //         tree *E1 = createNode(t->left);
    //         tree *N = createNode(t->left->right);
    //         tree *E2 = createNode(t->left->right->right);
    //         t->setVal("gamma");
    //         t->setType("KEYWORD");
    //         t->left = createNode("gamma", "KEYWORD");
    //         t->left->right = E2;
    //         t->left->left = N;
    //         t->left->left->right = E1;
    //     }

    //     return NULL;
    // }


    // /* -------------------------------- Helper Functions -------------------------------- */

    // // Arranges nodes in tauNode into a stack
    // void arrangeTuple(tree *tauNode, stack<tree *> &res)
    // {
    //     if (tauNode == NULL)
    //         return;
    //     if (tauNode->getVal() == "lamdaTuple")
    //         return;
    //     if (tauNode->getVal() != "tau" && tauNode->getVal() != "nil")
    //     {
    //         res.push(tauNode);
    //     }
    //     arrangeTuple(tauNode->left, res);
    //     arrangeTuple(tauNode->right, res);
    // }

};
