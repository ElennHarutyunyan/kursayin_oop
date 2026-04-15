#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>
#include <string>
#include "Instruction.h"
#include "SymbolTable.h"

struct Node {
    virtual ~Node() = default;
    virtual void flatten(std::vector<Instruction>& prog, SymbolTable& sym) = 0;
};

struct NumberNode : public Node {
    double value;
    NumberNode(double v) : value(v) {}
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        prog.push_back(Instruction(LI, -1, value));
    }
};

struct VarNode : public Node {
    std::string name;
    VarNode(std::string n) : name(n) {}
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        prog.push_back(Instruction(LOAD, sym.getAddress(name), 0.0));
    }
};

struct IfNode : public Node {
    std::shared_ptr<Node> cond, body;
    IfNode(std::shared_ptr<Node> c, std::shared_ptr<Node> b) : cond(c), body(b) {}
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        cond->flatten(prog, sym);
        int jzIdx = (int)prog.size();
        prog.push_back(Instruction(JZ, -1, 0.0)); 
        if (body) body->flatten(prog, sym);
        prog[jzIdx].address = (int)prog.size(); // Patch JZ to skip the body
    }
};

struct WhileNode : public Node {
    std::shared_ptr<Node> cond, body;
    WhileNode(std::shared_ptr<Node> c, std::shared_ptr<Node> b) : cond(c), body(b) {}
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        int start = (int)prog.size();
        cond->flatten(prog, sym);
        int jzIdx = (int)prog.size();
        prog.push_back(Instruction(JZ, -1, 0.0));
        if (body) body->flatten(prog, sym);
        prog.push_back(Instruction(JMP, start, 0.0)); // Jump back to condition
        prog[jzIdx].address = (int)prog.size();      // Patch JZ to exit loop
    }
};

struct AssignNode : public Node {
    std::string name;
    std::shared_ptr<Node> expr;
    AssignNode(std::string n, std::shared_ptr<Node> e) : name(n), expr(e) {}
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        expr->flatten(prog, sym);
        prog.push_back(Instruction(STORE, sym.getAddress(name), 0.0));
    }
};

struct PrintNode : public Node {
    std::shared_ptr<Node> expr;
    PrintNode(std::shared_ptr<Node> e) : expr(e) {}
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        expr->flatten(prog, sym);
        prog.push_back(Instruction(PRINT, -1, 0.0));
    }
};

struct StatementsNode : public Node {
    std::vector<std::shared_ptr<Node>> statements;
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        for (auto& s : statements) if (s) s->flatten(prog, sym);
    }
};

struct BinOpNode : public Node {
    char op;
    std::shared_ptr<Node> left, right;
    BinOpNode(char o, std::shared_ptr<Node> l, std::shared_ptr<Node> r) : op(o), left(l), right(r) {}
    void flatten(std::vector<Instruction>& prog, SymbolTable& sym) override {
        left->flatten(prog, sym);
        right->flatten(prog, sym);
        prog.push_back(Instruction((op == '+' ? ADD : SUB), -1, 0.0));
    }
};

#endif