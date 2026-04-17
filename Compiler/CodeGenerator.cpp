#include "CodeGenerator.h"
#include "ASTNodes.h"
#include <stdexcept>

void CodeGenerator::generate(ASTNode* node) {
    if (!node) return;

    if (auto assign = dynamic_cast<AssignmentNode*>(node)) {
        generateExpression(assign->expression.get(), 5); 
        int varReg = symbolTable.getRegister(assign->varName);
        
        // Կիրառում ենք static_cast<uint8_t> բոլոր ռեգիստրների համար
        instructions.push_back({
            OpCode::ADD, 
            static_cast<uint8_t>(varReg), 
            5, 
            0, 
            0
        });
    }
    else if (auto ifNode = dynamic_cast<IfNode*>(node)) {
        // ...
    }
}

void CodeGenerator::generateExpression(ExprNode* expr, int targetReg) {
    if (!expr) return;
    uint8_t dest = static_cast<uint8_t>(targetReg);

    if (auto lit = dynamic_cast<IntLiteralNode*>(expr)) {
        instructions.push_back({OpCode::ADDI, dest, 0, 0, lit->value});
    }
    else if (auto var = dynamic_cast<VariableNode*>(expr)) {
        int varReg = symbolTable.getRegister(var->name);
        instructions.push_back({OpCode::ADD, dest, static_cast<uint8_t>(varReg), 0, 0});
    }
    else if (auto bin = dynamic_cast<BinaryOpNode*>(expr)) {
        generateExpression(bin->left.get(), 6);
        generateExpression(bin->right.get(), 7);
        
        if (bin->op == "+") {
            instructions.push_back({OpCode::ADD, dest, 6, 7, 0});
        } else if (bin->op == "-") {
            instructions.push_back({OpCode::SUB, dest, 6, 7, 0});
        }
    }
}