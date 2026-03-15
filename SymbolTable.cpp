#include "SymbolTable.h"
#include <stdexcept>

void SymbolTable::set(const std::string &name, int index) {
    table[name] = index;
}

int SymbolTable::get(const std::string &name) const {
    auto it = table.find(name);
    if (it == table.end()) {
        throw std::runtime_error("Variable '" + name + "' used before assignment.");
    }
    return it->second;
}

bool SymbolTable::exists(const std::string &name) const {
    return table.find(name) != table.end();
}