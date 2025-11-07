#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Command for the calculator to do. Represents a two-place function with a
 * left arguemnt, right argument, and operation (e.g. +, -).
 */
typedef struct {
  double left;
  double right;
  char op;
} CalcCommand;

/*
 * Advance pos past all whitespace.
 *
 * Always returns 0, even at end of string.
 * An end of string should raise an error in another parsing function if
 * applicable.
 */
int parse_whitespace(char** pos) {
  while (**pos == ' ') {
    (*pos)++;
  }
  return 0;
}

/*
 * Advance pos to the end of the double. Place the result into num_result.
 *
 * Returns nonzero if the input is not a valid double.
 */
int parse_double(char** pos, double* double_result) {
  
}

int parse_operator(char** pos, char* operator_result) {}

/*
 * Read user input, parse it, and place the resulting CalcCommand into cmd.
 * CalcCommand* cmd: buffer to put result into.
 * size_t size: Allocated size of calc command.
 */
void read_calc_input(CalcCommand* cmd) {
  printf("%s", "calc > ");

  // Read user input into a 256 byte buffer
  char expr[256];
  fgets(expr, sizeof(expr), stdin);
  char* pos = expr;

  // Here we use pointer-to-pointer to track our progress parsing expr.
  // error is a return code, any nonzero value will cause error after parsing.
  int error = 0;

  error |= parse_whitespace(&pos);
  error |= parse_num(&pos, &cmd->left);
  error |= parse_whitespace(&pos);
  error |= parse_operator(&pos, &cmd->op);
  error |= parse_whitespace(&pos);
  error |= parse_num(&pos, &cmd->right);
}

int main(void) {
  CalcCommand cmd;
  read_calc_input(&cmd);
}
