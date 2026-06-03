#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../Common/Instruction.h"
#include "../src/linker/ExecutableFormat.h"

namespace debugger {

enum class ExecutionMode { Run, RunDebug, RunSingle };

struct LoadedProgram {
    std::vector<linker_stage::DebugSymbol> symbols;
    std::vector<Instruction> text;
    std::vector<int32_t> data;
    uint32_t dataBaseAddress = 4096;
    uint32_t bssSize = 0;
    std::string path;
};

}  // namespace debugger
