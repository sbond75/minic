// Based on https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl03.html

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

// "The codegen() method says to emit IR for that AST node along with all the things it depends on,
// and they all return an LLVM Value object. “Value” is the class used to represent a “Static Single
// Assignment (SSA) [ http://en.wikipedia.org/wiki/Static_single_assignment_form ] register” or “SSA
// value” in LLVM. The most distinct aspect of SSA values is that their value is computed as the
// related instruction executes, and it does not get a new value until (and if) the instruction
// re-executes. In other words, there is no way to “change” an SSA value. For more information,
// please read up on Static Single Assignment [
// http://en.wikipedia.org/wiki/Static_single_assignment_form ] - the concepts are really quite
// natural once you grok them."

// https://en.wikipedia.org/wiki/Static_single_assignment_form : "In compiler design, static single
// assignment form (often abbreviated as SSA form or simply SSA) is a property of an intermediate
// representation (IR), which requires that each variable be assigned exactly once, and every
// variable be defined before it is used. Existing variables in the original IR are split into
// versions, new variables typically indicated by the original name with a subscript in textbooks,
// so that every definition gets its own version. In SSA form, use-def chains are explicit and each
// contains a single element." ... "One can expect to find SSA in a compiler for Fortran, C or C++,
// whereas in functional language compilers, such as those for Scheme and ML, continuation-passing
// style (CPS) is generally used. SSA is formally equivalent to a well-behaved subset of CPS
// excluding non-local control flow, which does not occur when CPS is used as intermediate
// representation.[3] So optimizations and transformations formulated in terms of one immediately
// apply to the other."

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
  virtual ~ExprAST() {}
  virtual llvm::Value *codegen() = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
  double Val;

public:
  NumberExprAST(double Val) : Val(Val) {}
  virtual llvm::Value *codegen();
};

// "The static variables will be used during code generation. TheContext is an opaque object that
// owns a lot of core LLVM data structures, such as the type and constant value tables. We don’t
// need to understand it in detail, we just need a single instance to pass into APIs that require
// it.
static llvm::LLVMContext TheContext;
// "The Builder object is a helper object that makes it easy to generate LLVM
// instructions. Instances of the IRBuilder class template keep track of the current place to insert
// instructions and has methods to create new instructions."
// It also "generate[s] code into something."
static llvm::IRBuilder<> Builder(TheContext);
// "TheModule is an LLVM construct that contains functions and global variables. In many ways, it is
// the top-level structure that the LLVM IR uses to contain code. It will own the memory for all of
// the IR that we generate, which is why the codegen() method returns a raw Value*, rather than a
// unique_ptr<Value>."
static std::unique_ptr<llvm::Module> TheModule;
// "The NamedValues map keeps track of which values are defined in the current scope and what their
// LLVM representation is. (In other words, it is a symbol table for the code). In this form of
// Kaleidoscope, the only things that can be referenced are function parameters. As such, function
// parameters will be in this map when generating code for their function body."
static std::map<std::string, llvm::Value *> NamedValues;

llvm::Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

// "Generating LLVM code for expression nodes is very straightforward: less than 45 lines of
// commented code for all four of our expression nodes. First we’ll do numeric literals:"
llvm::Value *NumberExprAST::codegen() {
  return llvm::ConstantFP::get(TheContext, llvm::APFloat(Val));
}
// "In the LLVM IR, numeric constants are represented with the ConstantFP class, which holds the
// numeric value in an APFloat internally (APFloat has the capability of holding floating point
// constants of Arbitrary Precision). This code basically just creates and returns a
// ConstantFP. Note that in the LLVM IR that constants are all uniqued together and shared. For this
// reason, the API uses the “foo::get(…)” idiom instead of “new foo(..)” or “foo::Create(..)”.

// From https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html //

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
  std::string Name;

public:
  VariableExprAST(const std::string &Name) : Name(Name) {}
  virtual llvm::Value *codegen();
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                std::unique_ptr<ExprAST> RHS)
    : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
  virtual llvm::Value *codegen();
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
              std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
  virtual llvm::Value *codegen();
};

// //

llvm::Value *VariableExprAST::codegen() {
  // Look this variable up in the function.
  llvm::Value *V = NamedValues[Name];
  if (!V)
    LogErrorV("Unknown variable name");
  return V;
}

// "References to variables are also quite simple using LLVM. In the simple version of Kaleidoscope,
// we assume that the variable has already been emitted somewhere and its value is available. In
// practice, the only values that can be in the NamedValues map are function arguments. This code
// simply checks to see that the specified name is in the map (if not, an unknown variable is being
// referenced) and returns the value for it. In future chapters, we’ll add support for loop
// induction variables in the symbol table, and for local variables."
llvm::Value *BinaryExprAST::codegen() {
  llvm::Value *L = LHS->codegen();
  llvm::Value *R = RHS->codegen();
  if (!L || !R)
    return nullptr;

  switch (Op) {
  case '+':
    return Builder.CreateFAdd(L, R, "addtmp");
  case '-':
    return Builder.CreateFSub(L, R, "subtmp");
  case '*':
    return Builder.CreateFMul(L, R, "multmp");
  case '<':
    L = Builder.CreateFCmpULT(L, R, "cmptmp");
    // Convert bool 0/1 to double 0.0 or 1.0
    return Builder.CreateUIToFP(L, llvm::Type::getDoubleTy(TheContext),
                                "booltmp");
  default:
    return LogErrorV("invalid binary operator");
  }
}
// "This is all (intentionally) rather straight-forward: variables capture the variable name, binary
// operators capture their opcode (e.g. ‘+’), and calls capture a function name as well as a list of
// any argument expressions. One thing that is nice about our AST is that it captures the language
// features without talking about the syntax of the language. Note that there is no discussion about
// precedence of binary operators, lexical structure, etc."

// "For our basic language, these are all of the expression nodes we’ll define. Because it doesn’t
// have conditional control flow, it isn’t Turing-complete; we’ll fix that in a later
// installment. The two things we need next are a way to talk about the interface to a function, and
// a way to talk about functions themselves:"
/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> Args)
    : Name(name), Args(std::move(Args)) {}

  const std::string &getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
              std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
};
// "In Kaleidoscope, functions are typed with just a count of their arguments. Since all values are
// double precision floating point, the type of each argument doesn’t need to be stored anywhere. In
// a more aggressive and realistic language, the “ExprAST” class would probably have a type field."

// "With this scaffolding, we can now talk about parsing expressions and function bodies in
// Kaleidoscope."
