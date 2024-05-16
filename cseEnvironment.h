#ifndef RPAL_CSEENVIRONMENT_H
#define RPAL_CSEENVIRONMENT_H

#include <vector>
#include <iostream>
#include <map>
#include "tree.h"
using namespace std;

// Creates environments to store variables and their values in the current scope
class environment {
public:
	string envName; // name of current environment
	environment *prevEnv; // previous environment
	map<tree*,vector<tree*>> variablesMap; // map of variables and their values in the current environment

	// creates a new environment with default values
	environment() {
		prevEnv = NULL; // By default, there is no previous environment
		envName = "env0"; // By default, the environment is named env0
	}

	// creates a new environment with given values
	environment(environment *prev, string name) {
		prevEnv = prev; // sets the previous environment to the given environment
		envName = name; // sets the name of the environment to the given name
	}
};


#endif // RPAL_CSEENVIRONMENT_H
