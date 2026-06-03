#include <iostream>

#include "Debugger/Debugger.h"

int main() {
    try {
        debugger::Debugger debugger;
        debugger.runInteractive();
        debugger.shutdown();
    } catch (const std::exception& ex) {
        std::cerr << "Debugger error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
