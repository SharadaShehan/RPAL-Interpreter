#ifndef RPAL_CSEMACHINE_H
#define RPAL_CSEMACHINE_H

#include <stack>
#include <vector>
#include <iostream>
#include <sstream>

#include "cseEnvironment.h"
#include "tree.h"
#include "typeDef.h"
using namespace std;


// Create controlsStack Structures for CSE machine
void buildControlStructures(tree *treeNode, tree *(*controlStructuresMap)[300]) {
    static int rowIndex = 0;
    static int columnIndex = 0;
    static int nextDeltaIndex = 1;  // Index for next delta
    
    if (treeNode == NULL) return;

    // for lambda nodes
    if (treeNode->getValue() == "lambda") {
        int currentRow = rowIndex;
        controlStructuresMap[rowIndex][columnIndex] = tree::createNode("", "");
        rowIndex = 0;

        // find next non-empty row in the controlsStack structures map
        while (controlStructuresMap[rowIndex][0] != NULL) rowIndex++;
        
        nextDeltaIndex++;   // increment next delta index

        // create a delta number node
        stringstream strStream;
        strStream << rowIndex;
        rowIndex = currentRow;
        string deltaNumStr = strStream.str();
        tree *deltaNumNode = tree::createNode(deltaNumStr, "deltaNumber");

        // add the delta number node to the controlsStack structures map
        controlStructuresMap[rowIndex][columnIndex++] = deltaNumNode;
        controlStructuresMap[rowIndex][columnIndex++] = treeNode->left;
        controlStructuresMap[rowIndex][columnIndex++] = treeNode;
        int currentColumn = columnIndex + 3;

        // find next non-empty row in the controlsStack structures map
        while (controlStructuresMap[rowIndex][0] != NULL) rowIndex++;
        columnIndex = 0;

        // build controlsStack structure for the lambda node
        buildControlStructures(treeNode->left->right, controlStructuresMap);

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
        // add the delta number node to the controlsStack structures map
        controlStructuresMap[rowIndex][columnIndex++] = temp1;

        // create a delta number node for false branch
        strStream2 << nextDeltaIndex;
        string str3 = strStream2.str();
        tree *temp2 = tree::createNode(str3, "deltaNumber");
        // add the delta number node to the controlsStack structures map
        controlStructuresMap[rowIndex][columnIndex++] = temp2;

        // create a beta node to represent the condition
        tree *beta = tree::createNode("beta", "beta");
        // add the beta node to the controlsStack structures map
        controlStructuresMap[rowIndex][columnIndex++] = beta;

        // find next non-empty row in the controlsStack structures map
        while (controlStructuresMap[tempRow][0] != NULL) tempRow++;
        int lamdaCount = nextDeltaIndex;

        // build controlsStack structures for the condition
        buildControlStructures(treeNode->left, controlStructuresMap);
        int difficultLevel = nextDeltaIndex - lamdaCount;

        while (controlStructuresMap[rowIndex][0] != NULL) rowIndex++;
        columnIndex = 0;

        // build controlsStack structures for the true branch
        buildControlStructures(treeNode->left->right, controlStructuresMap);

        while (controlStructuresMap[rowIndex][0] != NULL) rowIndex++;
        columnIndex = 0;

        // build controlsStack structures for the false branch
        buildControlStructures(treeNode->left->right->right, controlStructuresMap);

        // create reference to delta, containing true branch
        if (difficultLevel == 0 || rowIndex < lamdaCount) {
            strStream3 << tempRow;
        } else {
            strStream3 << rowIndex - 1;
        }
        string str5 = strStream3.str();
        controlStructuresMap[currentRow][currentColumn]->setValue(str5);
        
        // create reference to delta, containing false branch
        strStream4 << rowIndex;
        string str6 = strStream4.str();
        controlStructuresMap[currentRow][currentColumn + 1]->setValue(str6);

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
        // add the child count node to the controlsStack structures map
        controlStructuresMap[rowIndex][columnIndex++] = countNode;

        // create a tau node
        tree *tauNode = tree::createNode("tau", "tau");
        // add the tau node to the controlsStack structures map
        controlStructuresMap[rowIndex][columnIndex++] = tauNode;

        // build controlsStack structures for the children (in case of nested tuples)
        buildControlStructures(treeNode->left, controlStructuresMap);
        treeNode = treeNode->left;
        while (treeNode != NULL) {
            buildControlStructures(treeNode->right, controlStructuresMap);
            treeNode = treeNode->right;
        }
    }
    // for other nodes
    else {
        // create a node with same value and type as the current node and add it to the controlsStack structures map
        controlStructuresMap[rowIndex][columnIndex++] = tree::createNode(treeNode->getValue(), treeNode->getType());
        // build controlsStack structures for the left and right children
        buildControlStructures(treeNode->left, controlStructuresMap);
        if (treeNode->left != NULL) buildControlStructures(treeNode->left->right, controlStructuresMap);
    }
}


// modify escape sequences in the string
string modifyString(string strValue) {
    // replace newline and tab characters with their escape sequences
    for (int i = 1; i < strValue.length(); i++) {
        // newline case
        if (strValue[i - 1] == 92 && (strValue[i] == 'n')) strValue.replace(i - 1, 2, "\\\n");
        // tab case
        else if (strValue[i - 1] == 92 && (strValue[i] == 't')) strValue.replace(i - 1, 2, "\\\t");
    }
    // remove backslashes and single quotes
    for (int i = 0; i < strValue.length(); i++)
        if (strValue[i] == 92 || strValue[i] == 39) strValue.replace(i, 1, "");

    // return the modified string
    return strValue;
}


