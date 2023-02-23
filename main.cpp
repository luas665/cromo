#include "lexer.hh"
#include "parser.hh"
#include "interpreter.hh"

using namespace std;


int main(int argc, char const *argv[])
{
    string text;
    text = "func ask(askPhrase)\ns=input(askPhrase)\nprint('Resposta: '+s)\nend\nask('Digite algo: ')\n";
    text = "func sum(x, y)\nreturn x+y\nend\n";
    if (argc == 2) {
        text = argv[1];
    }
    showNodes(new Parser(new Lexer(text)));
    // Interpreter(text).run();

}
