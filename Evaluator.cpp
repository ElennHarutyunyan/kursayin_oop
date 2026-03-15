#include "Evaluator.h"
#include <iostream>
#include <stdexcept>

void Evaluator::run(std::shared_ptr<StatementsNode> root) {
    if (!root) return;

    // 1. "Compile" the tree into a linear list of instructions
    // This fills our 'program' vector and 'rv' (Result Vector) indices
    root->flatten(program, rv, varMap);

    // 2. Start the iterative execution loop
    execute();
}

void Evaluator::execute() {
    // This is the Fetch-Decode-Execute loop from your notes
    for (size_t pc = 0; pc < program.size(); ++pc) {
        // FETCH
        Line& instr = program[pc];

        // DECODE & EXECUTE
        // We look up values in 'rv' using the indices stored in the instruction
        switch (instr.op) {
            case '+':
                rv[instr.resIdx] = rv[instr.leftIdx] + rv[instr.rightIdx];
                break;
            case '-':
                rv[instr.resIdx] = rv[instr.leftIdx] - rv[instr.rightIdx];
                break;
            case '*':
                rv[instr.resIdx] = rv[instr.leftIdx] * rv[instr.rightIdx];
                break;
            case '/':
                if (rv[instr.rightIdx] == 0) throw std::runtime_error("Division by zero");
                rv[instr.resIdx] = rv[instr.leftIdx] / rv[instr.rightIdx];
                break;
            case '=':
                // Assignment: copy value from right operand index to target index
                rv[instr.resIdx] = rv[instr.leftIdx];
                break;
            case 'P':
                // Print: output the value stored at the leftIdx
                std::cout << rv[instr.leftIdx] << std::endl;
                break;
            default:
                throw std::runtime_error("Unknown operation in instruction stream");
        }
        
        // Optional: Print the state of rv for every step to show the teacher
        // std::cout << "[Step " << pc << "] op: " << instr.op << " result: " << rv[instr.resIdx] << std::endl;
    }
}