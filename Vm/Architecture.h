#pragma once

#include <cstdint>
#include <string>

struct CpuArchitecture {
    std::string name = "RISC5";
    uint32_t xlenBits = 32;
    uint32_t registerCount = 16;
    uint32_t datapathBits = 16;
    bool littleEndian = true;
};
