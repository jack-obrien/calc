#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Operation for the calculator to do. Recursive structure to capture
 * precedence. Can hold either an operation and two children, or a double value.
 *
 */
typedef struct node {
  struct node* left; // children must be NULL if leaf node
  struct node* right;
  char op;      // op value only meaningful if not leaf node
  double value; // value only meaningful if leaf node
} node;

/*
 * Returns pointer to new uninitialised node.
 */
node* create_node() {
  node* n = malloc(sizeof(node));
  n->left = NULL;
  n->right = NULL;
  n->op = 0;
  n->value = 0.0;
  return n;
}

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
  if (**pos == '+' || **pos == '-' || **pos == '*' || **pos == '/') {
    *operator_result = **pos;
    (*pos)++;
    return 0;
  } else {
    return 1;
  }
}

/*
 * Read user input, parse it, and place the resulting node into cmd.
 * Return 0 in case of success, 1 in case of failure.
 *
 * Args:
 *  node* cmd: buffer to put result into.
 */
int read_calc_input(node* root) {

  // Read user input into a 256 byte buffer
  char expr[256];
  fgets(expr, sizeof(expr), stdin);

  // Here we use pointer-to-pointer to track our progress parsing expr.
  char* pos = expr;

  // error is a return code, any nonzero value will cause error after parsing.
  int error = 0;

  error |= parse_whitespace(&pos);
  error |= parse_double(&pos, &root->left->value);
  error |= parse_whitespace(&pos);
  error |= parse_operator(&pos, &root->op);
  error |= parse_whitespace(&pos);
  error |= parse_double(&pos, &root->right->value);

  if (error) {
    printf("Parsing error\n");
    return 1;
  }

  return 0;
}

double do_calc_command(node* cmd) {
  if (cmd->op == '+') {
    return cmd->left->value + cmd->right->value;
  } else if (cmd->op == '-') {
    return cmd->left->value - cmd->right->value;
  } else if (cmd->op == '*') {
    return cmd->left->value * cmd->right->value;
  } else if (cmd->op == '/') {
    return cmd->left->value / cmd->right->value;
  } else {
    return 0;
  }
}

int main(void) {
  // REPL
  while (1) {
    node left = {0};
    node right = {0};
    node* root = create_node();
    root->left = &left;
    root->right = &right;
    do {
      printf("%s", "calc > ");
    } while (read_calc_input(root));

    double result = do_calc_command(root);
    printf("%f\n", result);
  }
}
