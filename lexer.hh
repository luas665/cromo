#pragma once
#include "token.hh"
#include <map>
#include <iostream>
#include <vector>


const std::map<std::string, std::string> symbols = {
    {"+", "plus"},
    {"-", "minus"},
    {"*", "multiply"},
    {"/", "division"},
    {"(", "lparen"},
    {")", "rparen"},
    {"{", "lbrace"},
    {"}", "rbrace"},
    {"[", "lbracket"},
    {"]", "rbracket"},
    {"\n", "newline"},
    {",", "comma"},
    {".", "dot"},
    {"=", "assign"},
    {">", "greater"},
    {"<", "less"},
    {">=", "greaterequal"},
    {"<=", "lessequal"},
    {"==", "equal"},
    {"!=", "notequal"},
    {"**", "pot"},
};

const std::vector<std::string> resevedWords = {
    "if",
    "else",
    "do",
    "return",
    "func",
    "end",
    "and",
    "or",
    "not",
    "while",
    "import",
};

class Lexer
{
private:
    std::string input;
    int currentPos;
public:
    Lexer(std::string input);
    Token nextToken();
    Token peek();
    void reset();
};

void showToks(Lexer* lexer);
