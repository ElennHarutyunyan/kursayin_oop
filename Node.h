#ifndef NODE_H
#define NODE_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

// Represents a single step of the calculation (the 'string' expression step)
struct Line {
    char op;            // '+', '-', '*', '/', '=', 'P' (Print)
    int leftIdx;        // Index of the left operand in the result vector (rv)
    int rightIdx;       // Index of the right operand in the result vector (rv)
    int resIdx;         // Index where the result will be stored in the result vector (rv)
};

struct Node {
    virtual ~Node() = default;
    
    // Every node now 'flattens' itself into the program vector
    // Returns the index in 'rv' where this node's value is stored
    virtual int flatten(std::vector<Line>& program, 
                        std::vector<double>& rv, 
                        std::unordered_map<std::string, int>& varMap) = 0;
};

struct NumberNode : Node {
    double value;
    NumberNode(double val) : value(val) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        // Numbers are stored directly into the result vector
        rv.push_back(value);
        return rv.size() - 1; // Return the position of this number
    }
};

struct BinOpNode : Node {
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    char op;
    
    BinOpNode(std::shared_ptr<Node> l, char o, std::shared_ptr<Node> r)
        : left(l), op(o), right(r) {}
        
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        // 1. Get indices for children (Post-order traversal)
        int lIdx = left->flatten(program, rv, varMap);
        int rIdx = right->flatten(program, rv, varMap);
        
        // 2. Reserve a spot for this operation's result
        rv.push_back(0.0); 
        int myIdx = rv.size() - 1;
        
        // 3. Add the instruction to the program
        program.push_back({op, lIdx, rIdx, myIdx});
        
        return myIdx;
    }
};

struct VarNode : Node {
    std::string name;
    VarNode(const std::string &n) : name(n) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        // If the variable doesn't exist in our memory yet, initialize it
        if (varMap.find(name) == varMap.end()) {
            rv.push_back(0.0);
            varMap[name] = rv.size() - 1;
        }
        return varMap[name];
    }
};

struct AssignNode : Node {
    std::string name;
    std::shared_ptr<Node> value;
    AssignNode(const std::string &n, std::shared_ptr<Node> val) : name(n), value(val) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        int valIdx = value->flatten(program, rv, varMap);
        
        // Ensure the variable name has a reserved index
        if (varMap.find(name) == varMap.end()) {
            rv.push_back(0.0);
            varMap[name] = rv.size() - 1;
        }
        
        int varIdx = varMap[name];
        // Add assignment instruction: rv[varIdx] = rv[valIdx]
        program.push_back({'=', valIdx, -1, varIdx});
        
        return varIdx;
    }
};

struct PrintNode : Node {
    std::shared_ptr<Node> expr;
    PrintNode(std::shared_ptr<Node> e) : expr(e) {}
    
    int flatten(std::vector<Line>& program, std::vector<double>& rv, std::unordered_map<std::string, int>& varMap) override {
        int exprIdx = expr->flatten(program, rv, varMap);
        // Special instruction for printing
        program.push_back({'P', exprIdx, -1, -1});
        return -1;
    }
};

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