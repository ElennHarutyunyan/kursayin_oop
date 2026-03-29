#ifndef NODE_H
#define NODE_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>

// --- Low-level instruction set ---
enum class OpCode {
    LOAD,
    ADD,
    SUB,
    MUL,
    DIV,
    STORE,
    PRINT
};

// Each instruction references operand/result indices in memory (rv)
struct Line {
    OpCode op;
    int operandIdx;  // Index in rv for LOAD/ADD/SUB/MUL/DIV/PRINT
    int resultIdx;   // Used only for STORE
};

// --- Abstract Node ---
struct Node {
    virtual ~Node() = default;
    virtual int flatten(std::vector<Line>& program, 
                        std::vector<double>& rv, 
                        std::unordered_map<std::string, int>& varMap) = 0;
};

// --- Number Node ---
struct NumberNode : Node {
    double value;
    NumberNode(double val) : value(val) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        rv.push_back(value);
        return rv.size() - 1;
    }
};

// --- Binary Operation Node ---
struct BinOpNode : Node {
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    char op;
    
    BinOpNode(std::shared_ptr<Node> l, char o, std::shared_ptr<Node> r)
        : left(l), op(o), right(r) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        int lIdx = left->flatten(program, rv, varMap);
        int rIdx = right->flatten(program, rv, varMap);
        rv.push_back(0.0); 
        int myIdx = rv.size() - 1;

        // Generate LOAD + OP + STORE sequence
        program.push_back({OpCode::LOAD, lIdx, -1});
        switch (op) {
            case '+': program.push_back({OpCode::ADD, rIdx, -1}); break;
            case '-': program.push_back({OpCode::SUB, rIdx, -1}); break;
            case '*': program.push_back({OpCode::MUL, rIdx, -1}); break;
            case '/': program.push_back({OpCode::DIV, rIdx, -1}); break;
            default:
                throw std::runtime_error("Unknown binary operator");
        }
        program.push_back({OpCode::STORE, -1, myIdx});
        return myIdx;
    }
};

// --- Variable Node ---
struct VarNode : Node {
    std::string name;
    VarNode(const std::string &n) : name(n) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        if (varMap.find(name) == varMap.end()) {
            rv.push_back(0.0);
            varMap[name] = rv.size() - 1;
        }
        return varMap[name];
    }
};

// --- Assignment Node ---
struct AssignNode : Node {
    std::string name;
    std::shared_ptr<Node> value;
    AssignNode(const std::string &n, std::shared_ptr<Node> val) : name(n), value(val) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        int valIdx = value->flatten(program, rv, varMap);

        if (varMap.find(name) == varMap.end()) {
            rv.push_back(0.0);
            varMap[name] = rv.size() - 1;
        }

        int varIdx = varMap[name];

        // Assignment: LOAD value -> STORE variable
        program.push_back({OpCode::LOAD, valIdx, -1});
        program.push_back({OpCode::STORE, -1, varIdx});

        return varIdx;
    }
};

// --- Print Node ---
struct PrintNode : Node {
    std::shared_ptr<Node> expr;
    PrintNode(std::shared_ptr<Node> e) : expr(e) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        int exprIdx = expr->flatten(program, rv, varMap);
        program.push_back({OpCode::PRINT, exprIdx, -1});
        return -1;
    }
};

// --- Statements Node ---
struct StatementsNode : Node {
    std::vector<std::shared_ptr<Node>> statements;
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        for (auto &stmt : statements) {
            stmt->flatten(program, rv, varMap);
        }
        return -1;
    }
};

#endif // NODE_H