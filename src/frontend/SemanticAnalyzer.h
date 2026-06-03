#pragma once

#include <memory>
#include <vector>

#include "../../Compiler/ASTNodes.h"

namespace frontend {

class SemanticAnalyzer {
public:
    void analyze(const std::vector<std::unique_ptr<ASTNode>>& ast) const;
};

}  // namespace frontend
