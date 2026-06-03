#include "ExecutableFormat.h"

#include <fstream>
#include <stdexcept>

namespace linker_stage {

namespace {

void writeSymbol(std::ostream& out, const DebugSymbol& symbol) {
    const uint32_t nameLen = static_cast<uint32_t>(symbol.name.size());
    out.write(reinterpret_cast<const char*>(&symbol.address), sizeof(symbol.address));
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    out.write(symbol.name.data(), static_cast<std::streamsize>(symbol.name.size()));
}

DebugSymbol readSymbol(std::istream& in) {
    DebugSymbol symbol;
    uint32_t nameLen = 0;
    in.read(reinterpret_cast<char*>(&symbol.address), sizeof(symbol.address));
    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (!in) {
        throw std::runtime_error("Unexpected end of file while reading debug symbol");
    }
    symbol.name.resize(nameLen);
    if (nameLen > 0) {
        in.read(symbol.name.data(), static_cast<std::streamsize>(nameLen));
    }
    if (!in) {
        throw std::runtime_error("Unexpected end of file while reading debug symbol name");
    }
    return symbol;
}

}  // namespace

ExecutableImage ToolchainLinker::linkToImage(const std::vector<Instruction>& text,
                                             const std::vector<int32_t>& data,
                                             const std::vector<DebugSymbol>& symbols,
                                             uint32_t bssSize) const {
    ExecutableImage image;
    image.text = text;
    image.data = data;
    image.symbols = symbols;
    image.bssSize = bssSize;
    image.sections = {
        {static_cast<uint32_t>(SectionType::Code), 0,
         static_cast<uint32_t>(text.size() * sizeof(Instruction)), 0u,
         static_cast<uint32_t>(SectionFlags::Read) | static_cast<uint32_t>(SectionFlags::Exec)},
        {static_cast<uint32_t>(SectionType::Data), 0, static_cast<uint32_t>(data.size() * sizeof(int32_t)),
         4096u, static_cast<uint32_t>(SectionFlags::Read) | static_cast<uint32_t>(SectionFlags::Write)},
        {static_cast<uint32_t>(SectionType::Bss), 0, bssSize, 8192u,
         static_cast<uint32_t>(SectionFlags::Read) | static_cast<uint32_t>(SectionFlags::Write)},
        {static_cast<uint32_t>(SectionType::Symtab), 0, 0, 0, static_cast<uint32_t>(SectionFlags::Read)},
    };
    image.header.magic = kExecutableMagic;
    image.header.sectionCount = static_cast<uint32_t>(image.sections.size());
    image.header.headerSize =
        static_cast<uint32_t>(sizeof(FileHeader) + image.sections.size() * sizeof(SectionDescriptor));
    return image;
}

void ToolchainLinker::writeExecutable(const ExecutableImage& image, const std::string& outputPath) const {
    std::ofstream out(outputPath, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Unable to open output executable: " + outputPath);
    }

    ExecutableImage layout = image;
    const uint32_t tableBytes =
        static_cast<uint32_t>(sizeof(FileHeader) + layout.sections.size() * sizeof(SectionDescriptor));
    layout.header.headerSize = tableBytes;
    layout.header.sectionCount = static_cast<uint32_t>(layout.sections.size());

    uint32_t cursor = tableBytes;
    for (auto& section : layout.sections) {
        if (section.type == static_cast<uint32_t>(SectionType::Bss)) {
            section.fileOffset = 0;
            continue;
        }
        if (section.type == static_cast<uint32_t>(SectionType::Symtab)) {
            continue;
        }
        section.fileOffset = cursor;
        cursor += section.size;
    }

    std::vector<char> symtabBlob;
    for (const auto& symbol : layout.symbols) {
        const uint32_t nameLen = static_cast<uint32_t>(symbol.name.size());
        symtabBlob.insert(symtabBlob.end(), reinterpret_cast<const char*>(&symbol.address),
                          reinterpret_cast<const char*>(&symbol.address) + sizeof(symbol.address));
        symtabBlob.insert(symtabBlob.end(), reinterpret_cast<const char*>(&nameLen),
                          reinterpret_cast<const char*>(&nameLen) + sizeof(nameLen));
        symtabBlob.insert(symtabBlob.end(), symbol.name.begin(), symbol.name.end());
    }

    for (auto& section : layout.sections) {
        if (section.type == static_cast<uint32_t>(SectionType::Symtab)) {
            section.fileOffset = cursor;
            section.size = static_cast<uint32_t>(symtabBlob.size());
            cursor += section.size;
        }
    }

    out.write(reinterpret_cast<const char*>(&layout.header), sizeof(FileHeader));
    out.write(reinterpret_cast<const char*>(layout.sections.data()),
              static_cast<std::streamsize>(layout.sections.size() * sizeof(SectionDescriptor)));
    if (!layout.text.empty()) {
        out.write(reinterpret_cast<const char*>(layout.text.data()),
                  static_cast<std::streamsize>(layout.text.size() * sizeof(Instruction)));
    }
    if (!layout.data.empty()) {
        out.write(reinterpret_cast<const char*>(layout.data.data()),
                  static_cast<std::streamsize>(layout.data.size() * sizeof(int32_t)));
    }
    if (!symtabBlob.empty()) {
        out.write(symtabBlob.data(), static_cast<std::streamsize>(symtabBlob.size()));
    }
}

ExecutableImage ToolchainLinker::readExecutable(const std::string& inputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Unable to open executable: " + inputPath);
    }

    ExecutableImage image;
    in.read(reinterpret_cast<char*>(&image.header), sizeof(FileHeader));
    if (!in || image.header.magic != kExecutableMagic) {
        throw std::runtime_error("Invalid executable magic number");
    }

    image.sections.resize(image.header.sectionCount);
    in.read(reinterpret_cast<char*>(image.sections.data()),
            static_cast<std::streamsize>(image.sections.size() * sizeof(SectionDescriptor)));

    for (const auto& section : image.sections) {
        if (section.type == static_cast<uint32_t>(SectionType::Code)) {
            const size_t count = section.size / sizeof(Instruction);
            image.text.resize(count);
            in.seekg(static_cast<std::streamoff>(section.fileOffset));
            in.read(reinterpret_cast<char*>(image.text.data()),
                    static_cast<std::streamsize>(section.size));
        } else if (section.type == static_cast<uint32_t>(SectionType::Data)) {
            const size_t count = section.size / sizeof(int32_t);
            image.data.resize(count);
            in.seekg(static_cast<std::streamoff>(section.fileOffset));
            in.read(reinterpret_cast<char*>(image.data.data()),
                    static_cast<std::streamsize>(section.size));
        } else if (section.type == static_cast<uint32_t>(SectionType::Bss)) {
            image.bssSize = section.size;
        } else if (section.type == static_cast<uint32_t>(SectionType::Symtab)) {
            in.seekg(static_cast<std::streamoff>(section.fileOffset));
            const size_t end = section.fileOffset + section.size;
            while (static_cast<uint32_t>(in.tellg()) < end) {
                image.symbols.push_back(readSymbol(in));
            }
        }
    }
    return image;
}

}  // namespace linker_stage
