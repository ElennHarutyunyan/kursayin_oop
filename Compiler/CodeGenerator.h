#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <vector>
#include <string>
#include "ASTNodes.h"
#include "SymbolTable.h"
#include "../Common/Instruction.h"

class CodeGenerator {
public:
    CodeGenerator(SymbolTable& st) : symbolTable(st) {}

    // Հիմնական մուտքի կետը
    void generate(ASTNode* node);

    // Վերադարձնում է գեներացված RISC-V հրահանգները
    std::vector<Instruction> getResult() const { return instructions; }

private:
    std::vector<Instruction> instructions;
    SymbolTable& symbolTable;

    // Օժանդակ ֆունկցիա Expression-ների համար
    void generateExpression(ExprNode* expr, int targetReg);
};

#endif