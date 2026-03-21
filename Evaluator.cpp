#include "Evaluator.h"
#include <iostream>
#include <stdexcept>

void Evaluator::run(std::shared_ptr<StatementsNode> root) {
    if (!root) return;

    // 1. Flatten the AST into a list of instructions
    // This fills 'program', 'rv', and 'varMap'
    root->flatten(program, rv, varMap);

    // 2. Execute the instruction list
    execute();
}

void Evaluator::execute() {
    // Accumulator to simulate CPU execution
    double acc = 0.0;

    // Fetch-Decode-Execute loop
    for (size_t pc = 0; pc < program.size(); ++pc) {
        Line &instr = program[pc];

        switch (instr.op) {
            case OpCode::LOAD:
                acc = rv[instr.operandIdx];
                break;

            case OpCode::ADD:
                acc += rv[instr.operandIdx];
                break;

            case OpCode::SUB:
                acc -= rv[instr.operandIdx];
                break;

            case OpCode::MUL:
                acc *= rv[instr.operandIdx];
                break;

            case OpCode::DIV:
                if (rv[instr.operandIdx] == 0)
                    throw std::runtime_error("Division by zero");
                acc /= rv[instr.operandIdx];
                break;

            case OpCode::STORE:
                rv[instr.resultIdx] = acc;
                break;

            case OpCode::PRINT:
                std::cout << rv[instr.operandIdx] << std::endl;
                break;

            default:
                throw std::runtime_error("Unknown instruction in program");
        }

        // Optional: Debug output for each step
        // std::cout << "[Step " << pc << "] acc=" << acc << std::endl;
    }
}