#ifndef RPAL_GRAMMERRULES_H
#define RPAL_GRAMMERRULES_H
#include "parameters.h"
#include "utils.h"

/* ------------------------------ RPAL Grammar Rules -------------------------------- */

void procedure_E(parameters* params);
void procedure_T(parameters* params);
void procedure_Ta(parameters* params);
void procedure_Tc(parameters* params);
void procedure_B(parameters* params);
void procedure_Bt(parameters* params);
void procedure_Bs(parameters* params);
void procedure_Bp(parameters* params);
void procedure_A(parameters* params);
void procedure_At(parameters* params);
void procedure_Af(parameters* params);
void procedure_Ap(parameters* params);
void procedure_R(parameters* params);
void procedure_Rn(parameters* params);
void procedure_D(parameters* params);
void procedure_Da(parameters* params);
void procedure_Dr(parameters* params);
void procedure_Db(parameters* params);
void procedure_Vb(parameters* params);
void procedure_Vl(parameters* params);
void procedure_Ew(parameters* params);


void procedure_Vl(parameters* params)
/*
Vl -> '<IDENTIFIER>' list ','
*/
{
    int n = 1;
    read(params->nextToken.getValue(), "ID", params);

    while (params->nextToken.getValue() == ",")
    {
        read(",", "PUNCTION", params);
        read(params->nextToken.getValue(), "ID", params);
        n++;
    }
    if (n > 1)
    {
        buildTree(",", "KEYWORD", n, params);
    }
}


void procedure_Vb(parameters* params)
/*
Vb -> '<IDENTIFIER>'
   -> '(' Vl ')'
   -> '(' ')'
*/
{
    if (params->nextToken.getType() == "ID")
    {
        read(params->nextToken.getValue(), "ID", params);
    }
    else if (params->nextToken.getValue() == "(")
    {
        read("(", "PUNCTION", params);
        if (params->nextToken.getValue() == ")")
        {
            read(")", "PUNCTION", params);
            buildTree("()", "KEYWORD", 0, params);
        }
        else
        {
            procedure_Vl(params);
            read(")", "PUNCTION", params);
        }
    }
}


void procedure_Db(parameters* params)
/*
Db -> Vl '=' E
   -> '<IDENTIFIER>' Vb+ '=' E
   -> '(' D ')'
*/
{
    if (params->nextToken.getValue() == "(")
    {
        read("(", "PUNCTION", params);
        procedure_D(params);
        read(")", "PUNCTION", params);
    }
    else if (params->nextToken.getType() == "ID")
    {
        read(params->nextToken.getValue(), "ID", params);
        int n = 1;
        if (params->nextToken.getValue() == "=" || params->nextToken.getValue() == ",")
        {
            while (params->nextToken.getValue() == ",")
            {
                read(",", "PUNCTION", params);
                read(params->nextToken.getValue(), "ID", params);
                n++;
            }
            if (n > 1)
            {
                buildTree(",", "KEYWORD", n, params);
            }
            read("=", "OPERATOR", params);
            procedure_E(params);
            buildTree("=", "KEYWORD", 2, params);
        }
        else
        {
            do
            {
                procedure_Vb(params);
                n++;
            } while (params->nextToken.getType() == "ID" || params->nextToken.getValue() == "(");
            read("=", "OPERATOR", params);
            procedure_E(params);
            buildTree("function_form", "KEYWORD", n + 1, params);
        }
    }
}


void procedure_Dr(parameters* params)
/*
Dr -> 'rec' Db
   -> Db ;
*/
{
    if (params->nextToken.getValue() == "rec")
    {
        read("rec", "KEYWORD", params);
        procedure_Db(params);
        buildTree("rec", "KEYWORD", 1, params);
    }
    else
    {
        procedure_Db(params);
    }
}


void procedure_Da(parameters* params)
/*
Da -> Dr ( 'and' Dr )+
   -> Dr
*/
{
    procedure_Dr(params);

    int n = 1;
    while (params->nextToken.getValue() == "and")
    {
        n++;
        read("and", "KEYWORD", params);
        procedure_Dr(params);
    }
    if (n > 1)
    {
        buildTree("and", "KEYWORD", n, params);
    }
}


void procedure_D(parameters* params)
/*
D -> Da 'within' D
  -> Da
*/
{
    procedure_Da(params);
    if (params->nextToken.getValue() == "within")
    {
        read("within", "KEYWORD", params);
        procedure_D(params);
        buildTree("within", "KEYWORD", 2, params);
    }
}


