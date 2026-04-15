#ifndef INSTRUCTION_H
#define INSTRUCTION_H

// Ensure these match the cases in your Evaluator.cpp
enum OpCode { LI, LOAD, STORE, ADD, SUB, MUL, DIV, PRINT, JMP, JZ, JNZ };

struct Instruction {
    OpCode op;
    int address;
    double val;

    // The Constructor: This allows prog.push_back({LI, -1, value}) to work!
    Instruction(OpCode o, int addr = -1, double v = 0.0) 
        : op(o), address(addr), val(v) {}
};

#endif