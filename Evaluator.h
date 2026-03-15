#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "Node.h"
#include <memory>
#include <vector>
#include <unordered_map>

class Evaluator {
private:
    // This is the 'rv' from your notes - the actual memory
    std::vector<double> rv;
    
    // This stores our 'program' (the list of flattened instructions)
    std::vector<Line> program;
    
    // Maps variable names to their specific index in the rv vector
    std::unordered_map<std::string, int> varMap;

public:
    // This is the main engine: it flattens the tree then runs the loop
    void run(std::shared_ptr<StatementsNode> root);

private:
    // Internal method to execute the instructions iteratively
    void execute();
};

#endif // EVALUATOR_H