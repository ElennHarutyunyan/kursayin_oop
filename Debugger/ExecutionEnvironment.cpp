#include "ExecutionEnvironment.h"

#include <functional>
#include <iostream>
#include <stdexcept>

namespace debugger {

ExecutionEnvironment::ExecutionEnvironment(size_t memoryBytes)
    : memory(memoryBytes), bus(memory), hdd(8192), dma(memory, hdd), cpu(memory) {}

void ExecutionEnvironment::initialize(const LoadedProgram& program) {
    loadedProgram = program;
    hasProgram = true;
    memory.clear();
    loadDataSection();
    if (loadedProgram.bssSize > 0) {
        memory.reserveBss(loadedProgram.bssSize);
    }
    loadCodeSection();
    cpu.loadProgram(loadedProgram.text);
    cpu.initializeEnvironment(loadedProgram.dataBaseAddress, Memory::kStackBase);
}

void ExecutionEnvironment::reset() {
    if (hasProgram) {
        initialize(loadedProgram);
    }
}

void ExecutionEnvironment::loadDataSection() {
    for (size_t i = 0; i < loadedProgram.data.size(); ++i) {
        bus.write32(loadedProgram.dataBaseAddress + static_cast<uint32_t>(i * sizeof(int32_t)),
                    static_cast<uint32_t>(loadedProgram.data[i]));
    }
}

void ExecutionEnvironment::loadCodeSection() {
    const uint32_t codeBase = memory.getCodeBase();
    for (size_t i = 0; i < loadedProgram.text.size(); ++i) {
        const auto& instr = loadedProgram.text[i];
        const uint32_t address = codeBase + static_cast<uint32_t>(i * sizeof(Instruction));
        memory.write32(address, static_cast<uint32_t>(instr.opcode));
        memory.write32(address + 4, instr.rd | (instr.rs1 << 8) | (instr.rs2 << 16));
        memory.write32(address + 8, static_cast<uint32_t>(instr.imm));
    }
}

bool ExecutionEnvironment::fetchDecodeExecuteStep() {
    return cpu.step();
}

void ExecutionEnvironment::run(ExecutionMode mode, const std::function<bool(uint32_t)>& shouldBreak) {
    if (!hasProgram) {
        throw std::runtime_error("No program loaded");
    }
    auto breakFn = [&](uint32_t ip) {
        if (shouldBreak(ip)) {
            if (mode == ExecutionMode::RunDebug) {
                std::cout << "Breakpoint hit at IP = 0x" << std::hex << ip << std::dec << "\n";
            }
            return true;
        }
        return false;
    };
    if (mode == ExecutionMode::RunSingle) {
        fetchDecodeExecuteStep();
        return;
    }
    cpu.runUntilHaltOrBreakpoint(breakFn);
}

bool ExecutionEnvironment::isCallInstruction(const Instruction& instr) {
    return instr.opcode == OpCode::JAL || instr.opcode == OpCode::JALR;
}

bool ExecutionEnvironment::stepOver(const std::function<bool(uint32_t)>& shouldBreak) {
    if (!hasProgram || cpu.getPc() >= cpu.programSize()) {
        return false;
    }
    if (!isCallInstruction(cpu.currentInstruction())) {
        fetchDecodeExecuteStep();
        return cpu.isRunning();
    }
    const uint32_t returnIp = cpu.getPc() + 1;
    cpu.runUntilHaltOrBreakpoint([&](uint32_t ip) {
        return shouldBreak(ip) || ip >= returnIp;
    });
    return cpu.isRunning();
}

bool ExecutionEnvironment::stepOut(const std::function<bool(uint32_t)>& shouldBreak) {
    if (!hasProgram) {
        return false;
    }
    const uint32_t target = static_cast<uint32_t>(cpu.getReg(1));
    cpu.runUntilHaltOrBreakpoint([&](uint32_t ip) {
        return shouldBreak(ip) || ip >= target;
    });
    return cpu.isRunning();
}

}  // namespace debugger
