CXX := g++
CXXSTD ?= c++17
CXXFLAGS := -std=$(CXXSTD) -I.

COMMON_SOURCES := \
	Compiler/Lexer.cpp \
	Compiler/Parser.cpp \
	Compiler/CodeGenerator.cpp \
	Compiler/SymbolTable.cpp \
	Linker/Linker.cpp \
	Vm/VirtualMachine.cpp \
	Vm/Loader.cpp \
	Vm/Memory.cpp \
	src/frontend/FrontendPipeline.cpp \
	src/frontend/SemanticAnalyzer.cpp \
	src/backend/BackendPipeline.cpp \
	src/linker/ExecutableFormat.cpp \
	src/runtime/VmMonitor.cpp

DEBUGGER_SOURCES := \
	Debugger/ProgramLoader.cpp \
	Debugger/ExecutionEnvironment.cpp \
	Debugger/BreakpointManager.cpp \
	Debugger/CliController.cpp \
	Debugger/Debugger.cpp \
	Debugger/Hdd.cpp \
	Debugger/Dma.cpp

.PHONY: all build debugger test test-debugger clean

all: build debugger

build:
	$(CXX) $(CXXFLAGS) main.cpp $(COMMON_SOURCES) -o my_compiler

debugger:
	$(CXX) $(CXXFLAGS) debugger_main.cpp $(DEBUGGER_SOURCES) $(COMMON_SOURCES) -o my_debugger

test: build
	bash ./run_all_tests.sh ./my_compiler ./test/manifest.txt
	bash ./run_all_tests.sh ./my_compiler ./test/manifest/core.txt
	bash ./run_all_tests.sh ./my_compiler ./test/manifest/features.txt

test-debugger: debugger build
	bash ./test/debugger/run_debugger_tests.sh

clean:
	rm -f my_compiler my_debugger
