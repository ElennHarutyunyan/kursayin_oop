#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <cstdint>
#include <map>
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
    std::vector<int32_t> getDataSection() const { return dataSection; }
    int32_t getDataBaseAddress() const { return dataBaseAddress; }

private:
    std::vector<Instruction> instructions;
    std::vector<int32_t> dataSection;
    SymbolTable& symbolTable;
    int32_t dataBaseAddress = 4096;
    std::vector<std::vector<size_t>> breakPatchStack;
    std::vector<int32_t> continueTargetStack;
    std::map<const DeclarationNode*, int32_t> staticInitGuardAddressByDecl;

    // Օժանդակ ֆունկցիա Expression-ների համար
    void generateExpression(ExprNode* expr, int targetReg);
    int appendDataWords(const std::vector<int32_t>& words);
    void patchJump(size_t index, int32_t target);
    void patchBranch(size_t index, int32_t target);
};

#endif