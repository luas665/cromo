#include "interpreter.hh"

Interpreter::Interpreter(Parser* parser) : parserToInterpreter(parser) {}
Interpreter::Interpreter(StatementBlock block) : parserToInterpreter(nullptr), compiledBlock(block) {}
Interpreter::Interpreter(std::string input) : parserToInterpreter(new Parser(new Lexer(input))) {}

void Interpreter::setFilename(std::string filename) {
    this->filename = filename;
}
void Interpreter::setEscope(std::map<std::string, CmObj*> escope) {
    this->escope = escope;
}
std::map<std::string, CmObj*> Interpreter::getEscope() {
    return escope;
}

int cCmInt(CmObj* obj) {
    if (obj->type == "int") {
        return *(int*)obj->data;
    }
    return *(double*)obj->data;
}

int Interpreter::debug() {
    debug_ = true;
    return run();
}

Args Interpreter::getArgs(StatementBlock args) {
    Args result;
    for (Node *node : args.statement) {
        result.push_back(visit(node));
    }

    return result;
}

Node* Interpreter::nextNode() {
    if (parserToInterpreter != nullptr) {
        return parserToInterpreter->nextNode();
    }
    compPos++;
    return compiledBlock.statement[compPos];
}

CmObj* Interpreter::visitAssign(Assign* assign) {
    escope[assign->name] = visit(assign->value);
    return new CmObj();
}

CmObj* Interpreter::visitCallFunction(CallFunction* callFunc) {
    CmObj* result = visit(callFunc->func);
   if (result->type == "bifunc") {
        auto sFunc = builtInFunctions.find(*(std::string*)result->data);
        bifuncs func;
        if (sFunc == builtInFunctions.end()) {
            func = importedFunctions.find(*(std::string*)result->data)->second;
        } else {
            func = sFunc->second;
        }
        return func(getArgs(callFunc->args));
    } else if (result->type == "func") {
        Function* func = (Function*)result->data;
        if (callFunc->args.statement.size() == func->params.statement.size()) {
            std::map<std::string, CmObj*> funcEscope = escope;

            for (int i = 0; i < func->params.statement.size(); i++) {
                std::string name = ((Variable*)(func->params.statement[i]->node))->name;
                CmObj* val = visit(callFunc->args.statement[i]);
                funcEscope[name] = val;
            }

            Interpreter i(func->block);
            i.setEscope(funcEscope);
            i.run();

            CmObj* rtrn;

            auto r = i.getEscope().find("return");
            if(r != i.escope.end()){
                rtrn = r->second;
            }

            return rtrn;
        }
    }

    return new CmObj();
}

CmObj* Interpreter::visitVariable(Variable* variable) {
    auto value = escope.find(variable->name);
    if (value != escope.end()) {
        return value->second;
    }
    auto function = builtInFunctions.find(variable->name);
    if (function != builtInFunctions.end()) {
        CmObj* func = new CmObj("bifunc", new std::string(variable->name));
        return func;
    }
    function = importedFunctions.find(variable->name);
    if (function != importedFunctions.end()) {
        CmObj* func = new CmObj("bifunc", new std::string(variable->name));
        return func;
    }
    throw std::runtime_error("Error: No variable named: " + variable->name);
}

CmObj* Interpreter::visitString(String* string) {
    return new CmObj(string);
}


CmObj* Interpreter::visitBinOp(BinOp* binOp) {
    std::string op = binOp->op;
    CmObj* left = visit(binOp->left);
    CmObj* right = visit(binOp->right);


    if (op == "+" ) {
        return add(Args{left, right});
    } else if (op == "-" ) {
        return min(Args{left, right});
    } else if (op == "*" ) {
        return mul(Args{left, right});
    } else if (op == "/" ) {
        return div(Args{left, right});
    } else if (op == "**" ) {
        return pot(Args{left, right});
    } else if (op == "==" ) {
        return eq(Args{left, right});
    } else if (op == "!=" ) {
        return neq(Args{left, right});
    } else if (op == ">" ) {
        return gt(Args{left, right});
    } else if (op == ">=" ) {
        return gteq(Args{left, right});
    } else if (op == "<" ) {
        return lt(Args{left, right});
    } else if (op == "<=" ) {
        return lteq(Args{left, right});
    } else if (op == "and" ) {
        return andop(Args{left, right});
    } else if (op == "or" ) {
        return orop(Args{left, right});
    }
    
    return new CmObj();
}

