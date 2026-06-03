#pragma once

#include <functional>

#include "BreakpointManager.h"
#include "ExecutionEnvironment.h"

namespace debugger {

class CliController {
public:
    CliController(ExecutionEnvironment& env, BreakpointManager& breakpoints);

    void runLoop();
    bool handleLine(const std::string& line);

private:
    ExecutionEnvironment& environment;
    BreakpointManager& breakpointManager;
    bool shouldQuit = false;

    bool handleLoad(const std::vector<std::string>& tokens);
    bool handlePrint(const std::vector<std::string>& tokens);
    bool handleBreakpoint(const std::vector<std::string>& tokens);
    static std::vector<std::string> tokenize(const std::string& line);
    std::function<bool(uint32_t)> makeBreakPredicate() const;
};

}  // namespace debugger
