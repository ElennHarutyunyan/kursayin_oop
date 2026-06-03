#include "Hdd.h"

#include <stdexcept>

namespace debugger {

void Hdd::write32(uint32_t offset, uint32_t value) {
    if (offset + 3 >= storage.size()) {
        throw std::out_of_range("HDD write out of bounds");
    }
    storage[offset] = static_cast<uint8_t>(value & 0xFF);
    storage[offset + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    storage[offset + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    storage[offset + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

uint32_t Hdd::read32(uint32_t offset) const {
    if (offset + 3 >= storage.size()) {
        throw std::out_of_range("HDD read out of bounds");
    }
    return static_cast<uint32_t>(storage[offset]) |
           (static_cast<uint32_t>(storage[offset + 1]) << 8) |
           (static_cast<uint32_t>(storage[offset + 2]) << 16) |
           (static_cast<uint32_t>(storage[offset + 3]) << 24);
}

}  // namespace debugger
