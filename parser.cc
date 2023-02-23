#include "parser.hh"

bool stob(const std::string &__str)
{
    if (!__str.empty())
    {
        if (__str == "true")
        {
            return true;
        }
        else if (__str == "false")
        {
            return false;
        }
    }
    throw std::runtime_error("stob function gets a invalid string");
}

std::string to_string(bool __bool)
{
    if (__bool)
    {
        return "true";
    }
    return "false";
}

Parser::Parser(Lexer *lexer) : lexer_(lexer)
{
    currentToken_ = lexer_->nextToken();
}

inline void Parser::error(std::string message)
{
    std::cerr << "Error: " << message << std::endl;
    throw std::runtime_error(message);
}

inline void Parser::newlineVerification()
{
    while (currentToken_.tpe == "newline")
    {
        eat();
    }
}

inline void Parser::eat()
{
    currentToken_ = lexer_->nextToken();
}

inline void Parser::eat(std::string type)
{
    if (currentToken_.tpe == type)
    {
        eat();
    }
    else
    {
        error("Expect error: expected " + type + " got " + currentToken_.tpe + ".");
    }
}
inline Token Parser::peek()
{
    return lexer_->peek();
}

Node *Parser::factor()
{
    Token tok = currentToken_;
    std::string text = currentToken_.text;
    if (tok.tpe == "minus")
    {
        eat();
        text = '-' + currentToken_.text;
        tok = currentToken_;
    }
    if (tok.tpe == "integer")
    {
        eat();
        return new Node("Integer", new Integer(std::stoi(text)));
    }
    else if (tok.tpe == "float")
    {
        eat();
        return new Node("Float", new Float(std::stod(text)));
    }
    else if (tok.tpe == "string")
    {
        eat();
        return verifyIndex(new Node("String", new String(tok.text)));
    }
    else if (tok.tpe == "boolean")
    {
        eat();
        return new Node("Boolean", new Boolean(stob(tok.text)));
    }
    else if (tok.tpe == "lbracket")
    {
        return parseList();
    }
    else if (tok.tpe == "identifier")
    {
        eat();
        return verifyIndex(new Node("Variable", new Variable(tok.text)));
    }
    else if (tok.tpe == "lparen")
    {
        eat();
        Node *expr = expression();
        eat("rparen");
        return expr;
    }
    else if (tok.tpe == "not")
    {
        eat();
        return new Node("UnaryOp", new UnaryOp("not", expression()));
    }
    else
    {
        if (tok.tpe == "error") {
            std::cerr << tok.text << std::endl;
        }
        error("Unexpected token: token " + tok.tpe);
        return nullptr;
    }
}

Node *Parser::verifyIndex(Node *node)
{
    if (currentToken_.tpe == "lbracket")
    {
        eat();
        Node *index = expression();
        eat("rbracket");
        if (currentToken_.tpe == "lbracket" || currentToken_.tpe == "lparen")
        {
            return verifyIndex(new Node("GetIndex", new GetIndex(index, node)));
        }
        return new Node("GetIndex", new GetIndex(index, node));
    }

    if (currentToken_.tpe == "lparen")
    {
        StatementBlock args;
        eat("lparen");
        if (currentToken_.tpe != "rparen")
        {
            args.addNode(expression());
            while (currentToken_.tpe == "comma")
            {
                eat("comma");
                args.addNode(expression());
            }
        }

        eat("rparen");

        if (currentToken_.tpe == "lparen" || currentToken_.tpe == "lbracket")
        {
            return verifyIndex(new Node("CallFunction", new CallFunction(node, args)));
        }
        return new Node("CallFunction", new CallFunction(node, args));
    }

    return node;
}

Node *Parser::parseList()
{
    StatementBlock list;
    eat("lbracket");
    newlineVerification();
    if (currentToken_.tpe == "rbracket") {
        eat();
        return new Node("List", new List(list));
    }
    list.addNode(expression());
    while (currentToken_.tpe != "rbracket" && currentToken_.tpe != "EOF")
    {
        newlineVerification();
        eat("comma");
        newlineVerification();
        list.addNode(expression());
        newlineVerification();
    }
    if (currentToken_.tpe == "comma") {
        eat();
    }
    newlineVerification();
    eat("rbracket");
    newlineVerification();

    return new Node("List", new List(list));
}

