#include "FrontendPipeline.h"

#include "../../Compiler/Lexer.h"
#include "../../Compiler/Parser.h"
#include "SemanticAnalyzer.h"

namespace frontend {

FrontendResult FrontendPipeline::compileToAst(const std::string& source) const {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    FrontendResult result;
    result.ast = parser.parse();

    SemanticAnalyzer semantics;
    semantics.analyze(result.ast);
    return result;
}

}  // namespace frontend