// Rearrange tuple into a stack
void tupleToStack(tree *tauNode, stack<tree*> &nodeStack) {
    if (tauNode == NULL || tauNode->getValue() == "lamdaTuple") return;
    // push the node to the stack if it is not tau or nil types
    if (tauNode->getValue() != "tau" && tauNode->getValue() != "nil") nodeStack.push(tauNode);
    // recursively arrange the tuple elements into the stack
    tupleToStack(tauNode->left, nodeStack);
    tupleToStack(tauNode->right, nodeStack);
}


// Evaluate controlsStack structures using lambda calculus
void evaluateST(vector<vector<tree*>> &controlStructure) {
    stack<tree*> controlsStack;    // Stack for control structures
    stack<tree*> evaluatingStack;  // Stack for poped values from controlsStack
    stack<environment*> currentEnvironmentStack;    // Stack for current environment
    stack<environment*> environmentsStack;  // Stack for environments

    int nextEnvIndex = 0;    // Index of next environment to be created
    // Create initial environment
    environment *currentEnvironment = new environment();
    nextEnvIndex++;

    // create a node for the initial environment and push it to the stacks
    evaluatingStack.push(tree::createNode(currentEnvironment->envName, "ENV"));
    controlsStack.push(tree::createNode(currentEnvironment->envName, "ENV"));
    // push the initial environment to the environment stacks
    environmentsStack.push(currentEnvironment);
    currentEnvironmentStack.push(currentEnvironment);

    // Get the first control structure
    vector<tree*> delta0;
    delta0 = controlStructure.at(0);
    // push all nodes of the first control structure to the controlsStack stack
    for (int i = 0; i < delta0.size(); i++)
        controlsStack.push(delta0.at(i));

    while (!controlsStack.empty()) {
        tree *currentNode = controlsStack.top();    // Get the top node of the controlsStack stack
        controlsStack.pop();    // Pop the top node

        // set the type of the node to tau if it is nil
        if (currentNode->getValue() == "nil") currentNode->setType("tau");

        if (currentNode->getType() == "INT" || currentNode->getType() == "STR" || currentNode->getValue() == "lambda" || currentNode->getValue() == "YSTAR" || currentNode->getValue() == "Print" || currentNode->getValue() == "Isinteger" || currentNode->getValue() == "Istruthvalue" || currentNode->getValue() == "Isstring" || currentNode->getValue() == "Istuple" || currentNode->getValue() == "Isfunction" || currentNode->getValue() == "Isdummy" || currentNode->getValue() == "Stem" || currentNode->getValue() == "Stern" || currentNode->getValue() == "Conc" || currentNode->getType() == "BOOL" || currentNode->getType() == "NIL" || currentNode->getType() == "DUMMY" || currentNode->getValue() == "Order" || currentNode->getValue() == "nil") {
            if (currentNode->getValue() == "lambda") {
                // Pop variables bounded to lambda
                tree *variablesMap = controlsStack.top();
                controlsStack.pop();
                // Pop the delta node bound to lambda
                tree *nextDeltaIndex = controlsStack.top();
                controlsStack.pop();

                // create a new environment to be bound to lambda
                tree *env = tree::createNode(currentEnvironment->envName, "ENV");

                // push lambda into the stack with its parameters
                evaluatingStack.push(nextDeltaIndex);
                evaluatingStack.push(variablesMap);
                evaluatingStack.push(env);
                evaluatingStack.push(currentNode);
            } else {
                // push the node into the stack
                evaluatingStack.push(currentNode);
            }
        }
        // gamma node case
        else if (currentNode->getValue() == "gamma") {
            // get top of the evaluating stack to determine the which gamma rule to apply
            tree *evaluatingStackTop = evaluatingStack.top();

            // CSE Rule 4 - gamma & lambda
            if (evaluatingStackTop->getValue() == "lambda") {
                // Pop lambda node with its parameters
                evaluatingStack.pop();
                // Environment bounded to lambda
                tree *lambdaEnv = evaluatingStack.top();
                evaluatingStack.pop();
                // Variables bounded to lambda
                tree *variablesMap = evaluatingStack.top();
                evaluatingStack.pop();
                // Delta index bound to lambda
                tree *nextDeltaIndex = evaluatingStack.top();
                evaluatingStack.pop();

                // create a new environment for next delta expression evaluation
                environment *newEnvironment = new environment();
                std::stringstream strStream;
                strStream << nextEnvIndex;
                string str = strStream.str();
                newEnvironment->envName = "env" + str;  // Set the name of the new environment

                // Set the previous environment node of the new environment
                stack<environment*> tempEnv = environmentsStack;
                while (tempEnv.top()->envName != lambdaEnv->getValue()) tempEnv.pop();
                newEnvironment->prevEnv = tempEnv.top();

                // Chaeck value of Rand to determine the type of operation
                // Rand : tau case
                if (variablesMap->getValue() == "," && evaluatingStack.top()->getValue() == "tau") {
                    vector<tree *> boundVariables;
                    // get the bound variables of the tau
                    tree *nodeLeftToComa = variablesMap->left;
                    while (nodeLeftToComa != NULL) {
                        boundVariables.push_back(tree::createNode(nodeLeftToComa));
                        nodeLeftToComa = nodeLeftToComa->right;
                    }

                    // Pop tau node
                    tree *tauNode = evaluatingStack.top();
                    evaluatingStack.pop();

                    // get the values of the tau
                    vector<tree *> boundValues;
                    tree *tauValueNode = tauNode->left;
                    while (tauValueNode != NULL) {
                        boundValues.push_back(tauValueNode);
                        tauValueNode = tauValueNode->right;
                    }

                    // bind the variables to their values
                    for (int i = 0; i < boundValues.size(); i++) {
                        if (boundValues.at(i)->getValue() == "tau") {
                            stack<tree*> nodesStack;
                            tupleToStack(boundValues.at(i), nodesStack);
                        }
                        // create a vector of values for the bound variable
                        vector<tree*> nodeValuesVector;
                        nodeValuesVector.push_back(boundValues.at(i));
                        // Insert the bound variable and its value to the environment
                        newEnvironment->variablesMap.insert(pair<tree*,vector<tree*> >(boundVariables.at(i), nodeValuesVector));
                    }
                }
                // Rand : lambda case
                else if (evaluatingStack.top()->getValue() == "lambda") {
                    stack<tree*> nodesStack;
                    vector<tree*> nodeValuesVector;
                    tree *stackNode;

                    // get the bound variables of the lambda
                    for (int j = 0; j < 4; j++) {
                        nodesStack.push(evaluatingStack.top());
                        evaluatingStack.pop();
                    }
                    // add the values of the lambda to the vector
                    while (!nodesStack.empty()) {
                        stackNode = nodesStack.top();
                        nodesStack.pop();
                        nodeValuesVector.push_back(stackNode);
                    }
                    // Insert the bound variable and value to the environment
                    newEnvironment->variablesMap.insert(pair<tree *,vector<tree*>>(variablesMap, nodeValuesVector));
                }
                // Rand : Conc case
                else if (evaluatingStack.top()->getValue() == "Conc") {
                    stack<tree*> nodesStack;
                    vector<tree*> nodeValuesVector;
                    tree *stackNode;

                    // get the values of the conc
                    for (int j = 0; j < 2; j++) {
                        nodesStack.push(evaluatingStack.top());
                        evaluatingStack.pop();
                    }
                    // add the values of the conc to the vector
                    while (!nodesStack.empty()) {
                        stackNode = nodesStack.top();
                        nodesStack.pop();
                        nodeValuesVector.push_back(stackNode);
                    }
                    // Insert the bound variable and value to the environment
                    newEnvironment->variablesMap.insert(pair<tree*,vector<tree*>>(variablesMap, nodeValuesVector));
                }
                // Rand : eta case
                else if (evaluatingStack.top()->getValue() == "eta") {
                    stack<tree*> nodesStack;
                    vector<tree*> nodeValuesVector;
                    tree *stackNode;

                    // get the values of the eta
                    for (int j = 0; j < 4; j++) {
                        nodesStack.push(evaluatingStack.top());
                        evaluatingStack.pop();
                    }
                    // add the values of the eta to the vector
                    while (!nodesStack.empty()) {
                        stackNode = nodesStack.top();
                        nodesStack.pop();
                        nodeValuesVector.push_back(stackNode);
                    }
                    // Insert the bound variable and value to the environment
                    newEnvironment->variablesMap.insert(pair<tree*,vector<tree*>>(variablesMap, nodeValuesVector));
                }
                // Rand : Int case
                else {
                    // create a vector of values for the bound variable
                    vector<tree*> nodeValuesVector;
                    tree *intNode = evaluatingStack.top();
                    evaluatingStack.pop();
                    nodeValuesVector.push_back(intNode);
                    // Insert the bound variable and value to the environment
                    newEnvironment->variablesMap.insert(pair<tree*,vector<tree*>>(variablesMap, nodeValuesVector));
                }

                // push the new environment to the environment stacks
                currentEnvironment = newEnvironment;
                evaluatingStack.push(tree::createNode(currentEnvironment->envName, "ENV"));
                controlsStack.push(tree::createNode(currentEnvironment->envName, "ENV"));
                currentEnvironmentStack.push(currentEnvironment);
                environmentsStack.push(currentEnvironment);

                // get the next delta index
                int deltaIndex;
                istringstream isStrStream(nextDeltaIndex->getValue());
                isStrStream >> deltaIndex;

                // get next delta statement (control structure)
                vector<tree*> nextDelta = controlStructure.at(deltaIndex);
                // push all nodes of the next delta statement to the controlsStack stack
                for (int i = 0; i < nextDelta.size(); i++) controlsStack.push(nextDelta.at(i));
                // increment the next delta index
                nextEnvIndex++;
            }
            // CSE Rule 10 - tuple selection
            else if (evaluatingStackTop->getValue() == "tau") {
                // get tau node from evaluating stack
                tree *tauNode = evaluatingStack.top();
                evaluatingStack.pop();
                // get the index of the child to be selected
                tree *indexNode = evaluatingStack.top(); 
                evaluatingStack.pop();

                // convert the index to integer
                istringstream indexStream(indexNode->getValue());
                int selectionIndex;
                indexStream >> selectionIndex;

                // get the selected child
                tree *tauChild = tauNode->left;  // if first child is selected, will not enter the loop
                // loop till the selected child is reached
                while (selectionIndex > 1) {
                    selectionIndex--;
                    tauChild = tauChild->right;
                }

                // push the selected child to the evaluating stack
                tree *selectedChild;
                selectedChild = tree::createNode(tauChild);
                // nested tuple case
                if (selectedChild->getValue() == "lamdaTuple") {
                    tree *getNode = selectedChild->left;
                    // push the tuple elements to the stack
                    while (getNode != NULL) {
                        evaluatingStack.push(tree::createNode(getNode));
                        getNode = getNode->right;
                    }
                } else {
                    // push the selected child to the stack if it is not a tuple
                    evaluatingStack.push(selectedChild);
                }
            }
            // CSE Rule 12 - y* case
            else if (evaluatingStackTop->getValue() == "YSTAR") {
                // get the YSTAR node from the evaluating stack
                evaluatingStack.pop();
                if (evaluatingStack.top()->getValue() == "lambda") {
                    // create an eta node
                    tree *etaNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType());
                    etaNode->setValue("eta");
                    evaluatingStack.pop();

                    // get the bound environment
                    tree *boundEnvironment = evaluatingStack.top();
                    evaluatingStack.pop();
                    // get bound variables
                    tree *boundVariables = evaluatingStack.top();
                    evaluatingStack.pop();
                    // get the index of next controlsStack structure
                    tree *deltaIndex = evaluatingStack.top();
                    evaluatingStack.pop();

                    // Push nodes back to the stack
                    evaluatingStack.push(deltaIndex);
                    evaluatingStack.push(boundVariables);
                    evaluatingStack.push(boundEnvironment);
                    evaluatingStack.push(etaNode);
                }
                else return; // return if the next node is not lambda
            }
            // CSE Rule 13 - f.p case
            else if (evaluatingStackTop->getValue() == "eta") {
                // get the eta node from the evaluating stack
                tree *eta = evaluatingStack.top();
                evaluatingStack.pop();
                // get the bound environment
                tree *boundEnvironment = evaluatingStack.top();
                evaluatingStack.pop();
                // get the bound variables
                tree *boundVariables = evaluatingStack.top();
                evaluatingStack.pop();
                // get the index of next controlsStack structure
                tree *deltaIndex = evaluatingStack.top();
                evaluatingStack.pop();

                // push the eta node back into the stack
                evaluatingStack.push(deltaIndex);
                evaluatingStack.push(boundVariables);
                evaluatingStack.push(boundEnvironment);
                evaluatingStack.push(eta);

                // push another eta node to the stack
                evaluatingStack.push(deltaIndex);
                evaluatingStack.push(boundVariables);
                evaluatingStack.push(boundEnvironment);
                evaluatingStack.push(tree::createNode("lambda", "KEYWORD"));

                // push two gamma nodes to the stack
                controlsStack.push(tree::createNode("gamma", "KEYWORD"));
                controlsStack.push(tree::createNode("gamma", "KEYWORD"));
            }
            // if node on top of evaluating stack is a 'print' node
            else if (evaluatingStackTop->getValue() == "Print") {
                // get the 'print' node
                evaluatingStack.pop();
                tree *nextToPrint = evaluatingStack.top();

                // If the next node in evaluating stack is a tuple
                if (nextToPrint->getValue() == "tau") {
                    // get the tuple node
                    tree *tupleNode = evaluatingStack.top();
                    // create a stack to store the tuple elements
                    stack<tree*> elementsStack;
                    tupleToStack(tupleNode, elementsStack);

                    // reverse the stack
                    stack<tree *> reversedStack;
                    while (!elementsStack.empty()) {
                        reversedStack.push(elementsStack.top());
                        elementsStack.pop();
                    }
                    
                    // print the tuple elements within parentheses
                    cout << "(";
                    while (reversedStack.size() > 1) {
                        if (reversedStack.top()->getType() == "STR")
                            // if the element is a string, modify the escape sequences
                            cout << modifyString(reversedStack.top()->getValue()) << ", ";
                        else
                            // else print original value
                            cout << reversedStack.top()->getValue() << ", ";
                        reversedStack.pop();
                    }

                    // last element of the tuple
                    // print modified string if the last element is a string
                    if (reversedStack.top()->getType() == "STR") cout << modifyString(reversedStack.top()->getValue()) << ")";
                    // print the last element if it is not a string
                    else cout << reversedStack.top()->getValue() << ")";
                    reversedStack.pop();
                }
                // next item to print is a lambda
                else if (nextToPrint->getValue() == "lambda") {
                    // get lambda node
                    evaluatingStack.pop();

                    // get environment bounded to lambda
                    tree *boundEnvironment = evaluatingStack.top();
                    evaluatingStack.pop();
                    // get the variables bounded to lambda
                    tree *boundVariables = evaluatingStack.top();
                    evaluatingStack.pop();
                    // get index of next controlsStack structure
                    tree *nextDelta = evaluatingStack.top();
                    evaluatingStack.pop();

                    // format to print lambda node
                    cout << "[lambda closure: " << boundVariables->getValue() << ": " << nextDelta->getValue() << "]";
                    return;
                }
                // If the next node is either string or integer
                else {
                    // print modified string if the next node is a string
                    if (evaluatingStack.top()->getType() == "STR") cout << modifyString(evaluatingStack.top()->getValue());
                    // print the next node if it is not a string 
                    else cout << evaluatingStack.top()->getValue();
                }
            }
            // If top node of evaluating stack is 'Isinteger'
            else if (evaluatingStackTop->getValue() == "Isinteger") {
                // get the 'Isinteger' node
                evaluatingStack.pop();
                // get next node in stack (contains the value to check)
                tree *isNextInt = evaluatingStack.top();
                evaluatingStack.pop();

                // check if the next node is an integer
                if (isNextInt->getType() == "INT") {
                    // push boolean 'true' node to the stack
                    tree *booleanNode = tree::createNode("true", "boolean");
                    evaluatingStack.push(booleanNode);
                } else {
                    // push boolean 'false' node to the stack
                    tree *booleanNode = tree::createNode("false", "boolean");
                    evaluatingStack.push(booleanNode);
                }
            }
            // check if next node is a 'Istruthvalue' node
            else if (evaluatingStackTop->getValue() == "Istruthvalue") {
                // get the 'Istruthvalue' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *booleanNode = evaluatingStack.top();
                evaluatingStack.pop();

                // check if the next node has a valid boolean value
                if (booleanNode->getValue() == "true" || booleanNode->getValue() == "false") {
                    // create a boolean node with value 'true'
                    tree *newBoolNode = tree::createNode("true", "BOOL");
                    evaluatingStack.push(newBoolNode);
                } else {
                    // create a boolean node with value 'false'
                    tree *newBoolNode = tree::createNode("false", "BOOL");
                    evaluatingStack.push(newBoolNode);
                }
            }
            // Check if next item in stack is a 'Isstring' node
            else if (evaluatingStackTop->getValue() == "Isstring") {
                // get the 'Isstring' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *isStringNode = evaluatingStack.top();
                evaluatingStack.pop();

                // check if the next node is a string
                if (isStringNode->getType() == "STR") {
                    // push boolean 'true' node to the stack
                    tree *booleanNode = tree::createNode("true", "BOOL");
                    evaluatingStack.push(booleanNode);
                } else {
                    // push boolean 'false' node to the stack
                    tree *booleanNode = tree::createNode("false", "BOOL");
                    evaluatingStack.push(booleanNode);
                }
            }
            // Check if next node in stack is a 'Istuple' node
            else if (evaluatingStackTop->getValue() == "Istuple") {
                // get the 'Istuple' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *isTauNode = evaluatingStack.top();
                evaluatingStack.pop();

                // check if the next node is a tuple
                if (isTauNode->getType() == "tau") {
                    // push boolean 'true' node to the stack
                    tree *booleanNode = tree::createNode("true", "BOOL");
                    evaluatingStack.push(booleanNode);
                } else {
                    // push boolean 'false' node to the stack
                    tree *booleanNode = tree::createNode("false", "BOOL");
                    evaluatingStack.push(booleanNode);
                }
            }
            // Check if next node in stack is a 'Isfunction' node
            else if (evaluatingStackTop->getValue() == "Isfunction") {
                // get the 'Isfunction' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *isFunctionNode = evaluatingStack.top();

                // check if the next node is a lambda
                if (isFunctionNode->getValue() == "lambda") {
                    // push boolean 'true' node to the stack
                    tree *booleanNode = tree::createNode("true", "BOOL");
                    evaluatingStack.push(booleanNode);
                } else {
                    // push boolean 'false' node to the stack
                    tree *booleanNode = tree::createNode("false", "BOOL");
                    evaluatingStack.push(booleanNode);
                }
            }
            // Check if next node in stack is a 'Isdummy' node
            else if (evaluatingStackTop->getValue() == "Isdummy") {
                // get the 'Isdummy' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *isDummyNode = evaluatingStack.top();

                // check if the next node is a dummy
                if (isDummyNode->getValue() == "dummy") {
                    // push boolean 'true' node to the stack
                    tree *booleanNode = tree::createNode("true", "BOOL");
                    evaluatingStack.push(booleanNode);
                } else {
                    // push boolean 'false' node to the stack
                    tree *booleanNode = tree::createNode("false", "BOOL");
                    evaluatingStack.push(booleanNode);
                }
            }
            // check if next node in stack is a 'Stem' node
            else if (evaluatingStackTop->getValue() == "Stem") {
                // get the 'Stem' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *stringNode = evaluatingStack.top();

                // if next node is an empty string, operation cannot be performed
                if (stringNode->getValue() == "") return;

                // check if the next node is a string
                if (stringNode->getType() == "STR") {
                    // Get first character of the string
                    string fCharacterStr = "'" + stringNode->getValue().substr(1, 1) + "'";
                    evaluatingStack.pop();
                    evaluatingStack.push(tree::createNode(fCharacterStr, "STR"));
                }
            }
            // check if next node in stack is a 'Stern' node
            else if (evaluatingStackTop->getValue() == "Stern") {
                // get the 'Stern' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *isNextString = evaluatingStack.top();

                // if next node is an empty string, operation cannot be performed
                if (isNextString->getValue() == "") return;

                // check if the next node is a string
                if (isNextString->getType() == "STR") {
                    // Get remaining characters
                    string strRes = "'" + isNextString->getValue().substr(2, isNextString->getValue().length() - 3);
                    evaluatingStack.pop();
                    evaluatingStack.push(tree::createNode(strRes, "STR"));
                }
            }
            // check if next node in stack is an 'Order' node
            else if (evaluatingStackTop->getValue() == "Order") {
                // get the 'Order' node
                evaluatingStack.pop();
                // get the next node in stack
                tree *tupleNode = evaluatingStack.top();
    
                // count the number of items in the tuple
                int numOfItems = 0;
                if (tupleNode->left != NULL) tupleNode = tupleNode->left;
                while (tupleNode != NULL) {
                    numOfItems++;
                    tupleNode = tupleNode->right;
                }

                // point to the original tuple node (after counting the items)
                tupleNode = evaluatingStack.top();
                evaluatingStack.pop();

                // empty tuple case
                if ((tupleNode->getValue() == "nil")) numOfItems = 0;

                // create a node with the number of items in the tuple and push it to the stack
                stringstream strStream;
                strStream << numOfItems;
                string itemsCountStr = strStream.str();
                tree *orderNode = tree::createNode(itemsCountStr, "INT");
                evaluatingStack.push(orderNode);
            }
            // check if next node in stack is a 'Conc' node
            else if (evaluatingStackTop->getValue() == "Conc") {
                // get the 'Conc' node
                tree *concNode = evaluatingStack.top();
                evaluatingStack.pop();
                // get the first string
                tree *firstString = evaluatingStack.top();
                evaluatingStack.pop();
                // get the second string
                tree *secondString = evaluatingStack.top();

                if (secondString->getType() == "STR" || (secondString->getType() == "STR" && secondString->left != NULL && secondString->left->getValue() == "true")) {
                    evaluatingStack.pop();
                    //concatenate the two strings
                    string conatenatedString = "'" + firstString->getValue().substr(1, firstString->getValue().length() - 2) + secondString->getValue().substr(1, secondString->getValue().length() - 2) + "'";
                    // create a node with the concatenated string and push it to the controls stack
                    tree *conatenatedNode = tree::createNode(conatenatedString, "STR");
                    evaluatingStack.push(conatenatedNode);
                    controlsStack.pop();
                } else {
                    // if second string is not a string
                    concNode->left = firstString;
                    // push the conc node back to the stack
                    evaluatingStack.push(concNode);
                    // create a node for the second string and push it to the stack
                    firstString->left = tree::createNode("true", "flag");
                }
            }
        }
        // CSE Rule 5 - exit env
        // check if node is an environment
        else if (currentNode->getValue().substr(0, 3) == "env") {
            stack<tree *> valueNodesBeforeEnvNode;
            if (evaluatingStack.top()->getValue() == "lambda") {
                // if the node is a lambda node get its environment, variables and next delta index together with the lambda node
                valueNodesBeforeEnvNode.push(evaluatingStack.top());
                evaluatingStack.pop();
                valueNodesBeforeEnvNode.push(evaluatingStack.top());
                evaluatingStack.pop();
                valueNodesBeforeEnvNode.push(evaluatingStack.top());
                evaluatingStack.pop();
                valueNodesBeforeEnvNode.push(evaluatingStack.top());
                evaluatingStack.pop();
            } else {
                // if node is not a lambda node, just get the next node
                valueNodesBeforeEnvNode.push(evaluatingStack.top());
                evaluatingStack.pop();
            }

            tree *environmentNode = evaluatingStack.top(); // Get the environment node to remov

            // check if the environment to remove is same in top of both stacks
            if (currentNode->getValue() == environmentNode->getValue()) {
                evaluatingStack.pop();
                currentEnvironmentStack.pop();
                if (!currentEnvironmentStack.empty()) currentEnvironment = currentEnvironmentStack.top();
                else currentEnvironment = NULL;
            } else return;
            // Push the value node back to the stack
            while (!valueNodesBeforeEnvNode.empty()) {
                evaluatingStack.push(valueNodesBeforeEnvNode.top());
                valueNodesBeforeEnvNode.pop();
            }
        }
        // If any variable node is on top of the controls stack
        else if (currentNode->getType() == "ID" && currentNode->getValue() != "Print" && currentNode->getValue() != "Isinteger" && currentNode->getValue() != "Istruthvalue" && currentNode->getValue() != "Isstring" && currentNode->getValue() != "Istuple" && currentNode->getValue() != "Isfunction" && currentNode->getValue() != "Isdummy" && currentNode->getValue() != "Stem" && currentNode->getValue() != "Stern" && currentNode->getValue() != "Conc") {
            environment *environmentNode = currentEnvironment;
            int breakFlag = 0;
            // Find the variable in the environment
            while (environmentNode != NULL) {
                // loop through the variables in the environment
                map<tree*,vector<tree*>>::iterator iter = environmentNode->variablesMap.begin();
                while (iter != environmentNode->variablesMap.end()) {
                    if (currentNode->getValue() == iter->first->getValue()) {   // node with matching variable
                        vector<tree*> environmentNode = iter->second;   // environment node
                        if (environmentNode.size() == 1 && environmentNode.at(0)->getValue() == "Conc" && environmentNode.at(0)->left != NULL) {
                            // If the variable is a conc node, create a new conc node and push it to the controls stack
                            controlsStack.push(tree::createNode("gamma", "KEYWORD"));
                            // push the left child of the conc node and variable node to the stack
                            evaluatingStack.push(environmentNode.at(0)->left);
                            evaluatingStack.push(environmentNode.at(0));
                        } else {
                            // non-conc node case
                            int index = 0;
                            while (index < environmentNode.size()) {
                                if (environmentNode.at(index)->getValue() == "lamdaTuple") {
                                    // if the node is a lambda tuple, push its elements to the stack
                                    tree *lambdaTupleItemNode = environmentNode.at(index)->left;
                                    while (lambdaTupleItemNode != NULL) {
                                        evaluatingStack.push(tree::createNode(lambdaTupleItemNode));
                                        lambdaTupleItemNode = lambdaTupleItemNode->right;
                                    }
                                } else {
                                    // push the node to the stack
                                    if (environmentNode.at(index)->getValue() == "tau") {
                                        stack<tree *> nodeStack;
                                        tupleToStack(environmentNode.at(index), nodeStack);
                                    }
                                    evaluatingStack.push(environmentNode.at(index));
                                }
                                index++;
                            }
                        }
                        // set flag to 1 to break the loop
                        breakFlag = 1;
                        break;
                    }
                    iter++;
                }
                if (breakFlag == 1) break;
                // move to the previous environment
                environmentNode = environmentNode->prevEnv;
            }
            // return if the variable is not found in the environment
            if (breakFlag == 0) return;
        }
        // CSE Rule 6 - binop & CSE Rule 7 - unop cases
        else if (isBinaryOperator(currentNode->getValue()) || currentNode->getValue() == "neg" || currentNode->getValue() == "not") {
            // get unop or binop node from the stack
            string operation = currentNode->getValue();
            if (isBinaryOperator(currentNode->getValue())) {
                // If the node is a binary operator, get the operands from the stack
                tree *operand1Node = evaluatingStack.top();    // first operand
                evaluatingStack.pop();
                tree *operand2Node = evaluatingStack.top();    // second operand
                evaluatingStack.pop();

                if (operand1Node->getType() == "INT" && operand2Node->getType() == "INT") {
                    // If both operands are integers
                    int number1, number2;

                    // convert the operand values to integers
                    istringstream strValueStrm1(operand1Node->getValue());
                    strValueStrm1 >> number1;
                    istringstream strValueStrm2(operand2Node->getValue());
                    strValueStrm2 >> number2;

                    // initialize variables for the result
                    int result = 0;
                    double resultWithFloat = 0;

                    // perform the operation based on the operator
                    if (operation == "+") {
                        // Addition
                        result = number1 + number2;
                        stringstream strStream;
                        strStream << result;
                        string resultStr = strStream.str();
                        tree *resultNode = tree::createNode(resultStr, "INT");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "-") {
                        // Subtraction
                        result = number1 - number2;
                        stringstream strStream;
                        strStream << result;
                        string resultStr = strStream.str();
                        tree *resultNode = tree::createNode(resultStr, "INT");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "*") {
                        // Multiplication
                        result = number1 * number2;
                        stringstream strStream;
                        strStream << result;
                        string resultStr = strStream.str();
                        tree *resultNode = tree::createNode(resultStr, "INT");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "/") {
                        // Division
                        if (number2 == 0) {
                            // If division by zero
                            cout << "Exception: STATUS_INTEGER_DIVIDE_BY_ZERO" << endl;
                            return;
                        }
                        resultWithFloat = (double)number1 / (double)number2;
                        stringstream strStream;
                        strStream << resultWithFloat;
                        string resultStr = strStream.str();
                        tree *resultNode = tree::createNode(resultStr, "INT");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "**") {
                        // Power
                        result = pow(number1, number2);
                        stringstream strStream;
                        strStream << result;
                        string resultStr = strStream.str();
                        tree *resultNode = tree::createNode(resultStr, "INT");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == ">" || operation == "gr") {
                        // Greater than
                        string resultStr = number1 > number2 ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "bool");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == ">=" || operation == "ge") {
                        // Greater than or equal to
                        string resultStr = number1 >= number2 ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "bool");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "<" || operation == "ls") {
                        // Less than
                        string resultStr = number1 < number2 ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "bool");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "<=" || operation == "le") {
                        // Less than or equal to
                        string resultStr = number1 <= number2 ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "bool");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "=" || operation == "eq") {
                        // Equal
                        string resultStr = number1 == number2 ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "bool");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "><" || operation == "ne") {
                        // Not equal
                        string resultStr = number1 != number2 ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "bool");
                        evaluatingStack.push(resultNode);
                    }
                }
                else if (operand1Node->getType() == "STR" && operand2Node->getType() == "STR") {
                    // If both operands are strings
                    if (operation == "ne" || operation == "<>") {
                        // Not equal
                        string resultStr = operand1Node->getValue() != operand2Node->getValue() ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "BOOL");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "eq" || operation == "==") {
                        // Equal
                        string resultStr = operand1Node->getValue() == operand2Node->getValue() ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "BOOL");
                        evaluatingStack.push(resultNode);
                    }
                    else {
                        return; // return if the operation is not supported
                    }
                }
                else if ((operand1Node->getValue() == "true" || operand1Node->getValue() == "false") && (operand2Node->getValue() == "false" || operand2Node->getValue() == "true")) {
                // If both operands are boolean
                    if (operation == "<>"|| operation == "ne") {
                        // Not equal
                        string resultStr = operand1Node->getValue() != operand2Node->getValue() ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "BOOL");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "==" || operation == "eq") {
                        // Equal
                        string resultStr = operand1Node->getValue() == operand2Node->getValue() ? "true" : "false";
                        tree *resultNode = tree::createNode(resultStr, "BOOL");
                        evaluatingStack.push(resultNode);
                    }
                    else if (operation == "or") {
                        // Or
                        string resultStr;
                        if (operand1Node->getValue() == "true" || operand2Node->getValue() == "true") {
                            // If either operand is true, result is true
                            resultStr = "true";
                            tree *resultNode = tree::createNode(resultStr, "BOOL");
                            evaluatingStack.push(resultNode);
                        } else {
                            resultStr = "false";
                            tree *resultNode = tree::createNode(resultStr, "BOOL");
                            evaluatingStack.push(resultNode);
                        }
                    }
                    else if (operation == "&") {
                        // And
                        string resultStr;
                        if (operand1Node->getValue() == "true" && operand2Node->getValue() == "true") {
                            // if both operands are true, result is true
                            resultStr = "true";
                            tree *resultNode = tree::createNode(resultStr, "BOOL");
                            evaluatingStack.push(resultNode);
                        } else {
                            resultStr = "false";
                            tree *resultNode = tree::createNode(resultStr, "BOOL");
                            evaluatingStack.push(resultNode);
                        }
                    }
                }
            }
            // If the operation is an unary operator - negation or not
            else if (operation == "neg" || operation == "not") {
                // get the operand
                tree *operandNode = evaluatingStack.top();
                evaluatingStack.pop();

                if (operation == "neg") {
                    // negation operation
                    int number, result;
                    istringstream valueStrm(operandNode->getValue());
                    valueStrm >> number;
                    result = -number;   // apply negation
                    stringstream resultStrm;
                    resultStrm << result;
                    string resultStr = resultStrm.str();
                    tree *resultNode = tree::createNode(resultStr, "INT");
                    evaluatingStack.push(resultNode);
                }
                else if (operation == "not" && (operandNode->getValue() == "true" || operandNode->getValue() == "false")) {
                    // not operation
                    if (operandNode->getValue() == "true") {
                        // true to false
                        evaluatingStack.push(tree::createNode("false", "BOOL"));
                    } else {
                        // false to true
                        evaluatingStack.push(tree::createNode("true", "BOOL"));
                    }
                }
            }
        }
        // CSE Rule 8 - conditional statement
        else if (currentNode->getValue() == "beta") {
            int deltaIndex;
            // get boolean value from the stack
            tree *booleanNode = evaluatingStack.top();
            evaluatingStack.pop();
            // get index of else statement from the stack
            tree *elseNode = controlsStack.top();
            controlsStack.pop();
            // get index of then statement from the stack
            tree *thenNode = controlsStack.top();
            controlsStack.pop();
            
            if (booleanNode->getValue() == "true") {
                // if true case, get index of then statement
                istringstream thenValueStrm(thenNode->getValue());
                thenValueStrm >> deltaIndex;
            } else {
                // if false case, get index of else statement
                istringstream elseValueStrm(elseNode->getValue());
                elseValueStrm >> deltaIndex;
            }
            // get selected next control structure from the controlStack
            vector<tree*> nextDeltaStatement = controlStructure.at(deltaIndex);
            // Push all the nodes in the next control structure to the controlStack
            for (int i = 0; i < nextDeltaStatement.size(); i++) controlsStack.push(nextDeltaStatement.at(i));
        }
        // CSE Rule 9 - tuple formation
        else if (currentNode->getValue() == "tau") {
            // get tau node from the stack
            tree *tupleNode = tree::createNode("tau", "tau");

            // get node with number of items in the tuple
            tree *countNode = controlsStack.top();
            controlsStack.pop();

            // get the number of items in the tuple
            int itemsCount;
            istringstream valueStrm(countNode->getValue());
            valueStrm >> itemsCount;

            if (evaluatingStack.top()->getValue() == "lambda") {
                // If the first item is a lambda token
                // get lambda node from the stack
                tree *lambdaNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType());
                evaluatingStack.pop();
                // get environment node bounded to lambda
                tree *environmentNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType());
                evaluatingStack.pop();
                // get variables bounded to lambda
                tree *variablesNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType());
                evaluatingStack.pop();
                // get index of next control structure
                tree *nextDeltaNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType());
                evaluatingStack.pop();

                // Create a lambda tuple and set parameters
                tree *newLambdaNode = tree::createNode("lamdaTuple", "lamdaTuple");
                newLambdaNode->left = nextDeltaNode;
                nextDeltaNode->right = variablesNode;
                variablesNode->right = environmentNode;
                environmentNode->right = lambdaNode;
                tupleNode->left = newLambdaNode;
            } else {
                // If first node is not a lambda token
                tupleNode->left = tree::createNode(evaluatingStack.top());
                evaluatingStack.pop();
            }

            // get first item in the tuple
            tree *tupleItemNode = tupleNode->left;

            // Point to each item in the tuple
            for (int i = 1; i < itemsCount; i++) {
                // get next item node from the stack
                tree *nextItemNode = evaluatingStack.top();
                // If the next item is a lambda node
                if (nextItemNode->getValue() == "lambda") {
                    // get lambda node from the stack
                    tree *lamdaNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType()); // Pop lambda
                    evaluatingStack.pop();
                    // get environment node bounded to lambda
                    tree *environmentNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType()); // Pop the environment in which it was created
                    evaluatingStack.pop();
                    // get variables bounded to lambda
                    tree *variablesNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType()); // Pop the variable bounded to lambda
                    evaluatingStack.pop();
                    // get index of next control structure
                    tree *nextDeltaIndexNode = tree::createNode(evaluatingStack.top()->getValue(), evaluatingStack.top()->getType()); // Pop the index of next controlsStack structure
                    evaluatingStack.pop();

                    // create a new lambda tuple and set parameters
                    tree *newLambdaNode = tree::createNode("lamdaTuple", "lamdaTuple");
                    newLambdaNode->left = nextDeltaIndexNode;
                    nextDeltaIndexNode->right = variablesNode;
                    variablesNode->right = environmentNode;
                    environmentNode->right = lamdaNode;
                    tupleItemNode->right = newLambdaNode;
                    tupleItemNode = tupleItemNode->right;
                } else {
                    // If next node is not a lambda node
                    evaluatingStack.pop();
                    // set tuple item node to point to the current item
                    tupleItemNode->right = nextItemNode;
                    tupleItemNode = tupleItemNode->right;
                }
            }
            // push the tuple node to the stack
            evaluatingStack.push(tupleNode);
        }
        // If aug is top node of the controls stack
        else if (currentNode->getValue() == "aug") {
            // get the first item from the stack
            tree *firstItemNode = tree::createNode(evaluatingStack.top());
            evaluatingStack.pop();
            // get the second item from the stack
            tree *secondItemNode = tree::createNode(evaluatingStack.top());
            evaluatingStack.pop();

            if (firstItemNode->getValue() == "nil" && secondItemNode->getValue() == "nil") {
                // If both nodes are nil
                tree *tupleNode = tree::createNode("tau", "tau");
                tupleNode->left = firstItemNode;
                evaluatingStack.push(tupleNode);
            } else if (firstItemNode->getValue() == "nil") {
                // If the first node is nil
                tree *tupleNode = tree::createNode("tau", "tau");
                tupleNode->left = secondItemNode;
                evaluatingStack.push(tupleNode);
            } else if (secondItemNode->getValue() == "nil") {
                // If the second node is nil
                tree *tupleNode = tree::createNode("tau", "tau");
                tupleNode->left = firstItemNode;
                evaluatingStack.push(tupleNode);
            } else if (firstItemNode->getType() != "tau") {
                // If the first node is not a tuple
                tree *tupleNode = secondItemNode->left;
                while (tupleNode->right != NULL) tupleNode = tupleNode->right;
                tupleNode->right = tree::createNode(firstItemNode);
                evaluatingStack.push(secondItemNode);
            } else if (secondItemNode->getType() != "tau") {
                // If the second node is not a tuple
                tree *tupleNode = firstItemNode->left;
                while (tupleNode->right != NULL) tupleNode = tupleNode->right;
                tupleNode->right = tree::createNode(secondItemNode);
                evaluatingStack.push(firstItemNode);
            } else {
                // If both nodes are tuples
                tree *tupleNode = tree::createNode("tau", "tau");
                tupleNode->left = firstItemNode;
                tupleNode->left->right = secondItemNode;
                evaluatingStack.push(tupleNode);
            }
        }
    }
    cout << endl;
}

#endif // RPAL_CSEMACHINE_H
