#pragma once

#include <cstdint>
#include <vector>

namespace debugger {

class Hdd {
public:
    explicit Hdd(size_t capacityBytes = 8192) : storage(capacityBytes, 0) {}

    void write32(uint32_t offset, uint32_t value);
    uint32_t read32(uint32_t offset) const;
    size_t capacity() const { return storage.size(); }

private:
    std::vector<uint8_t> storage;
};

}  // namespace debugger
