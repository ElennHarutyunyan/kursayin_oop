#include "SemanticAnalyzer.h"

#include <stdexcept>

namespace frontend {

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<ASTNode>>& ast) const {
    bool hasMain = false;
    for (const auto& node : ast) {
        if (auto* fn = dynamic_cast<FunctionNode*>(node.get())) {
            if (fn->name == "main") {
                hasMain = true;
            }
        }
    }
    if (!hasMain) {
        throw std::runtime_error("Program must define main()");
    }
}

}  // namespace frontend
