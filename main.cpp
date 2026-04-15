#include "Lexer.h"
#include "Parser.h"
#include "Evaluator.h"
#include <iostream>
#include <string>

int main() {
    Evaluator vm; // <-- MOVE THIS OUTSIDE THE LOOP
    std::string line;

    std::cout << "Elen's Interpreter Console" << std::endl;

    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line) || line == "exit") break;

        try {
            Lexer lexer(line);
            Parser parser(lexer);
            auto tree = parser.parse();
            vm.run(tree); // Now it uses the same memory every time!
        } catch (...) { /* Error handling */ }
    }
    return 0;
}