Node *Parser::parsePop()
{
    Node *node = factor();
    while (currentToken_.tpe == "pot")
    {
        std::string op = currentToken_.text;
        eat();
        node = new Node("BinOp", new BinOp(node, factor(), op));
    }
    return node;
}

Node *Parser::parseMDop()
{
    Node *node = parsePop();
    while (currentToken_.tpe == "multiply" || currentToken_.tpe == "division")
    {
        std::string op = currentToken_.text;
        eat();
        node = new Node("BinOp", new BinOp(node, parsePop(), op));
    }
    return node;
}

Node *Parser::parsePMop()
{
    Node *node = parseMDop();
    while (currentToken_.tpe == "plus" || currentToken_.tpe == "minus")
    {
        std::string op = currentToken_.text;
        eat();
        node = new Node("BinOp", new BinOp(node, parseMDop(), op));
    }
    return node;
}

Node *Parser::parseGLENop()
{
    Node *node = parsePMop();
    while (currentToken_.tpe == "greater" || currentToken_.tpe == "less" || currentToken_.tpe == "greaterequal" || currentToken_.tpe == "lessequal" || currentToken_.tpe == "equal" || currentToken_.tpe == "notequal")
    {
        std::string op = currentToken_.text;
        eat();
        node = new Node("BinOp", new BinOp(node, parsePMop(), op));
    }
    return node;
}

Node *Parser::parseAOop()
{
    Node *node = parseGLENop();
    while (currentToken_.tpe == "and" || currentToken_.tpe == "or")
    {
        std::string op = currentToken_.text;
        eat();
        node = new Node("BinOp", new BinOp(node, parseGLENop(), op));
    }
    return node;
}

Node *Parser::expression()
{
    if (currentToken_.tpe == "not")
    {
        return new Node("UnaryOp", new UnaryOp("not", parseAOop()));
    }

    return parseAOop();
}

Node* Parser::whileStatement()
{
    eat("while");
    Node *condition = expression();
    StatementBlock b;
    eat("newline");
    b.addNode(nextNode());
    newlineVerification();
    while (currentToken_.tpe != "end" && currentToken_.tpe != "EOF") {
        b.addNode(nextNode());
        newlineVerification();

    }

    newlineVerification();
    eat("end");

    return new Node("WhileStatement", new WhileStatement(condition, b));
}

Node* Parser::functionDeclaration()
{
    eat("func");
    std::string funcName = currentToken_.text;
    StatementBlock params;
    StatementBlock block;
    eat("identifier");
    eat("lparen");

    if (currentToken_.tpe != "rparen") {
        params.addNode(new Node("Variable", new Variable(currentToken_.text)));
        eat("identifier");
    }

    while (currentToken_.tpe != "rparen" && currentToken_.tpe != "EOF") {
        eat("comma");
        params.addNode(new Node("Variable", new Variable(currentToken_.text)));
        eat("identifier");
    }
    eat("rparen");

    newlineVerification();
    for (; currentToken_.tpe != "end" && currentToken_.tpe != "EOF";)
    {
        block.addNode(nextNode());
        newlineVerification();
    }

    eat("end");

    block.addNode(new Node("EOF", nullptr));

    return new Node("CreateFunction", new CreateFunction(funcName, params, block));
}

