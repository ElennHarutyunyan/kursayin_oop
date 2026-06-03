#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

class Memory {
public:
    static constexpr uint32_t kRamSize = 65536;
    static constexpr uint32_t kStackSize = 16384;
    static constexpr uint32_t kStackBase = kRamSize - kStackSize;

    struct Layout {
        uint32_t codeBase = 0;
        uint32_t staticBase = 4096;
        uint32_t bssBase = 8192;
        uint32_t stackBase = kStackBase;
        uint32_t stackTop = kRamSize;
    };

    explicit Memory(size_t memSize = kRamSize);

    void write8(uint32_t address, uint8_t value);
    void write32(uint32_t address, uint32_t value);
    uint8_t read8(uint32_t address) const;
    uint32_t read32(uint32_t address) const;

    size_t getSize() const { return size; }
    uint32_t getCodeBase() const { return layout.codeBase; }
    uint32_t getStaticBase() const { return layout.staticBase; }
    uint32_t getStackPointer() const { return static_cast<uint32_t>(stackPointer); }

    void reserveBss(uint32_t bytes);
    uint32_t allocateDynamic(uint32_t bytes);
    uint32_t allocateStatic(uint32_t bytes);
    void hexDump(uint32_t start, uint32_t length) const;
    void clear();

private:
    std::vector<uint8_t> storage;
    size_t size;
    Layout layout;
    size_t stackPointer;
    size_t heapPointer;
    size_t staticPointer;
    size_t bssPointer;
};
