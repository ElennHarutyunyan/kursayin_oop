#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <vector>
#include <memory>
#include "Instruction.h"
#include "SymbolTable.h" // Add this!

struct StatementsNode;

class Evaluator {
    private:
        std::vector<Instruction> program;
        std::vector<double> memory;
        SymbolTable symbolTable; // This persists across multiple calls to run()
    public:
        Evaluator() : memory(100, 0.0) {}
        void run(std::shared_ptr<StatementsNode> root);
        void execute();
    };

#endif