Node* Parser::nextNode()
{
    if (currentToken_.tpe == "newline")
    {
        eat();
        return nextNode();
    }
    else if (currentToken_.tpe == "if")
    {
        return ifStatement();
    }
    else if (currentToken_.tpe == "while")
    {
        return whileStatement();
    }
    else if (currentToken_.tpe == "do")
    {
        return doStatement();
    }
    else if (currentToken_.tpe == "identifier")
    {
        return identifier();
    }
    else if (currentToken_.tpe == "func")
    {
        return functionDeclaration();
    }
    else if (currentToken_.tpe == "return")
    {
        eat();
        return new Node("UnaryOp", new UnaryOp("return", expression()));
    }
    else if (currentToken_.tpe == "import")
    {
        eat();
        return new Node("UnaryOp", new UnaryOp("import", expression()));
    }
    else if (currentToken_.tpe == "EOF")
    {
        return new Node("EOF", nullptr);
    }
    else
    {
        return expression();
    }
}

Node *Parser::identifier()
{
    std::string name = currentToken_.text;
    Token nxtTok = peek();
    if (nxtTok.tpe == "assign")
    {
        eat();
        eat();
        return new Node("Assign", new Assign(name, nextNode()));
    }

    return expression();
}

Node *Parser::elseStatement()
{
    StatementBlock block;
    newlineVerification();
    for (; currentToken_.tpe != "end" && currentToken_.tpe != "EOF";)
    {
        block.addNode(nextNode());
        newlineVerification();
    }
    eat("end");
    return new Node("ElseStatement", new IfStatement(nullptr, block, nullptr));
}

Node *Parser::ifStatement()
{
    eat("if");
    Node *condition = expression();
    newlineVerification();
    Node *elseNode = nullptr;
    StatementBlock block;
    for (; currentToken_.tpe != "else" && currentToken_.tpe != "end" && currentToken_.tpe != "EOF";)
    {
        block.addNode(nextNode());
        newlineVerification();
    }
    if (currentToken_.tpe == "else")
    {
        eat();
        if (currentToken_.tpe == "if")
        {
            elseNode = ifStatement();
        }
        else
        {
            elseNode = elseStatement();
        }
        return new Node("IfStatement", new IfStatement(condition, block, elseNode));
    }

    eat("end");
    return new Node("IfStatement", new IfStatement(condition, block, elseNode));
}

Node *Parser::doStatement()
{
}

std::string to_string(StatementBlock statement)
{
    std::string result = "StatementBlock([ ";
    result += statement.statement[0]->toString();
    for (int i = 1; i < statement.statement.size(); i++)
    {
        Node *node = statement.statement[i];
        result += ", " + node->toString();
    }
    return result + " ])";
}

std::string to_string(IfStatement *i)
{
    if (i->condition == nullptr)
    {
        return "ElseNode( " + to_string(i->block) + " )";
    }
    std::string result = "IfStatement( condition: " + i->condition->toString() + ", block: " + to_string(i->block);
    if (i->elseNode != nullptr)
    {
        return result + ", elseNode: " + i->elseNode->toString() + " )";
    }
    return result + ", NOELSENODE )";
}

std::string Node::toString()
{
    if (type == "BinOp")
    {
        BinOp *bop = (BinOp *)node;
        return "BinOp( left: " + bop->left->toString() + " op:" + bop->op + " right: " + bop->right->toString() + " )";
    }
    else if (type == "Integer")
    {
        Integer *n = (Integer *)node;
        return "Integer( " + std::to_string(n->value) + " )";
    }
    else if (type == "String")
    {
        String *n = (String *)node;
        return "String( \'" + n->value + "\' )";
    }
    else if (type == "Float")
    {
        Float *n = (Float *)node;
        return "Float( " + std::to_string(n->value) + " )";
    }
    else if (type == "Boolean")
    {
        Boolean *n = (Boolean *)node;
        return "Boolean( " + to_string(n->value) + " )";
    }
    else if (type == "Variable")
    {
        Variable *v = (Variable *)node;
        return "Variable( " + v->name + " )";
    }
    else if (type == "Assign")
    {
        Assign *a = (Assign *)node;
        return "Assign( name: " + a->name + ", value: " + a->value->toString() + " )";
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

void showNodes(Parser *parser)
{
    for (Node *node = parser->nextNode(); node->type != "EOF"; node = parser->nextNode())
    {
        std::cout << node->toString() << std::endl;
    }
}
