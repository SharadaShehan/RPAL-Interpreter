cmake_minimum_required(VERSION 3.25)
project(RPAL)

set(CMAKE_CXX_STANDARD 17)

add_executable(RPAL main.cpp token.h constants.h scanner.h typeDef.h tree.h grammerRules.h utils.h parameters.h standardizeTree.h cseMachine.h cseEnvironment.h parser.h)
