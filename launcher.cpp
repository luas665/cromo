#include "interpreter.hh"
#include <fstream>
#include <string>

int idle() {
    std::map<std::string, CmObj*> escope;
    std::string line;

    while ( true ) {
        std::cout << ">>> ";
        getline(std::cin, line);

        Interpreter interpreter(line);
        interpreter.setEscope(escope);
        interpreter.debug();

        escope = interpreter.getEscope();
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        std::ifstream cmFile;
        std::string line;
        std::string data = "";

        cmFile.open(argv[1]);

        if ( cmFile.is_open() ) {
            while ( cmFile ) {
                std::getline (cmFile, line);
                data += line + '\n';
            }
            data += '\n';
        } else {
            std::cout << "Couldn't open file " << argv[1] << '.' << std::endl;
            return 1;
        }
        // showNodes(new Parser(new Lexer(data)));
        return Interpreter(data).run();

    }

    return idle();
}