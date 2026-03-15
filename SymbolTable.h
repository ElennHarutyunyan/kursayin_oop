#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>

class SymbolTable {
private:
    std::unordered_map<std::string, int> table;

public:
    void set(const std::string &name, int index);
    int get(const std::string &name) const;
    bool exists(const std::string &name) const;
};

#endif