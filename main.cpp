#include <iostream>
#include <vector>
#include <string>

#include "Compiler/Lexer.h"
#include "Compiler/Parser.h"
#include "Compiler/CodeGenerator.h"
#include "Compiler/SymbolTable.h"
#include "Vm/VirtualMachine.h"
#include "Vm/Memory.h"

int main() {
    try {
        // 1. Մուտքային կոդ (Source Code)
        // Կարող ես փոխել սա և թեստավորել տարբեր արտահայտություններ
        std::string source = "x = 10 + 20; y = x + 5;";
        std::cout << "Compiling source: " << source << std::endl;

        // 2. Lexical Analysis (Tokens)
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        // 3. Parsing (AST)
        Parser parser(tokens);
        auto astRoot = parser.parse(); 

        // 4. Code Generation (RISC-V)
        SymbolTable st;
        // Նախապես ավելացնենք x և y փոփոխականները, որպեսզի Generator-ը գտնի դրանք
        st.addSymbol("x", "int", SymbolType::Local);
        st.addSymbol("y", "int", SymbolType::Local);

        CodeGenerator gen(st);
        for (auto& node : astRoot) {
            gen.generate(node.get());
        }

        std::vector<Instruction> program = gen.getResult();
        program.push_back({OpCode::HALT, 0, 0, 0, 0}); // Միշտ ավելացրու HALT

        std::cout << "Generated " << program.size() << " instructions." << std::endl;

        // 5. Execution
        Memory mem(65536);
        VirtualMachine vm(mem);
        
        std::cout << "--- Executing Generated Code ---" << std::endl;
        vm.run(program);

        // 6. Արդյունքների դամփ
        vm.dumpRegisters();
        
        // Եթե CodeGenerator-ը x-ը պահել է a0-ում (x10), իսկ y-ը a1-ում (x11)
        std::cout << "Final Result Check:" << std::endl;
        std::cout << "x (a0): " << vm.getReg(10) << " (Expected 30)" << std::endl;
        std::cout << "y (a1): " << vm.getReg(11) << " (Expected 35)" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error during compilation/execution: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}