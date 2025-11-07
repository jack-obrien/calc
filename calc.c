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

// Parse token into double, exit if it fails.
double parse_num(char* token) {
  char* endptr; // Needed for strtod error checking
  double num = strtod(token, &endptr);

  // endptr == tok (point to the same addr) when input is not a number
  // *endtptr != '/0' (char referecned by endptr is str terminator) when input
  // has trailing letters
  if (endptr == token || *endptr != '\0') {
    printf("%s", "User input is not a number");
    exit(1);
  }

  return num;
}

/*
 * Read user input, parse it, and place the resulting CalcCommand into cmd.
 * CalcCommand* cmd: buffer to put result into.
 * size_t size: Allocated size of calc command.
 */
void read_calc_input(CalcCommand* cmd) {
  printf("%s", "calc > ");

  // Read user input into a 256 byte buffer
  char input[256];
  fgets(input, sizeof(input), stdin);

  char* tok = strtok(input, " ");

  cmd->left = parse_num(tok);
  cmd->op = p; // TODO: Parse symbol?
  cmd->right = parse_num(tok);
}

int main(void) {
  CalcCommand cmd;
  read_calc_input(&cmd);
}
