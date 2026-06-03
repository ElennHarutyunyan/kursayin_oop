#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "../../Common/Instruction.h"

namespace linker_stage {

constexpr uint32_t kExecutableMagic = 0x454C454E;  // "ELEN"

enum class SectionType : uint32_t {
    Code = 1,
    Data = 2,
    Bss = 3,
    Symtab = 4,
};

enum SectionFlags : uint32_t {
    Read = 1u,
    Write = 2u,
    Exec = 4u,
};

struct FileHeader {
    uint32_t magic = kExecutableMagic;
    uint32_t headerSize = 0;
    uint32_t sectionCount = 0;
};

struct SectionDescriptor {
    uint32_t type = 0;
    uint32_t fileOffset = 0;
    uint32_t size = 0;
    uint32_t vma = 0;
    uint32_t flags = 0;
};

struct DebugSymbol {
    std::string name;
    uint32_t address = 0;
};

struct ExecutableImage {
    FileHeader header;
    std::vector<SectionDescriptor> sections;
    std::vector<Instruction> text;
    std::vector<int32_t> data;
    uint32_t bssSize = 0;
    std::vector<DebugSymbol> symbols;
};

class ToolchainLinker {
public:
    ExecutableImage linkToImage(const std::vector<Instruction>& text,
                                const std::vector<int32_t>& data,
                                const std::vector<DebugSymbol>& symbols = {},
                                uint32_t bssSize = 0) const;
    void writeExecutable(const ExecutableImage& image, const std::string& outputPath) const;
    static ExecutableImage readExecutable(const std::string& inputPath);
};

}  // namespace linker_stage
