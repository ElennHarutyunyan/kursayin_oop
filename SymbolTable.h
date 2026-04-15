#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <string>
#include <unordered_map>

struct SymbolEntry {
    std::string name;
    int address;
};

class SymbolTable {
private:
    std::unordered_map<std::string, SymbolEntry> table;
    int nextAddr = 0;
public:
    int getAddress(const std::string& name);
};
#endif