CmObj* Interpreter::visitInteger(Integer* integer) {
    return new CmObj(integer);
}

CmObj* Interpreter::visitFloat(Float* f) {
    return new CmObj(f);
}

CmObj* Interpreter::visitBool(Boolean* f) {
    return new CmObj(f);
}

CmObj* Interpreter::visitList(List* list) {
    Args nlist;
    for (Node* node : list->elements.statement) {
        nlist.push_back(visit(node));
    }

    return new CmObj(nlist);
}

CmObj* Interpreter::visitGetIndex(GetIndex* getIndex) {
    CmObj* var = visit(getIndex->var);
    CmObj* index = visit(getIndex->index);

    if(index->type == "int" || index->type == "float") {
        if (var->type == "str") {
            int i = cCmInt(index);
            int size = (*(std::string*)var->data).length();
            if (i < 0) {
                i = size+i;
            }
            if (i < size && i>= 0) {
                return new CmObj(String((*(std::string*)var->data)[i]));
            }
        } else if (var->type == "list") {
            int i = cCmInt(index);
            int size = (*(Args*)var->data).size();
            if (i < 0) {
                i = size+i;
            }
            if (i < size && i>= 0) {
                return (*(Args*)var->data)[i];
            }
        }
    }
    return new CmObj(std::string("Error: Invalid index."));
}

void Interpreter::addCFunction(std::string name, bifuncs func) {
    importedFunctions[name] = func;
}

CmObj* Interpreter::runBlock(StatementBlock block) {
    CmObj* r;
    for (Node* node : block.statement) {
        r = visit(node);
        if(r->type == "return") {
            return new CmObj("return", nullptr);
        }
    }
    return new CmObj();
}

CmObj* Interpreter::visitIfStatement(IfStatement* ifStatement) {
    bool contdition = *(bool*)to_bool(visit(ifStatement->condition))->data;
    if (contdition) { 
        CmObj* r = runBlock(ifStatement->block);
        if(r->type == "return") {
            return new CmObj("return", nullptr);
        }
    } else {

        if (ifStatement->elseNode == nullptr) {
            return new CmObj();
        }

        return visit(ifStatement->elseNode);
    }

    return new CmObj();
}

CmObj* Interpreter::visitElseStatement(IfStatement* elseStatement) {
    CmObj* r = runBlock(elseStatement->block);
    if(r->type == "return") {
        return new CmObj("return", nullptr);
    }
    return new CmObj();
}

std::map<std::string, CmObj*> getEscopeByFileName(std::string filename) {
    int s = filename.length();
    std::string endsWith = std::string(1, filename[s-3]) + filename[s-2] + filename[s-1];

    

    if (!std::filesystem::is_regular_file(filename)) {
        filename = PACKAGES_PATH + filename;
    }

    if (std::filesystem::is_directory(filename)) {
        filename += "/__module__.co";
    } else if (endsWith != ".co") {
        filename += ".co";
    }

    std::ifstream cmFile;
    std::string line;
    std::string data = "";

    cmFile.open(filename);

    if ( cmFile.is_open() ) {
        while ( cmFile ) {
            std::getline (cmFile, line);
            data += line + '\n';
        }
        data += "\n\0";
    } else {
        std::cerr << "Error: No module named " + filename << std::endl;
        exit(1);
    }

    Interpreter i(data);
    i.run();

    return i.getEscope();
}

