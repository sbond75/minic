all: minic

CC := clang
CXX := clang++
SRC := src
OBJ := obj

CFLAGS = -g3
# https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl03.html
CXXFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs core` $(NIX_CFLAGS_COMPILE) -std=c++14

LLVM_LDFLAGS = -lLLVMCore -lLLVMSupport -lLLVMRuntimeDyld -lLLVMExecutionEngine -lLLVMCodeGen -lLLVMObjCARCOpts -lLLVMScalarOpts -lLLVMInstCombine -lLLVMTransformUtils -lLLVMAnalysis -lLLVMTarget -lLLVMMC -lLLVMObject
LDFLAGS=$(NIX_LDFLAGS) $(llvm-config --ldflags --libs --system-libs) $(LLVM_LDFLAGS) -stdlib=libc++ -lz -lpthread -lncurses -lm
export LLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING := 1

export MACOSX_DEPLOYMENT_TARGET = 10.15

C_SOURCES := $(wildcard $(SRC)/*.c)
CPP_SOURCES := $(wildcard $(SRC)/*.cpp)
C_OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(C_SOURCES))
CPP_OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(CPP_SOURCES))
OBJECTS := $(C_OBJECTS) $(CPP_OBJECTS)

minic: $(OBJECTS)
	$(CXX) $^ -o $@ $(LIBS) $(LDFLAGS) $(CXXFLAGS)

# C files
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@
# C++ files
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC) -c $< -o $@

clean:
	rm -f $(OBJECTS) minic
