#pragma once

#include <string>
#include <vector>

#include "../Common/Instruction.h"

class Loader {
public:
    static std::vector<Instruction> loadExecutable(const std::string& filename);
};
