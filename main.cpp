#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string.h>

#include "parser.h"
using namespace std;

int main(int argc, const char **argv) {
    if (argc >= 2) {
        int filePathIndex;
        bool isASTFlagged = false;

        if (argc == 3) {
            // if AST flag is present
            if (strcmp(argv[1], "-ast") == 0) {
                // if -ast flag is present as first argument
                filePathIndex = 2;
                isASTFlagged = true;
            } else if (strcmp(argv[2], "-ast") == 0) {
                // if -ast flag is present as second argument
                filePathIndex = 1;
                isASTFlagged = true;
            } else {
                cout << " Error : Invalid flag" << endl;
                return 1;
            }
        } else if (argc == 2) {
            // if no flag is present
            filePathIndex = 1;
        } else {
            cout << " Error : Incorrect no. of inputs " << endl;
            return 1;
        }

        string filepath = argv[filePathIndex];    // Get the file path

        // Read file
        ifstream input(filepath);
        if (!input.is_open()) {
            // Check if file exists
            std::cout << "File " << "\"" << filepath << "\"" << " not found!" << "\n";
            return 1;
        }

        // Read file contents into a string
        string inputString((istreambuf_iterator<char>(input)), (istreambuf_iterator<char>()));
        input.close();

        // Convert string to a char array
        const int fileLength = inputString.size();
        char characterArray[fileLength];
        for (int i = 0; i < inputString.size(); i++) characterArray[i] = inputString[i];

        // Initialize parser and start parsing
        parser rpal_parser(characterArray, fileLength, isASTFlagged);
        rpal_parser.parse();

        return 0;
    } else if (argc == 1) {
        cout << " Error :No Input Program " << endl;
        return 1;
    } else
        cout << " Error : Incorrect no. of inputs " << endl;
        return 1;
    return 1;
}
