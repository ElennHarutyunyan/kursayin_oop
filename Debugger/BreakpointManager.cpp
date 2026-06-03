#include "BreakpointManager.h"
#include <optional>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

namespace debugger {

namespace {

std::string trim(const std::string& value) {
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }
    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }
    return value.substr(start, end - start);
}

}  // namespace

int BreakpointManager::add(const BreakpointSpec& spec, const LoadedProgram& program) {
    BreakpointSpec copy = spec;
    copy.id = nextId++;
    breakpoints.push_back(copy);
    return copy.id;
}

bool BreakpointManager::remove(int id) {
    const auto it = std::find_if(breakpoints.begin(), breakpoints.end(),
                                 [&](const BreakpointSpec& bp) { return bp.id == id; });
    if (it == breakpoints.end()) {
        return false;
    }
    breakpoints.erase(it);
    return true;
}

void BreakpointManager::clear() {
    breakpoints.clear();
}

void BreakpointManager::list() const {
    if (breakpoints.empty()) {
        std::cout << "No breakpoints set\n";
        return;
    }
    for (const auto& bp : breakpoints) {
        std::cout << "#" << bp.id;
        if (bp.address.has_value()) {
            std::cout << " addr=0x" << std::hex << *bp.address << std::dec;
        }
        if (bp.functionName.has_value()) {
            std::cout << " func=" << *bp.functionName;
        }
        if (bp.offset.has_value()) {
            std::cout << " offset=" << *bp.offset;
        }
        if (bp.condition.has_value()) {
            std::cout << " if " << *bp.condition;
        }
        std::cout << "\n";
    }
}

bool BreakpointManager::shouldBreak(uint32_t ip, const ExecutionEnvironment& env) const {
    for (const auto& bp : breakpoints) {
        const auto resolved = resolveAddress(bp, env.getProgram());
        if (resolved.has_value() && *resolved == ip) {
            if (!bp.condition.has_value() || evaluateCondition(*bp.condition, env)) {
                return true;
            }
        }
    }
    return false;
}

std::optional<uint32_t> BreakpointManager::resolveAddress(const BreakpointSpec& spec,
                                                          const LoadedProgram& program) {
    if (spec.address.has_value()) {
        return spec.address;
    }
    if (!spec.functionName.has_value()) {
        return std::nullopt;
    }
    for (const auto& symbol : program.symbols) {
        if (symbol.name == *spec.functionName) {
            return symbol.address + spec.offset.value_or(0);
        }
    }
    return std::nullopt;
}

bool BreakpointManager::evaluateCondition(const std::string& condition, const ExecutionEnvironment& env) {
    std::istringstream input(condition);
    std::string left;
    std::string op;
    int32_t rhs = 0;
    input >> left >> op >> rhs;
    if (!input) {
        return false;
    }
    const int reg = registerIndexFromToken(left);
    if (reg < 0) {
        return false;
    }
    const int32_t value = env.getCpu().getReg(reg);
    if (op == "==") return value == rhs;
    if (op == "!=") return value != rhs;
    if (op == "<") return value < rhs;
    if (op == "<=") return value <= rhs;
    if (op == ">") return value > rhs;
    if (op == ">=") return value >= rhs;
    return false;
}

int BreakpointManager::registerIndexFromToken(const std::string& token) {
    const std::string cleaned = trim(token);
    if (cleaned.size() > 1 && cleaned[0] == 'x') {
        return std::stoi(cleaned.substr(1));
    }
    for (int i = 0; i < 32; ++i) {
        if (cleaned == RegNames[i]) {
            return i;
        }
    }
    return -1;
}

}  // namespace debugger
