#ifndef TREE_H_
#define TREE_H_
#include <stack>
#include <iostream>

using namespace std;

// Syntax tree node
class tree {
	private:
		string value;	 // Value of current node
		string type; // Type of current node
	public:
		tree *left;	// Left child
		tree *right; // Right child

	// Getters and setters for value and type
	string getType() {
		return type;
	}
	string getValue() {
		return value;
	}
	void setType(string nodeType) {
		type = nodeType;
	}
	void setValue(string nodeValue) {
		value = nodeValue;
	}

	// Create a new tree node with the given value and type
	static tree *createNode(string value, string type)
	{
		tree *treeNode = new tree();

		treeNode->setType(type);
		treeNode->setValue(value);
		treeNode->left = NULL;
		treeNode->right = NULL;

		return treeNode;
	}

	// create a new tree node from the given tree node
	static tree *createNode(tree *x)
	{
		tree *treeNode = new tree();

		treeNode->setType(x->getType());
		treeNode->setValue(x->getValue());
		treeNode->left = x->left;
		treeNode->right = NULL;

		return treeNode;
	}

	// Print the syntax tree
	void printTree(int dotsCount)
	{
		// Print dots to nest the hierarchy
		for (int i = 0; i < dotsCount; i++) cout << ".";

		// If token type is ID, STR or INT print <type:val>
		if (type == "ID" || type == "STR" || type == "INT") cout << "<" << type << ":" << value << ">" << endl;
		// If token type is BOOL, NIL or DUMMY print <val>
		else if (type == "BOOL" || type == "NIL" || type == "DUMMY") cout << "<" << value << ">" << endl;
		// other tokens
		else cout << value << endl;

		// Print subtrees recursively
		if (left != NULL) left->printTree(dotsCount + 1);
		if (right != NULL) right->printTree(dotsCount);
	}
};

#endif
