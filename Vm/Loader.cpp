#include "Loader.h"

#include "../src/linker/ExecutableFormat.h"
#include <fstream>
#include <stdexcept>

std::vector<Instruction> Loader::loadExecutable(const std::string& filename) {
    auto image = linker_stage::ToolchainLinker::readExecutable(filename);
    return image.text;
}
