#include "Memory.h"

#include <iomanip>
#include <iostream>

Memory::Memory(size_t memSize) : size(memSize) {
    storage.resize(size, 0);
    stackPointer = layout.stackTop;
    heapPointer = layout.bssBase;
    staticPointer = layout.staticBase;
    bssPointer = layout.bssBase;
}

void Memory::write8(uint32_t address, uint8_t value) {
    if (address >= size) {
        throw std::out_of_range("Memory write out of bounds");
    }
    storage[address] = value;
}

void Memory::write32(uint32_t address, uint32_t value) {
    if (address + 3 >= size) {
        throw std::out_of_range("Memory write out of bounds");
    }
    storage[address] = static_cast<uint8_t>(value & 0xFF);
    storage[address + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    storage[address + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    storage[address + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

uint8_t Memory::read8(uint32_t address) const {
    if (address >= size) {
        throw std::out_of_range("Memory read out of bounds");
    }
    return storage[address];
}

uint32_t Memory::read32(uint32_t address) const {
    if (address + 3 >= size) {
        throw std::out_of_range("Memory read out of bounds");
    }
    return static_cast<uint32_t>(storage[address]) |
           (static_cast<uint32_t>(storage[address + 1]) << 8) |
           (static_cast<uint32_t>(storage[address + 2]) << 16) |
           (static_cast<uint32_t>(storage[address + 3]) << 24);
}

void Memory::reserveBss(uint32_t bytes) {
    const size_t aligned = (bytes + 3u) & ~3u;
    if (bssPointer + aligned > layout.stackBase) {
        throw std::out_of_range("BSS reservation exceeds memory layout");
    }
    for (size_t i = 0; i < aligned; ++i) {
        storage[bssPointer + i] = 0;
    }
    bssPointer += aligned;
    if (heapPointer < bssPointer) {
        heapPointer = bssPointer;
    }
}

uint32_t Memory::allocateDynamic(uint32_t bytes) {
    const size_t aligned = (bytes + 3u) & ~3u;
    if (heapPointer + aligned >= stackPointer) {
        throw std::out_of_range("Dynamic memory exhausted");
    }
    const uint32_t addr = static_cast<uint32_t>(heapPointer);
    heapPointer += aligned;
    return addr;
}

uint32_t Memory::allocateStatic(uint32_t bytes) {
    const size_t aligned = (bytes + 3u) & ~3u;
    if (staticPointer + aligned >= bssPointer) {
        throw std::out_of_range("Static memory exhausted");
    }
    const uint32_t addr = static_cast<uint32_t>(staticPointer);
    staticPointer += aligned;
    return addr;
}

void Memory::hexDump(uint32_t start, uint32_t length) const {
    for (uint32_t offset = 0; offset < length; offset += 16) {
        const uint32_t addr = start + offset;
        std::cout << std::hex << std::setw(8) << std::setfill('0') << addr << ": ";
        for (uint32_t i = 0; i < 16 && offset + i < length; ++i) {
            if (addr + i < size) {
                std::cout << std::setw(2) << static_cast<int>(storage[addr + i]) << ' ';
            }
        }
        std::cout << '\n';
    }
    std::cout << std::dec;
}

void Memory::clear() {
    std::fill(storage.begin(), storage.end(), 0);
    heapPointer = layout.bssBase;
    staticPointer = layout.staticBase;
    bssPointer = layout.bssBase;
    stackPointer = layout.stackTop;
}
