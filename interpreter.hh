#pragma once
#include "node.hh"
#include "parser.hh"
#include "cmobj.hh"
#include "builtincmfuncs.hh"
#include <map>
#include <fstream>
#include <filesystem>

#define PACKAGES_PATH "packages/"

class Interpreter
{
private:
    std::string filename;
    StatementBlock compiledBlock;
    Parser* parserToInterpreter;
    bool debug_ = false;
    int compPos = -1;
    std::map<std::string, CmObj*> escope;
    std::map<std::string, bifuncs> importedFunctions;

    Args getArgs(StatementBlock args);

    Node* nextNode();
    CmObj* runBlock(StatementBlock block);
    CmObj* visitAssign(Assign* assign);
    CmObj* visitCallFunction(CallFunction* callFunc);
    CmObj* visitVariable(Variable* variable);
    CmObj* visitString(String* string);
    CmObj* visitList(List* list);
    CmObj* visitGetIndex(GetIndex* getIndex);
    CmObj* visitIfStatement(IfStatement* ifStatement);
    CmObj* visitElseStatement(IfStatement* elseStatement);
    CmObj* visitWhileStatement(WhileStatement* whileStatement);
    CmObj* visitCreateFunction(CreateFunction* createFunction);

    CmObj* visitBinOp(BinOp* binOp);
    CmObj* visitUnaryOp(UnaryOp* unOp);
    CmObj* visitInteger(Integer* integer);
    CmObj* visitFloat(Float* f);
    CmObj* visitBool(Boolean* f);
    CmObj* visit(Node* node);
public:
    void setEscope(std::map<std::string, CmObj*> escope);
    std::map<std::string, CmObj*> getEscope();
    void setFilename(std::string filename);
    void addCFunction(std::string name, bifuncs func);
    
    Interpreter(std::string input);
    Interpreter(Parser* parser);
    Interpreter(StatementBlock block);
    
    int debug();
    int run();
};
