#include "lexer.hh"

Lexer::Lexer(std::string input):input(input),currentPos(0)
{
}
void Lexer::reset()
{
    currentPos = 0;
}

Token Lexer::peek()
{
    int initPos = currentPos;
    Token nextTok = nextToken();
    this->currentPos = initPos;
    return nextTok;
}

Token Lexer::nextToken()
{
    while (currentPos < input.length())
    {
        if (input[currentPos] == ' ' || input[currentPos] == '\t')
        {
            currentPos++;
            continue;
        } else if (input[currentPos] == '\'' || input[currentPos] == '"') 
        {
            char quote = input[currentPos];
            std::string text = "";
            currentPos++;
            while (input[currentPos] != quote && currentPos < input.length())
            {
                if (input[currentPos] == '\\')
                {
                    switch (input[currentPos+1])
                    {
                    case 'n':
                        text += "\n";
                        break;
                    case 't':
                        text += "\t";
                        break;
                    case 'r':
                        text += "\r";
                        break;                    
                    default:
                        text += "\\" + input[currentPos+1];
                        break;
                    }
                    currentPos += 2;
                } else {
                    text += input[currentPos];
                    currentPos++;
                }
            }
            currentPos++;
            return Token("string", text);
        } else if (std::isdigit(input[currentPos]))
        {
            std::string text = "";
            bool dot = false;
            while (std::isdigit(input[currentPos]) || input[currentPos] == '.')
            {
                if (input[currentPos] == '.'){
                    if (!dot) {
                        dot = true;
                    } else {
                        break;
                    }
                }
                text += input[currentPos];
                currentPos++;
            }
            if (dot) {
                return Token("float", text);
            }
            return Token("integer", text);
        } else if (std::isalpha(input[currentPos]) || input[currentPos] == '_')
        {
            std::string text = "";
            while (currentPos < input.length() && (std::isalnum(input[currentPos]) || input[currentPos] == '_'))
            {
                text += input[currentPos];
                currentPos++;
            }

            if (text == "true" || text == "false") {
                return Token("boolean", text);
            }

            if (std::find(resevedWords.begin(), resevedWords.end(), text) != resevedWords.end()) {
                return Token(text, text);
            }
            return Token("identifier", text);
        } else
        {
           
            std::string lookahead = "";
            lookahead = input[currentPos];
            lookahead += input[currentPos + 1];
            auto tpe = symbols.find(lookahead);
            if (tpe != symbols.end())
            {
                currentPos++;
                currentPos++;
                return Token(tpe->second, lookahead);
            }
            lookahead = input[currentPos];
            tpe = symbols.find(lookahead);
            if (tpe != symbols.end())
            {
                currentPos++;
                return Token(tpe->second, lookahead);
            }
            return Token("error", "error at position " + std::to_string(currentPos));
        }
    }

    return Token("EOF", "EOF");
    
}

void showToks(Lexer* lexer)
{
    Token tok;
    for (tok = lexer->nextToken(); tok.tpe != "EOF"; tok = lexer->nextToken())
    {
        std::cout << tok.to_string() << std::endl;
        if (tok.tpe == "error") {
            exit(1);
        }
    }
    std::cout << tok.to_string() << std::endl;
    lexer->reset();
}
