#include "Dma.h"

namespace debugger {

void Dma::transferToRam(uint32_t hddOffset, uint32_t ramAddress, uint32_t bytes) {
    for (uint32_t i = 0; i < bytes; ++i) {
        const uint32_t wordOffset = hddOffset + i;
        const uint32_t byteInWord = wordOffset % 4;
        const uint32_t hddWord = hdd.read32(wordOffset - byteInWord);
        const uint8_t value = static_cast<uint8_t>((hddWord >> (8 * byteInWord)) & 0xFF);
        memory.write8(ramAddress + i, value);
    }
}

void Dma::transferToHdd(uint32_t ramAddress, uint32_t hddOffset, uint32_t bytes) {
    uint32_t current = 0;
    for (uint32_t i = 0; i < bytes; ++i) {
        const uint8_t value = memory.read8(ramAddress + i);
        const uint32_t shift = (i % 4) * 8;
        current |= static_cast<uint32_t>(value) << shift;
        if ((i % 4) == 3 || i + 1 == bytes) {
            hdd.write32(hddOffset + (i / 4) * 4, current);
            current = 0;
        }
    }
}

}  // namespace debugger
