#include <iostream>
#include <string>
#include <cstdint>

uint64_t SALT_1 = 0xA784F19E3B5D28C6ULL;
uint64_t SALT_2 = 0x6E4C90B21F87D35AULL;

bool validateSerial(std::string serialKey) {
    if (serialKey.substr(0, 4) == "EXTR") serialKey = serialKey.substr(5);
    std::string block1 = serialKey.substr(0, 4);
    std::string block2 = serialKey.substr(5, 4);
    std::string block3 = serialKey.substr(10, 4);
    std::string block4 = serialKey.substr(15, 4);

    uint32_t val1 = std::stoul(block1, nullptr, 16);
    uint32_t val2 = std::stoul(block2, nullptr, 16);
    uint32_t val3 = std::stoul(block3, nullptr, 16);
    uint32_t val4 = std::stoul(block4, nullptr, 16);

    uint64_t seed = ((uint64_t)val1 << 32) | val1;
    uint32_t expected2 = (uint32_t)(((seed ^ SALT_1) * 0x45D9F3BULL) >> 16) & 0xFFFF;
    uint32_t expected3 = (uint32_t)((((seed << 13) | (seed >> 19)) ^ SALT_2) * 0x27D4EB2DULL >> 16) & 0xFFFF;
    uint32_t expected4 = ((val1 ^ expected2 ^ expected3 ^ 0xBEEF) * 0x119DE1ULL) & 0xFFFF;

    return (val2 == expected2 && val3 == expected3 && val4 == expected4);
}

int main() {
    std::cout << (validateSerial("EXTR-5458-7C57-35CC-65AC") ? "VALID" : "INVALID") << std::endl;
    return 0;
}
