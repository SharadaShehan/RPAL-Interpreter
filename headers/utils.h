#ifndef RPAL_UTILS_H
#define RPAL_UTILS_H

#include "parameters.h"
#include "scanner.h"
#include "tree.h"


// Build tree for the input token
void buildTree(string val, string type, int child, parameters* params)
{
    // child is the number of descendants of the current node
    if (child == 0) // Leaf node case
    {
        tree *newNode = tree::createNode(val, type);
        params->syntaxTree.push(newNode);  // push the node to the tree stack
    }
    else if (child > 0)
    {
        int childCount = child; // copy the number of descendants to a variable
        stack<tree*> tempNodeStack;

        // pop the descendants from the main stack and add them to the temporary stack
        int removedCount = 0;
        while (removedCount < childCount && !params->syntaxTree.empty())
        {
            tempNodeStack.push(params->syntaxTree.top());
            params->syntaxTree.pop();
            removedCount++;
        }

        tree *leftChild = tempNodeStack.top();
        tempNodeStack.pop();    // pop the left child
        childCount--;

        // If there are more descendants to add, add them to the right of the left child
        if (childCount > 0 && !tempNodeStack.empty())
        {
            tree *rightNode = tempNodeStack.top();
            leftChild->right = rightNode;
            tempNodeStack.pop();
            childCount--;

            // Add the remaining descendants to the right of the right child
            while (childCount > 0 && !tempNodeStack.empty())
            {
                tree *addRight = tempNodeStack.top();
                tempNodeStack.pop();
                rightNode->right = addRight;
                rightNode = rightNode->right;
                childCount--;
            }
        }

        // Create a new tree node with the given value and type
        tree *toPush = tree::createNode(val, type);
        // Set the left child of the new node
        toPush->left = leftChild;
        // Push the new node to the main stack
        params->syntaxTree.push(toPush);
    }
}

// Read the next token recursively and build the tree
void read(string val, string type, parameters* params)
{
    // Check if the next token does not match the expected token
    if (type != params->nextToken.getType() || val != params->nextToken.getValue())
    {
        // Parse error
        cout << "Unable to Parse: Expected " << "\"" << val << "\"" << " but " << "\"" << params->nextToken.getValue() << "\"" << " was there" << endl;
        // Exit the program
        exit(0);
    }

    // If the token is an identifier, integer or string build a new tree node
    if (type == "ID" || type == "STR" || type == "INT") buildTree(val, type, 0, params);

    // Read the next token
    params->nextToken = scan(params->programString, params->sizeOfFile, params->index);
    // Repeat until a non-DELETE token is found
    while(params->nextToken.getType() == "DELETE")
        params->nextToken = scan(params->programString, params->sizeOfFile, params->index);
}

#endif //RPAL_UTILS_H
