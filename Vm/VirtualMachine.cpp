#include "VirtualMachine.h"
#include <iostream>
#include <iomanip>

VirtualMachine::VirtualMachine(Memory& mem) : memory(mem), pc(0), running(false) {
    for (int i = 0; i < 32; ++i) registers[i] = 0;
    
    // sp (stack pointer) սկզբնավորում հիշողության վերջից (x2 ռեգիստրը)
    registers[2] = memory.getSize(); 
}

void VirtualMachine::run(const std::vector<Instruction>& program) {
    running = true;
    pc = 0;

    while (running && pc < program.size()) {
        const Instruction& instr = program[pc];
        executeInstruction(instr);
        
        // x0 ռեգիստրը միշտ պետք է մնա 0
        registers[0] = 0;
        
        pc++; // Անցնում ենք հաջորդ հրահանգին
    }
}

void VirtualMachine::executeInstruction(const Instruction& instr) {
    switch (instr.opcode) {
        case OpCode::ADD:
            registers[instr.rd] = registers[instr.rs1] + registers[instr.rs2];
            break;

        case OpCode::ADDI:
            registers[instr.rd] = registers[instr.rs1] + instr.imm;
            break;

        case OpCode::SUB:
            registers[instr.rd] = registers[instr.rs1] - registers[instr.rs2];
            break;

        case OpCode::LW: {
            uint32_t addr = registers[instr.rs1] + instr.imm;
            registers[instr.rd] = memory.read32(addr);
            break;
        }

        case OpCode::SW: {
            uint32_t addr = registers[instr.rs1] + instr.imm;
            memory.write32(addr, registers[instr.rs2]);
            break;
        }

        case OpCode::BEQ:
            // Եթե rs1 == rs2, թռչել (փոխել PC-ն)
            if (registers[instr.rs1] == registers[instr.rs2]) {
                pc += (instr.imm - 1); // -1 որովհետև վերջում pc++ է լինում
            }
            break;

        case OpCode::BNE:
            if (registers[instr.rs1] != registers[instr.rs2]) {
                pc += (instr.imm - 1);
            }
            break;

        case OpCode::JAL:
            // Պահել վերադարձի հասցեն rd-ում և թռչել
            if (instr.rd != 0) registers[instr.rd] = pc + 1;
            pc += (instr.imm - 1);
            break;

        case OpCode::HALT:
            running = false;
            break;

        default:
            std::cerr << "Unknown OpCode encountered at PC: " << pc << std::endl;
            running = false;
            break;
    }
}

void VirtualMachine::dumpRegisters() const {
    std::cout << "\n--- Register Dump ---" << std::endl;
    for (int i = 0; i < 32; ++i) {
        std::cout << std::setw(4) << RegNames[i] << ": " 
                  << std::setw(10) << registers[i] << (i % 4 == 3 ? "\n" : " | ");
    }
    std::cout << "---------------------\n" << std::endl;
}