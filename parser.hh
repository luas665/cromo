#pragma once
#include "lexer.hh"
#include "node.hh"

class Parser {
private:
    Lexer* lexer_;
    Token currentToken_;
    inline void newlineVerification();
    inline void eat();
    inline void eat(std::string type);
    inline void error(std::string message);
    inline Token peek();
    Node* verifyIndex(Node* node);

    Node* factor();
    Node* parsePop(); // Parses Power operation
    Node* parseMDop(); // Parses multiplication division operations
    Node* parsePMop(); // Parses plus minus operations
    Node* parseGLENop(); // Parses greater less than equal not equal operations
    Node* parseAOop(); // Parses and or operations

    Node* expression();
    Node* identifier();
    Node* parseList();
    Node* ifStatement();
    Node* elseStatement();
    Node* whileStatement();
    Node* doStatement();
    Node* functionDeclaration();
public:
    Parser(Lexer* lexer);
    Node* nextNode();
};

void showNodes(Parser* parser);