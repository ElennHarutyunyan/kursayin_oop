#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "DebuggerTypes.h"
#include "ExecutionEnvironment.h"

namespace debugger {

struct BreakpointSpec {
    int id = 0;
    std::optional<uint32_t> address;
    std::optional<std::string> functionName;
    std::optional<uint32_t> offset;
    std::optional<std::string> condition;
};

class BreakpointManager {
public:
    int add(const BreakpointSpec& spec, const LoadedProgram& program);
    bool remove(int id);
    void clear();
    void list() const;
    bool shouldBreak(uint32_t ip, const ExecutionEnvironment& env) const;

private:
    std::vector<BreakpointSpec> breakpoints;
    int nextId = 1;

    static std::optional<uint32_t> resolveAddress(const BreakpointSpec& spec, const LoadedProgram& program);
    static bool evaluateCondition(const std::string& condition, const ExecutionEnvironment& env);
    static int registerIndexFromToken(const std::string& token);
};

}  // namespace debugger