void procedure_Rn(parameters* params)
/*
Rn -> '<IDENTIFIER>'
   -> '<INTEGER>'
   -> '<STRING>'
   -> 'true'
   -> 'false'
   -> 'nil'
   -> '(' E ')'
   -> 'dummy'
*/
{
    if (params->nextToken.getType() == "ID" || params->nextToken.getType() == "INT" || params->nextToken.getType() == "STR")
    {
        read(params->nextToken.getValue(), params->nextToken.getType(), params);
    }
    else if (params->nextToken.getValue() == "true")
    {
        read("true", "KEYWORD", params);
        buildTree("true", "BOOL", 0, params);
    }
    else if (params->nextToken.getValue() == "false")
    {
        read("false", "KEYWORD", params);
        buildTree("false", "BOOL", 0, params);
    }
    else if (params->nextToken.getValue() == "nil")
    {
        read("nil", "KEYWORD", params);
        buildTree("nil", "NIL", 0, params);
    }
    else if (params->nextToken.getValue() == "(")
    {
        read("(", "PUNCTION", params);
        procedure_E(params);
        read(")", "PUNCTION", params);
    }
    else if (params->nextToken.getValue() == "dummy")
    {
        read("dummy", "KEYWORD", params);
        buildTree("dummy", "DUMMY", 0, params);
    }
}


void procedure_R(parameters* params)
/*
R -> RRn
  -> Rn
*/
{
    procedure_Rn(params);
    while (params->nextToken.getType() == "ID" || params->nextToken.getType() == "INT" || params->nextToken.getType() == "STR" || params->nextToken.getValue() == "true" || params->nextToken.getValue() == "false" || params->nextToken.getValue() == "nil" || params->nextToken.getValue() == "(" || params->nextToken.getValue() == "dummy")
    {
        procedure_Rn(params);
        buildTree("gamma", "KEYWORD", 2, params);
    }
}


void procedure_Ap(parameters* params)
/*
Ap -> Ap '@' '<IDENTIFIER>' R
   -> R
*/
{
    procedure_R(params);
    while (params->nextToken.getValue() == "@")
    {
        read("@", "OPERATOR", params);
        if (params->nextToken.getType() != "ID")
        {
            cout << "Exception: UNEXPECTED_TOKEN";
        }
        else
        {
            read(params->nextToken.getValue(), "ID", params);
            procedure_R(params);
            buildTree("@", "KEYWORD", 3, params);
        }
    }
}


void procedure_Af(parameters* params)
/*
Af -> Ap '**' Af
   -> Ap
*/
{
    procedure_Ap(params);
    if (params->nextToken.getValue() == "**")
    {
        read("**", "OPERATOR", params);
        procedure_Af(params);
        buildTree("**", "KEYWORD", 2, params);
    }
}


void procedure_At(parameters* params)
/*
At -> At '*' Af
   -> At '/' Af
   -> Af
*/
{
    procedure_Af(params);

    while (params->nextToken.getValue() == "*" || params->nextToken.getValue() == "/")
    {
        string environmentNode = params->nextToken.getValue();
        read(environmentNode, "OPERATOR", params);
        procedure_Af(params);
        buildTree(environmentNode, "OPERATOR", 2, params);
    }
}


void procedure_A(parameters* params)
/*
A -> A '+' At
  -> A '-' At
  -> '+' At
  -> '-' At
  -> At
*/
{
    if (params->nextToken.getValue() == "+")
    {
        read("+", "OPERATOR", params);
        procedure_At(params);
    }
    else if (params->nextToken.getValue() == "-")
    {
        read("-", "OPERATOR", params);
        procedure_At(params);
        buildTree("neg", "KEYWORD", 1, params);
    }
    else
    {
        procedure_At(params);
    }

    while (params->nextToken.getValue() == "+" || params->nextToken.getValue() == "-")
    {
        string environmentNode = params->nextToken.getValue();
        read(environmentNode, "OPERATOR", params);
        procedure_At(params);
        buildTree(environmentNode, "OPERATOR", 2, params);
    }
}


