#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "../Common/Instruction.h"
#include "Architecture.h"
#include "Memory.h"

enum class ExecutionMode { Run, RunDebug, RunSingle };

class VirtualMachine {
public:
    explicit VirtualMachine(Memory& mem);

    void loadProgram(const std::vector<Instruction>& prog);
    void initializeEnvironment(uint32_t dataBaseAddress, uint32_t initialSp);
    void reset();

    bool step();
    void run();
    void runUntilHaltOrBreakpoint(const std::function<bool(uint32_t)>& shouldBreak);

    bool isRunning() const { return running; }
    uint32_t getPc() const { return pc; }
    void setPc(uint32_t newPc) { pc = newPc; }
    const Instruction& currentInstruction() const;
    size_t programSize() const { return program.size(); }

    void dumpRegisters() const;
    int32_t getReg(int idx) const { return registers[idx]; }
    void setReg(int idx, int32_t value) { registers[idx] = value; }
    const CpuArchitecture& getArchitecture() const { return arch; }

    uint32_t getStackPointer() const { return stackPointer; }
    uint32_t getBasePointer() const { return basePointer; }

private:
    int32_t registers[32];
    uint32_t pc;
    uint32_t stackPointer;
    uint32_t basePointer;
    Memory& memory;
    CpuArchitecture arch;
    bool running;
    std::vector<Instruction> program;

    void executeInstruction(const Instruction& instr);
};
