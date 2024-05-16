#ifndef RPAL_STANDARDIZETREE_H
#define RPAL_STANDARDIZETREE_H

#include "tree.h"
using namespace std;


// Standardize the parsed tree
tree *makeStandardizedTree(tree *treeNode)
{
    // if tree is null, return null
    if (treeNode == NULL) return NULL;
    
    // recursively call the function for left and right children
    makeStandardizedTree(treeNode->left);
    makeStandardizedTree(treeNode->right);

    // Standardize the tree based on the rules

    
    if (treeNode->getValue() == "let")
    /*
    let[ =[ X E ] P ] => gamma[ lambda[ X P ] E ]
    */
    {
        if (treeNode->left->getValue() == "=") {
            // new root node
            treeNode->setType("KEYWORD");
            treeNode->setValue("gamma");

            // descendents of new root node
            tree *X = tree::createNode(treeNode->left->left);
            tree *P = tree::createNode(treeNode->left->right);
            tree *E = tree::createNode(treeNode->left->left->right);
            treeNode->left = tree::createNode("lambda", "KEYWORD");
            treeNode->left->right = E;
            tree *lambdaNode = treeNode->left;
            lambdaNode->left = X;
            lambdaNode->left->right = P;
        }
    }
    else if (treeNode->getValue() == "and" && treeNode->left->getValue() == "=")
    /*
    and[ =++[ X E ]] => =[ ,[ X++ ] tau[ E++ ]]
    */
    {
        // new root node
        treeNode->setType("KEYWORD");
        treeNode->setValue("=");

        // descendents of new root node
        tree *equalNode = treeNode->left;
        treeNode->left = tree::createNode(",", "PUNCTION");
        tree *commaNode = treeNode->left;
        commaNode->left = tree::createNode(equalNode->left);
        treeNode->left->right = tree::createNode("tau", "KEYWORD");
        tree *tauNode = treeNode->left->right;
        tauNode->left = tree::createNode(equalNode->left->right);
        tauNode = tauNode->left;
        commaNode = commaNode->left;
        equalNode = equalNode->right;
        // recursively create and add nodes for other assignment operations
        while (equalNode != NULL) {
            commaNode->right = tree::createNode(equalNode->left);
            commaNode = commaNode->right;
            tauNode->right = tree::createNode(equalNode->left->right);
            tauNode = tauNode->right;
            equalNode = equalNode->right;
        }
    }
    else if (treeNode->getValue() == "where")
    /*
    where[ P =[ X E ] ] => gamma[ lambda[ X P ] E ]
    */
    {
        // new root node
        treeNode->setType("KEYWORD");
        treeNode->setValue("gamma");

        if (treeNode->left->right->getValue() == "=") {
            // descendents of new root node
            tree *P = tree::createNode(treeNode->left);
            tree *X = tree::createNode(treeNode->left->right->left);
            tree *E = tree::createNode(treeNode->left->right->left->right);
            treeNode->left = tree::createNode("lambda", "KEYWORD");
            treeNode->left->right = E;
            treeNode->left->left = X;
            treeNode->left->left->right = P;
        }
    }
    else if (treeNode->getValue() == "within")
    /*
    within[ =[ X1 E1 ] =[ X2 E2 ] ] => =[ X2 gamma[ lambda[ X1 E2 ] E1 ]]
    */
    {
        if (treeNode->left->getValue() == "=" && treeNode->left->right->getValue() == "=")
        {
            // new root node
            treeNode->setType("KEYWORD");
            treeNode->setValue("=");

            // descendents of new root node
            tree *X1 = tree::createNode(treeNode->left->left);
            tree *E1 = tree::createNode(treeNode->left->left->right);
            tree *X2 = tree::createNode(treeNode->left->right->left);
            tree *E2 = tree::createNode(treeNode->left->right->left->right);
            treeNode->left = X2;
            treeNode->left->right = tree::createNode("gamma", "KEYWORD");
            tree *gammaNode = treeNode->left->right;
            gammaNode->left = tree::createNode("lambda", "KEYWORD");
            gammaNode->left->right = E1;
            gammaNode = gammaNode->left;
            gammaNode->left = X1;
            gammaNode->left->right = E2;
        }
    }
    else if (treeNode->getValue() == "rec" && treeNode->left->getValue() == "=")
    /*
    rec[ =[ X E ] ] => =[ X gamma[ Ystar lambda[ X E ] ] ]
    */
    {
        // new root node
        treeNode->setType("KEYWORD");
        treeNode->setValue("=");

        // descendents of new root node
        tree *X = tree::createNode(treeNode->left->left);
        tree *E = tree::createNode(treeNode->left->left->right);
        treeNode->left = X;
        treeNode->left->right = tree::createNode("gamma", "KEYWORD");
        treeNode->left->right->left = tree::createNode("YSTAR", "KEYWORD");
        tree *ystarNode = treeNode->left->right->left;
        ystarNode->right = tree::createNode("lambda", "KEYWORD");
        ystarNode->right->left = tree::createNode(X);
        ystarNode->right->left->right = tree::createNode(E);
    }
    else if (treeNode->getValue() == "function_form")
    /*
    fcn_form[ P V+ E ] => =[ P +lambda[ V .E ]]
    */
    {
        // new root node
        treeNode->setType("KEYWORD");
        treeNode->setValue("=");

        // descendents of new root node
        tree *P = tree::createNode(treeNode->left);
        tree *V = treeNode->left->right;
        treeNode->left = P;
        tree *lambdaNode = treeNode;
        // recursively create and add nodes for other function parameters
        while (V->right->right != NULL) {
            lambdaNode->left->right = tree::createNode("lambda", "KEYWORD");
            lambdaNode = lambdaNode->left->right;
            lambdaNode->left = tree::createNode(V);
            V = V->right;
        }
        // last function parameter to be added
        lambdaNode->left->right = tree::createNode("lambda", "KEYWORD");
        lambdaNode = lambdaNode->left->right;
        lambdaNode->left = tree::createNode(V);
        lambdaNode->left->right = V->right;
    }
    else if (treeNode->getValue() == "lambda")
    /*
    lambda[ V++ E ] => ++lambda[ V .E ]
    */
    {
        // recursively create and add nodes for lambda function parameters
        if (treeNode->left != NULL) {
            tree *V = treeNode->left;
            tree *lambdaNode = treeNode;
            if (V->right != NULL && V->right->right != NULL) {
                while (V->right->right != NULL) {
                    lambdaNode->left->right = tree::createNode("lambda", "KEYWORD");
                    lambdaNode = lambdaNode->left->right;
                    lambdaNode->left = tree::createNode(V);
                    V = V->right;
                }
                // last function parameter to be added
                lambdaNode->left->right = tree::createNode("lambda", "KEYWORD");
                lambdaNode = lambdaNode->left->right;
                lambdaNode->left = tree::createNode(V);
                lambdaNode->left->right = V->right;
            }
        }
    }
    else if (treeNode->getValue() == "@")
    /*
    @[ E1 N E2 ] => gamma[ gamma[ N E1 ] E2 ]
    */
    {
        // new root node
        treeNode->setType("KEYWORD");
        treeNode->setValue("gamma");

        // descendents of new root node
        tree *E1 = tree::createNode(treeNode->left);
        tree *N = tree::createNode(treeNode->left->right);
        tree *E2 = tree::createNode(treeNode->left->right->right);
        treeNode->left = tree::createNode("gamma", "KEYWORD");
        treeNode->left->right = E2;
        treeNode->left->left = N;
        treeNode->left->left->right = E1;
    }
    return NULL;
}

#endif // RPAL_STANDARDIZETREE_H
