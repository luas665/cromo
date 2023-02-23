#include "parser.hh"

std::string getVarCType(Node *node)
{

}

std::string ndToC(Node* node)
{
    std::string type = node->type;
    if (type == "BinOp")
    {
        BinOp *bop = (BinOp *)node;
        return ndToC(bop->left) + bop->op + ndToC(bop->right);
    }
    else if (type == "Integer")
    {
        Integer *n = (Integer *)node;
        return std::to_string(n->value);
    }
    else if (type == "String")
    {
        String *n = (String *)node;
        return '"' + n->value + '"';
    }
    else if (type == "Float")
    {
        Float *n = (Float *)node;
        return std::to_string(n->value);
    }
    else if (type == "Boolean")
    {
        Boolean *n = (Boolean *)node;
        return std::to_string(n->value);
    }
    else if (type == "Variable")
    {
        Variable *v = (Variable *)node;
        return v->name;
    }
    else if (type == "Assign")
    {
        Assign *a = (Assign *)node;
        return  getVarCType(a->value) + " " + a->name + " = " + ndToC(a->value);
    }
    else if (type == "UnaryOp")
    {
        UnaryOp *u = (UnaryOp *)node;
        return "UnaryOp( op: " + u->op + ", value: " + u->value->toString() + " )";
    }
    else if (type == "IfStatement")
    {
        IfStatement *i = (IfStatement *)node;
        return type + "( " + to_string(i->block) + " )";
    }
    else if (type == "ElseStatement")
    {
        IfStatement *i = (IfStatement *)node;
        return to_string(i);
    }
    else if (type == "CallFunction")
    {
        CallFunction *c = (CallFunction *)node;
        return "CallFunction( name: " + c->func->toString() + ", args: " + to_string(c->args) + " )";
    }
    else if (type == "CreateFunction")
    {
        CreateFunction *c = (CreateFunction *)node;
        return "CreateFunction( name: " + c->name + ", params: " + to_string(c->params) + ", block: " + to_string(c->block) + " )";
    }
    else
    {
        return type + "( not implemented )";
    }
}

