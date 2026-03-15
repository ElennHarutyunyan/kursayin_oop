#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "Lexer.h"
#include "Parser.h"
#include "Evaluator.h"

int main() {
    std::string line;
    std::string input;

    std::cout << "--- OOP Interpreter (Iterative Mode) ---" << std::endl;
    std::cout << "Enter your code (End with Ctrl+D or an empty line):" << std::endl;

    // Read multiple lines of code
    while (std::getline(std::cin, line) && !line.empty()) {
        input += line + "\n";
    }

    if (input.empty()) {
        std::cout << "No input provided. Exiting." << std::endl;
        return 0;
    }

    try {
        // 1. Lexical Analysis: Break input string into tokens
        Lexer lexer(input);

        // 2. Parsing: Build the Abstract Syntax Tree (AST)
        Parser parser(lexer);
        auto tree = parser.parse();

        // 3. Evaluation: 
        // This will 'Flatten' the tree into a vector of Instructions
        // and then run the Fetch-Decode-Execute loop.
        Evaluator eval;
        eval.run(tree);

    } catch (const std::exception& e) {
        // Handle syntax or runtime errors (like division by zero)
        std::cerr << "RUNTIME ERROR: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Execution finished successfully." << std::endl;
    return 0;
}