#include "CliController.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "ProgramLoader.h"

namespace debugger {

CliController::CliController(ExecutionEnvironment& env, BreakpointManager& breakpoints)
    : environment(env), breakpointManager(breakpoints) {}

std::vector<std::string> CliController::tokenize(const std::string& line) {
    std::istringstream input(line);
    std::vector<std::string> tokens;
    std::string token;
    while (input >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::function<bool(uint32_t)> CliController::makeBreakPredicate() const {
    return [this](uint32_t ip) { return breakpointManager.shouldBreak(ip, environment); };
}

void CliController::runLoop() {
    std::cout << "Debugger ready. Type 'help' for commands.\n";
    std::string line;
    while (!shouldQuit && std::cout.good()) {
        std::cout << "(dbg) ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        if (line.empty()) {
            continue;
        }
        handleLine(line);
    }
}

bool CliController::handleLine(const std::string& line) {
    const auto tokens = tokenize(line);
    if (tokens.empty()) {
        return true;
    }

    const std::string& cmd = tokens[0];
    if (cmd == "quit" || cmd == "q") {
        shouldQuit = true;
        return false;
    }
    if (cmd == "help") {
        std::cout << "Commands:\n"
                     "  load -f <path>\n"
                     "  print -mode functions | print IP\n"
                     "  go | go debug | step | step over | step out\n"
                     "  br | br.add ... | br.rem <id>\n"
                     "  quit\n";
        return true;
    }
    if (cmd == "load") {
        return handleLoad(tokens);
    }
    if (cmd == "print") {
        return handlePrint(tokens);
    }
    if (cmd == "go" && tokens.size() >= 2 && tokens[1] == "debug") {
        environment.run(ExecutionMode::RunDebug, makeBreakPredicate());
        return true;
    }
    if (cmd == "go") {
        environment.run(ExecutionMode::Run, makeBreakPredicate());
        return true;
    }
    if (cmd == "step" && tokens.size() >= 2 && tokens[1] == "over") {
        environment.stepOver(makeBreakPredicate());
        environment.getCpu().dumpRegisters();
        return true;
    }
    if (cmd == "step" && tokens.size() >= 2 && tokens[1] == "out") {
        environment.stepOut(makeBreakPredicate());
        environment.getCpu().dumpRegisters();
        return true;
    }
    if (cmd == "step") {
        environment.run(ExecutionMode::RunSingle, makeBreakPredicate());
        environment.getCpu().dumpRegisters();
        return true;
    }
    if (cmd == "br" || cmd == "br.add" || cmd == "br.rem") {
        return handleBreakpoint(tokens);
    }

    std::cerr << "Unknown command: " << cmd << "\n";
    return true;
}

bool CliController::handleLoad(const std::vector<std::string>& tokens) {
    std::string path;
    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == "-f" && i + 1 < tokens.size()) {
            path = tokens[++i];
        }
    }
    if (path.empty()) {
        std::cerr << "Usage: load -f <path>\n";
        return true;
    }
    try {
        LoadedProgram program = ProgramLoader::loadIntoEnvironment(path);
        environment.initialize(program);
        std::cout << "Loaded " << path << " (" << program.text.size() << " instructions)\n";
    } catch (const std::exception& ex) {
        std::cerr << "Load failed: " << ex.what() << "\n";
    }
    return true;
}

bool CliController::handlePrint(const std::vector<std::string>& tokens) {
    if (tokens.size() >= 3 && tokens[1] == "-mode" && tokens[2] == "functions") {
        for (const auto& symbol : environment.getProgram().symbols) {
            std::cout << symbol.name << " @ IP = 0x" << std::hex << symbol.address << std::dec << "\n";
        }
        return true;
    }
    if (tokens.size() >= 2 && tokens[1] == "IP") {
        const auto& cpu = environment.getCpu();
        std::cout << "Instruction Pointer (byte): 0x" << std::hex << (environment.getIp() * 4) << std::dec << "\n";
        cpu.dumpRegisters();
        std::cout << "Stack frame: SP=0x" << std::hex << cpu.getStackPointer()
                  << " BP=0x" << cpu.getBasePointer() << std::dec << "\n";
        auto& mem = environment.getMemory();
        std::cout << "Code segment:\n";
        mem.hexDump(mem.getCodeBase(), 64);
        std::cout << "Data segment:\n";
        mem.hexDump(mem.getStaticBase(), 64);
        return true;
    }
    std::cerr << "Usage: print -mode functions | print IP\n";
    return true;
}

bool CliController::handleBreakpoint(const std::vector<std::string>& tokens) {
    if (tokens[0] == "br" && tokens.size() == 1) {
        breakpointManager.list();
        return true;
    }
    if (tokens[0] == "br.rem") {
        if (tokens.size() < 2) {
            std::cerr << "Usage: br.rem <id>\n";
            return true;
        }
        const int id = std::stoi(tokens[1]);
        if (!breakpointManager.remove(id)) {
            std::cerr << "Breakpoint not found: " << id << "\n";
        }
        return true;
    }
    if (tokens[0] == "br.add") {
        BreakpointSpec spec;
        for (size_t i = 1; i < tokens.size(); ++i) {
            if (tokens[i] == "-func" && i + 1 < tokens.size()) {
                spec.functionName = tokens[++i];
            } else if (tokens[i] == "-offset" && i + 1 < tokens.size()) {
                spec.offset = static_cast<uint32_t>(std::stoul(tokens[++i]));
            } else if (tokens[i].rfind("0x", 0) == 0) {
                spec.address = static_cast<uint32_t>(std::stoul(tokens[i], nullptr, 16));
            } else if (tokens[i].find("==") != std::string::npos || tokens[i].find("!=") != std::string::npos) {
                spec.condition = tokens[i];
                if (i + 2 < tokens.size()) {
                    spec.condition = tokens[i] + " " + tokens[i + 1] + " " + tokens[i + 2];
                    i += 2;
                }
            }
        }
        const int id = breakpointManager.add(spec, environment.getProgram());
        std::cout << "Breakpoint added: #" << id << "\n";
        return true;
    }
    std::cerr << "Usage: br | br.add [-func name] [-offset N] | br.rem <id>\n";
    return true;
}

}  // namespace debugger
