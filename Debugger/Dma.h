#pragma once

#include "../Vm/Memory.h"
#include "Hdd.h"

namespace debugger {

class Dma {
public:
    Dma(Memory& ram, Hdd& disk) : memory(ram), hdd(disk) {}

    void transferToRam(uint32_t hddOffset, uint32_t ramAddress, uint32_t bytes);
    void transferToHdd(uint32_t ramAddress, uint32_t hddOffset, uint32_t bytes);

private:
    Memory& memory;
    Hdd& hdd;
};

}  // namespace debugger
