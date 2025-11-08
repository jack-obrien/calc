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
 * Advance pos to the end of the double. Place the result into double_result.
 *
 * Returns nonzero if the input is not a valid double.
 */
int parse_double(char** pos, double* double_result) {
  char* endptr;
  double parsed = strtod(*pos, &endptr);

  // in case parsing fails, strtod will store string starting address in endptr
  if (endptr == *pos) {
    return 1;
  }

  *double_result = parsed;
  *pos = endptr;
  return 0;
}

int parse_operator(char** pos, char* operator_result) {
  if (**pos == '+' || **pos == '-') {
    *operator_result = **pos;
    (*pos)++;
    return 0;
  } else {
    return 1;
  }
}

/*
 * Read user input, parse it, and place the resulting CalcCommand into cmd.
 * Return 0 in case of success, 1 in case of failure.
 *
 * Args:
 *  CalcCommand* cmd: buffer to put result into.
 */
int read_calc_input(CalcCommand* cmd) {
  printf("%s", "calc > ");

  // Read user input into a 256 byte buffer
  char expr[256];
  fgets(expr, sizeof(expr), stdin);

  // Here we use pointer-to-pointer to track our progress parsing expr.
  char* pos = expr;

  // error is a return code, any nonzero value will cause error after parsing.
  int error = 0;

  error |= parse_whitespace(&pos);
  error |= parse_double(&pos, &cmd->left);
  error |= parse_whitespace(&pos);
  error |= parse_operator(&pos, &cmd->op);
  error |= parse_whitespace(&pos);
  error |= parse_double(&pos, &cmd->right);

  if (error) {
    printf("Parsing error\n");
    return 1;
  }

  return 0;
}

int main(void) {
  CalcCommand cmd;
  while (read_calc_input(&cmd)) {}

  printf("Parsed value: %f %c %f\n", cmd.left, cmd.op, cmd.right);
}
