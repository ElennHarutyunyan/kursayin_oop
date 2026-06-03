#pragma once

#include <string>

#include "DebuggerTypes.h"

namespace debugger {

class ProgramLoader {
public:
    static void verifyMagic(const std::string& path);
    static LoadedProgram loadIntoEnvironment(const std::string& path);
};

}  // namespace debugger
