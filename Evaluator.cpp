#include "Evaluator.h"
#include "Node.h"
#include "SymbolTable.h"
#include <iostream>

void Evaluator::run(std::shared_ptr<StatementsNode> root) {
    if (!root) return;
    
    program.clear(); // Clear old instructions
    // Use the class-level symbolTable member
    root->flatten(program, symbolTable); 
    
    execute();
}

void Evaluator::execute() {
    int pc = 0;
    std::vector<double> stack; // Stack can be local to execute

    while (pc < (int)program.size()) {
        const auto& instr = program[pc];
        switch (instr.op) {
            case LI:    stack.push_back(instr.val); pc++; break;
            case LOAD:  stack.push_back(memory[instr.address]); pc++; break;
            case STORE: 
                if(!stack.empty()){ 
                    memory[instr.address] = stack.back(); 
                    stack.pop_back(); 
                }
                pc++; break;
            // ... rest of your ADD, SUB, etc. ...
            case JZ: 
                if (!stack.empty()) {
                    double val = stack.back();
                    stack.pop_back();
                    if (val == 0) pc = instr.address;
                    else pc++;
                } else pc++; break;
        }
    }
}