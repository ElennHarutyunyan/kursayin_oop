#include <iostream>
#include <vector>
#include <string>

#include "Compiler/SymbolTable.h"
#include "src/backend/BackendPipeline.h"
#include "src/frontend/FrontendPipeline.h"
#include "src/linker/ExecutableFormat.h"
#include "src/runtime/VmMonitor.h"

int main() {
    try {
        std::string source =
            "int g = 5; "
            "int main() { "
            "int acc = 0; "
            "for(int i = 0; i < 4; i = i + 1) { "
            "static int s = i + 1; "
            "acc = acc + s; "
            "} "
            "g = g + 1; "
            "acc = acc + g; "
            "return acc; "
            "}";
        std::cout << "Compiling source: " << source << "\n";

        // 1) Frontend: Parser -> AST
        frontend::FrontendPipeline frontend;
        auto frontendResult = frontend.compileToAst(source);

        // 2) AST optimizer stage (placeholder hook for project requirements)
        backend::AstOptimizer astOptimizer;
        astOptimizer.optimize(frontendResult.ast);

        // 3) IR translation
        SymbolTable st;

        backend::BackendPipeline backend;
        auto ir = backend.lowerToLogicalIr(frontendResult.ast, st);

        // 4) IR optimizer stage (placeholder hook for project requirements)
        backend::IrOptimizer irOptimizer;
        irOptimizer.optimize(ir);

        // 5) Final program image
        std::vector<Instruction> program = ir.instructions;
        program.push_back({OpCode::HALT, 0, 0, 0, 0});

        linker_stage::ToolchainLinker linker;
        auto image = linker.linkToImage(program, ir.dataWords);
        linker.writeExecutable(image, "a.out.exe");

        // 6) VM Monitor runtime
        runtime::VmMonitor monitor(65536);
        std::cout << "--- Executing Generated Code ---\n";
        monitor.run(program, ir.dataWords, static_cast<uint32_t>(ir.dataBaseAddress));
        monitor.dumpRegisters();
        std::cout << "return value (a0): " << monitor.readRegister(10) << " (expected 10)\n";

    } catch (const std::exception& e) {
        std::cerr << "Error during compilation/execution: " << e.what() << "\n";
        return 1;
    }

    return 0;
}