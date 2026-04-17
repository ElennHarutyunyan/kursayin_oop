#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <string>

class Memory {
private:
    std::vector<uint8_t> storage;
    size_t size;

    // Սահմաններ սեկցիաների համար (պարզեցված)
    size_t stackPointer;
    size_t heapPointer;

public:
    // memSize-ը բայթերով է (օր. 1024 * 1024 = 1MB)
    Memory(size_t memSize = 65536) : size(memSize) {
        storage.resize(size, 0);
        stackPointer = size; // Stack-ը սկսում է վերջից և աճում է դեպի ներքև
        heapPointer = size / 2; // Heap-ը սկսում է մեջտեղից
    }

    // --- Հիմնական գործողություններ (Read/Write) ---

    // Գրել 1 բայթ (byte)
    void write8(uint32_t address, uint8_t value) {
        if (address >= size) throw std::out_of_range("Memory write out of bounds");
        storage[address] = value;
    }

    // Գրել 4 բայթ (word) - RISC-V հիմնական միավորը
    void write32(uint32_t address, uint32_t value) {
        if (address + 3 >= size) throw std::out_of_range("Memory write out of bounds");
        // Little-endian format
        storage[address]     = (value & 0xFF);
        storage[address + 1] = ((value >> 8) & 0xFF);
        storage[address + 2] = ((value >> 16) & 0xFF);
        storage[address + 3] = ((value >> 24) & 0xFF);
    }

    // Կարդալ 1 բայթ
    uint8_t read8(uint32_t address) const {
        if (address >= size) throw std::out_of_range("Memory read out of bounds");
        return storage[address];
    }

    // Կարդալ 4 բայթ
    uint32_t read32(uint32_t address) const {
        if (address + 3 >= size) throw std::out_of_range("Memory read out of bounds");
        // Little-endian վերականգնում
        return (uint32_t)storage[address] |
               ((uint32_t)storage[address + 1] << 8) |
               ((uint32_t)storage[address + 2] << 16) |
               ((uint32_t)storage[address + 3] << 24);
    }

    // --- Օժանդակ ֆունկցիաներ ---

    size_t getSize() const { return size; }
    
    // Հիշողության դեմփ (dump) դեբագի համար
    void hexDump(uint32_t start, uint32_t length) const;

    // Մաքրել հիշողությունը
    void clear() { std::fill(storage.begin(), storage.end(), 0); }
};

#endif