#ifndef B36A04FB_AF91_4DCE_967C_77A5166758E1
#define B36A04FB_AF91_4DCE_967C_77A5166758E1

#include <stack>
#include <vector>
#include <iostream>
#include <sstream>

#include "cseEnvironment.h"
#include "tree.h"
#include "typeDef.h"
using namespace std;

// Create Control Structures for CSE machine
void buildControlStructures(tree *treeNode, tree *(*controlStructsMap)[300]) {
    static int rowIndex = 0;
    static int columnIndex = 0;
    static int nextDeltaIndex = 1;  // Index for next delta
    
    if (treeNode == NULL) return;

    // for lambda nodes
    if (treeNode->getValue() == "lambda") {
        int currentRow = rowIndex;
        controlStructsMap[rowIndex][columnIndex] = tree::createNode("", "");
        rowIndex = 0;

        // find next non-empty row in the control structures map
        while (controlStructsMap[rowIndex][0] != NULL) rowIndex++;
        
        nextDeltaIndex++;   // increment next delta index

        // create a delta number node
        stringstream strStream;
        strStream << rowIndex;
        rowIndex = currentRow;
        string deltaNumStr = strStream.str();
        tree *deltaNumNode = tree::createNode(deltaNumStr, "deltaNumber");

        // add the delta number node to the control structures map
        controlStructsMap[rowIndex][columnIndex++] = deltaNumNode;
        controlStructsMap[rowIndex][columnIndex++] = treeNode->left;
        controlStructsMap[rowIndex][columnIndex++] = treeNode;
        int currentColumn = columnIndex + 3;

        // find next non-empty row in the control structures map
        while (controlStructsMap[rowIndex][0] != NULL) rowIndex++;
        columnIndex = 0;

        // build control structure for the lambda node
        buildControlStructures(treeNode->left->right, controlStructsMap);

        // reassign current index values
        rowIndex = currentRow;
        columnIndex = currentColumn;
    }
    // for conditional nodes
    else if (treeNode->getValue() == "->") {
        int currentColumn = columnIndex;
        int currentRow = rowIndex;
        int tempRow = rowIndex;
        int nextDelta = nextDeltaIndex;

        // stringstream to convert int to string
        stringstream strStream;
        stringstream strStream2;
        stringstream strStream3;
        stringstream strStream4;

        // create a delta number node for true branch
        strStream << nextDelta;
        string str2 = strStream.str();
        tree *temp1 = tree::createNode(str2, "deltaNumber");
        // add the delta number node to the control structures map
        controlStructsMap[rowIndex][columnIndex++] = temp1;

        // create a delta number node for false branch
        strStream2 << nextDeltaIndex;
        string str3 = strStream2.str();
        tree *temp2 = tree::createNode(str3, "deltaNumber");
        // add the delta number node to the control structures map
        controlStructsMap[rowIndex][columnIndex++] = temp2;

        // create a beta node to represent the condition
        tree *beta = tree::createNode("beta", "beta");
        // add the beta node to the control structures map
        controlStructsMap[rowIndex][columnIndex++] = beta;

        // find next non-empty row in the control structures map
        while (controlStructsMap[tempRow][0] != NULL) tempRow++;
        int lamdaCount = nextDeltaIndex;

        // build control structures for the condition
        buildControlStructures(treeNode->left, controlStructsMap);
        int difficultLevel = nextDeltaIndex - lamdaCount;

        while (controlStructsMap[rowIndex][0] != NULL) rowIndex++;
        columnIndex = 0;

        // build control structures for the true branch
        buildControlStructures(treeNode->left->right, controlStructsMap);

        while (controlStructsMap[rowIndex][0] != NULL) rowIndex++;
        columnIndex = 0;

        // build control structures for the false branch
        buildControlStructures(treeNode->left->right->right, controlStructsMap);

        // create reference to delta, containing true branch
        if (difficultLevel == 0 || rowIndex < lamdaCount) {
            strStream3 << tempRow;
        } else {
            strStream3 << rowIndex - 1;
        }
        string str5 = strStream3.str();
        controlStructsMap[currentRow][currentColumn]->setValue(str5);
        
        // create reference to delta, containing false branch
        strStream4 << rowIndex;
        string str6 = strStream4.str();
        controlStructsMap[currentRow][currentColumn + 1]->setValue(str6);

        // reassign current index values
        rowIndex = currentRow;
        columnIndex = 0;
    }
    // for tau nodes
    else if (treeNode->getValue() == "tau") {
        // count the number of children
        tree *leftChild = treeNode->left;
        int childCount = 0;
        while (leftChild != NULL) {
            childCount++;
            leftChild = leftChild->right;
        }

        // create a node with the number of children
        stringstream strStream;
        strStream << childCount;
        string childCountStr = strStream.str();
        tree *countNode = tree::createNode(childCountStr, "CHILDCOUNT");
        // add the child count node to the control structures map
        controlStructsMap[rowIndex][columnIndex++] = countNode;

        // create a tau node
        tree *tauNode = tree::createNode("tau", "tau");
        // add the tau node to the control structures map
        controlStructsMap[rowIndex][columnIndex++] = tauNode;

        // build control structures for the children (in case of nested tuples)
        buildControlStructures(treeNode->left, controlStructsMap);
        treeNode = treeNode->left;
        while (treeNode != NULL) {
            buildControlStructures(treeNode->right, controlStructsMap);
            treeNode = treeNode->right;
        }
    }
    // for other nodes
    else {
        // create a node with same value and type as the current node and add it to the control structures map
        controlStructsMap[rowIndex][columnIndex++] = tree::createNode(treeNode->getValue(), treeNode->getType());
        // build control structures for the left and right children
        buildControlStructures(treeNode->left, controlStructsMap);
        if (treeNode->left != NULL) buildControlStructures(treeNode->left->right, controlStructsMap);
    }
}


#endif /* B36A04FB_AF91_4DCE_967C_77A5166758E1 */
