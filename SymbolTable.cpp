#include "SymbolTable.h"

int SymbolTable::getAddress(const std::string& name) {
    if (table.find(name) == table.end()) {
        table[name] = {name, nextAddr++};
    }
    return table[name].address;
}