#include "ProgramLoader.h"

#include <fstream>
#include <stdexcept>

namespace debugger {

void ProgramLoader::verifyMagic(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open executable: " + path);
    }
    uint32_t magic = 0;
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (!in) {
        throw std::runtime_error("Executable is too small: " + path);
    }
    if (magic != linker_stage::kExecutableMagic) {
        throw std::runtime_error("Invalid executable magic number");
    }
}

LoadedProgram ProgramLoader::loadIntoEnvironment(const std::string& path) {
    verifyMagic(path);
    auto image = linker_stage::ToolchainLinker::readExecutable(path);

    LoadedProgram loaded;
    loaded.path = path;
    loaded.text = std::move(image.text);
    loaded.data = std::move(image.data);
    loaded.symbols = std::move(image.symbols);
    loaded.bssSize = image.bssSize;
    loaded.dataBaseAddress = 4096;
    return loaded;
}

}  // namespace debugger
