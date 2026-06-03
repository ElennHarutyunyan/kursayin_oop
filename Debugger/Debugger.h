#pragma once

#include "BreakpointManager.h"
#include "CliController.h"
#include "ExecutionEnvironment.h"

namespace debugger {

class Debugger {
public:
    Debugger();

    void runInteractive();
    void shutdown();

private:
    ExecutionEnvironment environment;
    BreakpointManager breakpointManager;
    CliController cli;
};

}  // namespace debugger