void procedure_Bp(parameters* params)
/*
Bp -> A ('gr' | '>' ) A
   -> A ('ge' | '>=') A
   -> A ('ls' | '<' ) A
   -> A ('le' | '<=') A
   -> A 'eq' A
   -> A 'ne' A
   -> A
*/
{
    procedure_A(params);
    string environmentNode = params->nextToken.getValue();
    string temp2 = params->nextToken.getType();

    if (params->nextToken.getValue() == "gr" || params->nextToken.getValue() == ">")
    {
        read(environmentNode, temp2, params);
        procedure_A(params);
        buildTree("gr", "KEYWORD", 2, params);
    }
    else if (params->nextToken.getValue() == "ge" || params->nextToken.getValue() == ">=")
    {
        read(environmentNode, temp2, params);
        procedure_A(params);
        buildTree("ge", "KEYWORD", 2, params);
    }
    else if (params->nextToken.getValue() == "ls" || params->nextToken.getValue() == "<")
    {
        read(environmentNode, temp2, params);
        procedure_A(params);
        buildTree("ls", "KEYWORD", 2, params);
    }
    else if (params->nextToken.getValue() == "le" || params->nextToken.getValue() == "<=") // Bp -> A (’le’ | ’<=’)
    {
        read(environmentNode, temp2, params);
        procedure_A(params);
        buildTree("le", "KEYWORD", 2, params);
    }
    else if (params->nextToken.getValue() == "eq") // Bp -> A ’eq’ A
    {
        read(environmentNode, temp2, params);
        procedure_A(params);
        buildTree("eq", "KEYWORD", 2, params);
    }
    else if (params->nextToken.getValue() == "ne") // Bp -> A ’ne’ A
    {
        read(environmentNode, temp2, params);
        procedure_A(params);
        buildTree("ne", "KEYWORD", 2, params);
    }
}


void procedure_Bs(parameters* params)
/*
Bs -> 'not' Bp
   -> Bp ;
*/
{
    if (params->nextToken.getValue() == "not")
    {
        read("not", "KEYWORD", params);
        procedure_Bp(params);
        buildTree("not", "KEYWORD", 1, params);
    }
    else
    {
        procedure_Bp(params);
    }
}


void procedure_Bt(parameters* params)
/*
Bt -> Bt '&' Bs
   -> Bs
*/
{
    procedure_Bs(params);
    while (params->nextToken.getValue() == "&")
    {
        read("&", "OPERATOR", params);
        procedure_Bs(params);
        buildTree("&", "KEYWORD", 2, params);
    }
}


void procedure_B(parameters* params)
/*
B -> B 'or' Bt
  -> Bt
*/
{
    procedure_Bt(params);
    while (params->nextToken.getValue() == "or")
    {
        read("or", "KEYWORD", params);
        procedure_Bt(params);
        buildTree("or", "KEYWORD", 2, params);
    }
}


void procedure_Tc(parameters* params)
/*
Tc -> B '->' Tc '|' Tc
   -> B
*/
{
    procedure_B(params);

    if (params->nextToken.getValue() == "->")
    {
        read("->", "OPERATOR", params);
        procedure_Tc(params);
        read("|", "OPERATOR", params);
        procedure_Tc(params);
        buildTree("->", "KEYWORD", 3, params);
    }
}


void procedure_Ta(parameters* params)
/*
Ta -> Ta 'aug' Tc
   -> Tc
*/
{
    procedure_Tc(params);

    while (params->nextToken.getValue() == "aug")
    {
        read("aug", "KEYWORD", params);
        procedure_Tc(params);
        buildTree("aug", "KEYWORD", 2, params);
    }
}


void procedure_T(parameters* params)
/*
T -> Ta ( ',' Ta )+
  -> Ta
*/
{
    procedure_Ta(params);

    int n = 1;
    while (params->nextToken.getValue() == ",")
    {
        n++;
        read(",", "PUNCTION", params);
        procedure_Ta(params);
    }
    if (n > 1)
    {
        buildTree("tau", "KEYWORD", n, params);
    }
}


void procedure_Ew(parameters* params)
/*
Ew -> T 'where' Dr
   -> T
*/
{
    procedure_T(params);

    if (params->nextToken.getValue() == "where")
    {
        read("where", "KEYWORD", params);
        procedure_Dr(params);
        buildTree("where", "KEYWORD", 2, params);
    }
}


void procedure_E(parameters* params)
/*
E -> 'let' D 'in' E
  -> 'fn' Vb+ '.' E
  -> Ew
*/
{
    if (params->nextToken.getValue() == "let")
    {
        read("let", "KEYWORD", params);
        procedure_D(params);

        read("in", "KEYWORD", params);
        procedure_E(params);

        buildTree("let", "KEYWORD", 2, params);
    }
    else if (params->nextToken.getValue() == "fn")
    {
        int n = 0;
        read("fn", "KEYWORD", params);
        do
        {
            procedure_Vb(params);
            n++;
        } while (params->nextToken.getType() == "ID" || params->nextToken.getValue() == "(");
        read(".", "OPERATOR", params);
        procedure_E(params);

        buildTree("lambda", "KEYWORD", n + 1, params);
    }
    else
    {
        procedure_Ew(params);
    }
}


#endif //RPAL_GRAMMERRULES_H
