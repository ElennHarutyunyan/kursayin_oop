#include "Debugger.h"

namespace debugger {

Debugger::Debugger() : cli(environment, breakpointManager) {}

void Debugger::runInteractive() {
    cli.runLoop();
}

void Debugger::shutdown() {
    breakpointManager.clear();
    environment.reset();
}

}  // namespace debugger
