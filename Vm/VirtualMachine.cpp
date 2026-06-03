#include "VirtualMachine.h"

#include <functional>
#include <iomanip>
#include <iostream>

namespace {

int32_t maskDatapath(int32_t value) {
    return static_cast<int32_t>(static_cast<uint32_t>(value) & 0xFFFFu);
}

}  // namespace

VirtualMachine::VirtualMachine(Memory& mem)
    : registers{0}, pc(0), stackPointer(0), basePointer(0), memory(mem), running(false) {}

void VirtualMachine::loadProgram(const std::vector<Instruction>& prog) {
    program = prog;
    reset();
}

void VirtualMachine::initializeEnvironment(uint32_t dataBaseAddress, uint32_t initialSp) {
    reset();
    stackPointer = initialSp;
    basePointer = initialSp;
    registers[2] = static_cast<int32_t>(stackPointer);
    registers[8] = static_cast<int32_t>(basePointer);
    registers[3] = static_cast<int32_t>(dataBaseAddress);
}

void VirtualMachine::reset() {
    for (int i = 0; i < 32; ++i) {
        registers[i] = 0;
    }
    pc = 0;
    running = false;
    stackPointer = Memory::kStackBase;
    basePointer = stackPointer;
}

const Instruction& VirtualMachine::currentInstruction() const {
    if (pc >= program.size()) {
        static const Instruction halt{};
        return halt;
    }
    return program[pc];
}

bool VirtualMachine::step() {
    if (!running) {
        running = true;
    }
    if (!running || pc >= program.size()) {
        running = false;
        return false;
    }
    executeInstruction(program[pc]);
    pc++;
    return running;
}

void VirtualMachine::run() {
    runUntilHaltOrBreakpoint([](uint32_t) { return false; });
}

void VirtualMachine::runUntilHaltOrBreakpoint(const std::function<bool(uint32_t)>& shouldBreak) {
    running = true;
    while (running && pc < program.size()) {
        if (shouldBreak(pc)) {
            break;
        }
        executeInstruction(program[pc]);
        pc++;
    }
}

void VirtualMachine::executeInstruction(const Instruction& instr) {
    switch (instr.opcode) {
        case OpCode::ADD:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] + registers[instr.rs2]);
            break;
        case OpCode::ADDI:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] + instr.imm);
            break;
        case OpCode::SUB:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] - registers[instr.rs2]);
            break;
        case OpCode::MUL:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] * registers[instr.rs2]);
            break;
        case OpCode::DIV:
            registers[instr.rd] = maskDatapath((registers[instr.rs2] == 0) ? 0
                                                                               : (registers[instr.rs1] / registers[instr.rs2]));
            break;
        case OpCode::MOD:
            registers[instr.rd] = maskDatapath((registers[instr.rs2] == 0) ? 0
                                                                               : (registers[instr.rs1] % registers[instr.rs2]));
            break;
        case OpCode::AND:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] & registers[instr.rs2]);
            break;
        case OpCode::OR:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] | registers[instr.rs2]);
            break;
        case OpCode::XOR:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] ^ registers[instr.rs2]);
            break;
        case OpCode::SLL:
            registers[instr.rd] = maskDatapath(registers[instr.rs1] << (registers[instr.rs2] & 0xF));
            break;
        case OpCode::SRL:
            registers[instr.rd] = maskDatapath(static_cast<int32_t>(static_cast<uint32_t>(registers[instr.rs1]) >>
                                                                 (registers[instr.rs2] & 0xF)));
            break;
        case OpCode::LW:
            registers[instr.rd] = maskDatapath(static_cast<int32_t>(memory.read32(registers[instr.rs1] + instr.imm)));
            break;
        case OpCode::SW:
            memory.write32(registers[instr.rs1] + instr.imm, static_cast<uint32_t>(registers[instr.rs2]));
            break;
        case OpCode::BEQ:
            if (registers[instr.rs1] == registers[instr.rs2]) {
                pc = static_cast<uint32_t>(static_cast<int32_t>(pc) + instr.imm - 1);
            }
            break;
        case OpCode::BNE:
            if (registers[instr.rs1] != registers[instr.rs2]) {
                pc = static_cast<uint32_t>(static_cast<int32_t>(pc) + instr.imm - 1);
            }
            break;
        case OpCode::BLT:
            if (registers[instr.rs1] < registers[instr.rs2]) {
                pc = static_cast<uint32_t>(static_cast<int32_t>(pc) + instr.imm - 1);
            }
            break;
        case OpCode::BGE:
            if (registers[instr.rs1] >= registers[instr.rs2]) {
                pc = static_cast<uint32_t>(static_cast<int32_t>(pc) + instr.imm - 1);
            }
            break;
        case OpCode::JAL:
            registers[instr.rd] = static_cast<int32_t>(pc + 1);
            pc = static_cast<uint32_t>(instr.imm - 1);
            break;
        case OpCode::JALR:
            registers[instr.rd] = static_cast<int32_t>(pc + 1);
            pc = static_cast<uint32_t>(registers[instr.rs1] + instr.imm - 1);
            break;
        case OpCode::HALT:
            running = false;
            break;
        case OpCode::PRINT:
            std::cout << registers[instr.rs1] << "\n";
            break;
        default:
            running = false;
            break;
    }
    registers[0] = 0;
    registers[2] = static_cast<int32_t>(stackPointer);
    registers[8] = static_cast<int32_t>(basePointer);
}

void VirtualMachine::dumpRegisters() const {
    std::cout << "General-purpose registers:\n";
    for (int i = 0; i < static_cast<int>(arch.registerCount); ++i) {
        std::cout << std::setw(4) << RegNames[i] << "(x" << i << ") = " << registers[i] << "\n";
    }
    std::cout << "IP (byte) = 0x" << std::hex << (pc * 4) << std::dec << "\n";
    std::cout << "SP = 0x" << std::hex << stackPointer << std::dec << "\n";
    std::cout << "BP = 0x" << std::hex << basePointer << std::dec << "\n";
}
