#pragma once
#include <string>
#include <vector>

struct Node {
    std::string type;
    void *node;
    Node(std::string type, void *node) : type(type), node(node) {}
    std::string toString();
};

struct StatementBlock {
    std::vector<Node*> statement;

    void addNode(Node *node) {
        statement.push_back(node);
    }
};

struct Function {
    StatementBlock params;
    StatementBlock block;
    Function(StatementBlock params, StatementBlock block) : params(params), block(block) {}
};

struct Integer
{
    int value;
    Integer(int value) : value(value) {}
};

struct CreateFunction
{
    std::string name;
    StatementBlock params;
    StatementBlock block;
    CreateFunction(std::string name, StatementBlock params, StatementBlock block):name(name), params(params), block(block) {}
};

struct Float
{
    double value;
    Float(double value) : value(value) {}
};

struct List
{
    StatementBlock elements;
    List(StatementBlock elements) : elements(elements) {}
};

struct String
{
    std::string value;
    String(std::string value) : value(value) {}
    String(char value) : value(std::string(1, value)) {}
};

struct Boolean
{
    bool value;
    Boolean(bool value) : value(value) {}
};

struct Variable
{
    std::string name;
    Variable(std::string name) : name(name) {}
};

struct BinOp {
    Node *left;
    Node *right;
    std::string op;
    BinOp(Node *left, Node *right, std::string op) : left(left), right(right), op(op) {}
};

struct UnaryOp {
    Node *value;
    std::string op;
    UnaryOp(std::string op, Node *value) : value(value), op(op) {}
};

struct Assign {
    std::string name;
    Node *value;
    Assign(std::string name, Node *value) : value(value), name(name) {}
};

struct GetIndex {
    Node* index;
    Node *var;
    GetIndex(Node* index, Node* var) : index(index), var(var) {}
};

struct DoStatement {
    int start;
    int end;
    int count;
    StatementBlock block;

    DoStatement(int start, int end, int count, StatementBlock block) : start(start), end(end), count(count), block(block) {}
};

struct IfStatement {
    Node *condition;
    StatementBlock block;
    Node *elseNode;
    IfStatement(StatementBlock block) : condition(nullptr), elseNode(nullptr), block(block) {}
    IfStatement(Node *expr, StatementBlock block, Node* elseNode) : condition(expr), elseNode(elseNode), block(block) {}

    void addElse(Node* elseNode) {
        this->elseNode = elseNode;
    }
};

struct CallFunction {
    Node* func;
    StatementBlock args;
    CallFunction(Node* func, StatementBlock args) : func(func), args(args) {}
};

struct WhileStatement {
    Node* condition;
    StatementBlock block;
    WhileStatement(Node* condition, StatementBlock block): condition(condition), block(block) {}
};
