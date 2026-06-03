#pragma once

#include <functional>

#include "../Vm/Memory.h"
#include "../Vm/VirtualMachine.h"
#include "../src/runtime/Bus.h"
#include "DebuggerTypes.h"
#include "Dma.h"
#include "Hdd.h"

namespace debugger {

class ExecutionEnvironment {
public:
    explicit ExecutionEnvironment(size_t memoryBytes = 65536);

    void initialize(const LoadedProgram& program);
    void reset();

    bool fetchDecodeExecuteStep();
    void run(ExecutionMode mode, const std::function<bool(uint32_t)>& shouldBreak);
    bool stepOver(const std::function<bool(uint32_t)>& shouldBreak);
    bool stepOut(const std::function<bool(uint32_t)>& shouldBreak);

    uint32_t getIp() const { return cpu.getPc(); }
    const LoadedProgram& getProgram() const { return loadedProgram; }
    VirtualMachine& getCpu() { return cpu; }
    const VirtualMachine& getCpu() const { return cpu; }
    Memory& getMemory() { return memory; }

private:
    Memory memory;
    runtime::Bus bus;
    Hdd hdd;
    Dma dma;
    VirtualMachine cpu;
    LoadedProgram loadedProgram;
    bool hasProgram = false;

    static bool isCallInstruction(const Instruction& instr);
    void loadDataSection();
    void loadCodeSection();
};

}  // namespace debugger
