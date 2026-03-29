#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "Node.h"
#include <memory>
#include <vector>
#include <unordered_map>

class Evaluator {
private:
    std::vector<double> rv;              // Memory
    std::vector<Line> program;           // Instruction list
    std::unordered_map<std::string, int> varMap;  // Variable name -> index

public:
    void run(std::shared_ptr<StatementsNode> root);

private:
    void execute();
};

#endif