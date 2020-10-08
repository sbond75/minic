#include "mpc.h"

void run(char* grammar) {
mpc_parser_t* program = mpc_new("program");
mpc_parser_t* top_level_stmt = mpc_new("top_level_stmt");
mpc_parser_t* decl = mpc_new("decl");
mpc_parser_t* function = mpc_new("function");
mpc_parser_t* type = mpc_new("type");
mpc_parser_t* id = mpc_new("id");
mpc_parser_t* block = mpc_new("block");
mpc_parser_t* stmt = mpc_new("stmt");
mpc_parser_t* expr = mpc_new("expr");
mpc_parser_t* binary_op = mpc_new("binary_op");
mpc_parser_t* letter = mpc_new("letter");
mpc_parser_t* number = mpc_new("number");
mpc_parser_t* char_ = mpc_new("char");
mpc_parser_t* string = mpc_new("string");
mpc_parser_t* int_ = mpc_new("int");
mpc_parser_t* float_ = mpc_new("float");
mpc_parser_t* digit = mpc_new("digit");
mpc_parser_t* ch = mpc_new("ch");


  mpca_lang(MPCA_LANG_DEFAULT,
	    grammar,
	   "program", "top_level_stmt", "decl", "function", "type", "id", "block", "stmt", "expr", "binary_op", "letter", "number", "char", "string", "int", "float", "digit", "ch", NULL);
  
  const char* input = "(4 * 2 * 11 + 2) - 5";

  mpc_result_t r;

  if (mpc_parse("input", input, program, &r)) {
    mpc_ast_print(r.output);
    mpc_ast_delete(r.output);
  } else {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
  }

  mpc_cleanup(18, program, top_level_stmt, decl, function, type, id, block, stmt, expr, binary_op, letter, number, char_, string, int_, float_, digit, ch);
}

int main() {
  if (!withContents("grammar.txt", run)) {
    perror("Failed to read grammar.txt");
    return 1;
  }

  return 0;
}