CmObj* Interpreter::visitUnaryOp(UnaryOp* unOp) {
    if (unOp->op == "not") {
        Node* val = ((UnaryOp*)(unOp->value->node))->value;
        bool value = (*(bool*)to_bool(visit(val))->data);
        return new CmObj(Boolean( !value ));
    } else if (unOp->op == "return") {
        escope["return"] = visit(unOp->value);
        return new CmObj("return", nullptr);
    } else if (unOp->op == "import") {
        std::string fn;
        if (unOp->value->type == "Variable") {
            fn = ((Variable*)(unOp->value->node))->name;
        } else if (unOp->value->type == "String") {
            fn = ((String*)(unOp->value->node))->value;
        } else {
            return new CmObj("Error: Invalid import expression");
        }

        std::map<std::string, CmObj*>::iterator it;
        std::map<std::string, CmObj*> addEscope = getEscopeByFileName(fn);
        for (it = addEscope.begin(); it != addEscope.end(); it++) {
            escope[it->first] = it->second;
        }
    }
    return new CmObj();
}

CmObj* Interpreter::visitWhileStatement(WhileStatement* whileStatement) {
    bool contdition = *(bool*)to_bool(visit(whileStatement->condition))->data;

    while(contdition) {
        CmObj* r = runBlock(whileStatement->block);
        if(r->type == "return") {
            return new CmObj("return", nullptr);
        }
        contdition = *(bool*)to_bool(visit(whileStatement->condition))->data;
    }
    return new CmObj();
}

CmObj* Interpreter::visitCreateFunction(CreateFunction* createFunction) {
    escope[createFunction->name] = new CmObj(Function(createFunction->params, createFunction->block));
    return new CmObj();
}

CmObj* Interpreter::visit(Node* node) {
    // std::cout << "visit: " << node->toString() << std::endl;
    if (node->type == "BinOp") {
        BinOp* bop = (BinOp*)node->node;
        return visitBinOp(bop);
    } else if (node->type == "UnaryOp") {
        UnaryOp* uop = (UnaryOp*)node->node;
        return visitUnaryOp(uop);
    } else if (node->type == "Integer") {
        Integer* n = (Integer*)node->node;
        return visitInteger(n);
    } else if (node->type == "String") {
        String* n = (String*)node->node;
        return visitString(n);
    } else if (node->type == "Float") {
        Float* n = (Float*)node->node;
        return visitFloat(n);
    } else if (node->type == "Boolean") {
        Boolean* n = (Boolean*)node->node;
        return visitBool(n);
    } else if (node->type == "Variable") {
        Variable* v = (Variable*)node->node;
        return visitVariable(v);
    } else if (node->type == "Assign") {
        Assign* a = (Assign*)node->node;
        return visitAssign(a);
    } else if (node->type == "CallFunction") {
        CallFunction* c = (CallFunction*)node->node;
        return visitCallFunction(c);
    } else if (node->type == "List") {
        List* l = (List*)node->node;
        return visitList(l);
    } else if (node->type == "GetIndex") {
        GetIndex* getIndex = (GetIndex*)node->node;
        return visitGetIndex(getIndex);
    } else if (node->type == "IfStatement") {
        IfStatement* ifStatement = (IfStatement*)node->node;
        return visitIfStatement(ifStatement);
    } else if (node->type == "ElseStatement") {
        IfStatement* elseStatement = (IfStatement*)node->node;
        return visitElseStatement(elseStatement);
    } else if (node->type == "WhileStatement") {
        WhileStatement* ws = (WhileStatement*)node->node;
        return visitWhileStatement(ws);
    } else if (node->type == "CreateFunction") {
        CreateFunction* cf = (CreateFunction*)node->node;
        return visitCreateFunction(cf);
    }
    return new CmObj();
}


int Interpreter::run() {
    CmObj* result;
    for (Node* node = nextNode(); node->type != "EOF"; node = nextNode()) {
        result = visit(node);
        if (result->type == "return") {
            return 0;
        } else if (result->type != "None" && debug_) {
            print(Args{result});
        }
    }

    return 0;
}
