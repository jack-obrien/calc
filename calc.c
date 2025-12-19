#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char* bool_to_printable(bool b) { return b ? "true" : "false"; }

/*
 * Operation for the calculator to do. Recursive structure to capture
 * precedence. Can hold either an operation and two children, or a double value.
 *
 */
typedef struct node {
  struct node* left; // children must be NULL if leaf node
  struct node* right;
  struct node* parent;
  char op;      // op only meaningful if not leaf node
  double value; // value only meaningful if leaf node

  // set to true for non-leaf nodes who have evaluated op on children's values.
  // set to true for leaf nodes
  bool is_evaluated;
} node;

/*
 * Returns pointer to new uninitialised node.
 */
node* create_node() {
  node* n = malloc(sizeof(node));
  n->left = NULL;
  n->right = NULL;
  n->parent = NULL;
  n->op = 0;
  n->value = 0.0;
  n->is_evaluated = false;
  return n;
}

/* Recursively free root
 * Would love to do this iteratively using a morris traversal - O(1) memory,
 * O(N) time, but i can't be assed. I'd love to eventually finish this project.
 */
void free_node(node* root) {
  if (root->left != NULL) {
    free_node(root->left);
  }
  if (root->right != NULL) {
    free_node(root->right);
  }
  free(root);
}

// print n tabs
void print_tabs(uint8_t n) {
  for (int i = 0; i < n; i++) {
    printf("\t");
  }
}

void print_tree(node* node, int tablevel) {
  print_tabs(tablevel);
  printf("Node at %p\n", (void*)node);
  print_tabs(tablevel);
  printf("parent: %p\n", (void*)(node->parent));
  print_tabs(tablevel);
  printf("op: %c\n", node->op);
  print_tabs(tablevel);
  printf("value: %f\n", node->value);
  print_tabs(tablevel);
  printf("is_evaluated: %s\n", bool_to_printable(node->is_evaluated));
  // Print children
  if (node->left != NULL) {
    for (int i = 0; i < tablevel; i++) {
      printf("\t");
    }
    printf("Left nodes:\n");
    print_tree(node->left, tablevel + 1);
  }
  if (node->right != NULL) {
    for (int i = 0; i < tablevel; i++) {
      printf("\t");
    }
    printf("Right nodes:\n");
    print_tree(node->right, tablevel + 1);
  }
}

/*
 * Advance pos past all whitespace.
 *
 * Always returns 0, even at end of string.
 * An end of string should raise an error in another parsing function if
 * applicable.
 */
void advance_past_whitespace(char** pos) {
  while (**pos == ' ' || **pos == '\n') {
    (*pos)++;
  }
}

/*
 * Advance pos to the end of the double. Place the result into double_result.
 *
 * Returns nonzero if the input is not a valid double.
 */
int parse_double(char** pos, double* double_result) {
  char* endptr;
  double parsed = strtod(*pos, &endptr);

  // in case parsing fails, strtod will store string starting address in
  // endptr
  if (endptr == *pos) {
    return 1;
  }

  *double_result = parsed;
  *pos = endptr;
  return 0;
}

/* Parse factor and return root of the factor tree.
 * For now this is just a wrapper around parse_double which deals with the
 * syntax tree structure. In future this function could call further language
 * extensions like parse_power for the higher-precedence exponentiation
 * operation, or parse_negation for the higher-precedence negation of a number
 * (e.g. -5 )
 */
node* parse_factor(char** pos) {
  node* leaf = create_node();
  parse_double(pos, &leaf->value);
  leaf->is_evaluated = true;
  return leaf;
}

/* Parse term and return root of the term tree.
 * This might contain other higher-precedence opertations too like
 * multiplication or powers.
 *
 * formal definition:
 *  term -> factor (('*' | '/') factor)
 */
node* parse_term(char** pos) {
  node* current = parse_factor(pos);
  advance_past_whitespace(pos);

  while (**pos == '*' || **pos == '/') {
    node* parent = create_node();

    parent->op = **pos;
    (*pos)++;
    advance_past_whitespace(pos);

    parent->right = parse_factor(pos);
    parent->right->parent = parent;
    advance_past_whitespace(pos);

    parent->left = current;
    parent->left->parent = parent;
    current = parent;
  }

  return current;
}

/* Parse expression and return root of expression tree
 *
 * formal definition:
 *  expr -> term (('+' | '-') term)*
 */
node* parse_expression(char** pos) {
  node* current = parse_term(pos);
  advance_past_whitespace(pos);

  while (**pos == '+' || **pos == '-') {
    node* parent = create_node();

    parent->op = **pos;
    (*pos)++;
    advance_past_whitespace(pos);

    parent->right = parse_term(pos);
    parent->right->parent = parent;
    advance_past_whitespace(pos);

    parent->left = current;
    parent->left->parent = parent;
    current = parent;
  }

  return current;
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
 * Read user input, parse it, and return the resulting node.
 * Return NULL in case of failure.
 */
node* read_calc_input() {
  node* root = create_node();

  // Read user input into a 256 byte buffer
  char expr[256];
  fgets(expr, sizeof(expr), stdin);

  // Here we use pointer-to-pointer to track our progress parsing expr.
  char* pos = expr;

  // error is a return code, any nonzero value will cause error after parsing.
  int error = 0;

  advance_past_whitespace(&pos);
  root = parse_expression(&pos);
  advance_past_whitespace(&pos);
  error |= *pos != '\0'; // Ensure we have reached end of input string

  if (error) {
    printf("Parsing error\n");
    print_tree(root, 0);
    free_node(root);
    return NULL;
  }

  return root;
}

double eval_operation(char op, double left, double right) {
  if (op == '+') {
    return left + right;
  } else if (op == '-') {
    return left - right;
  } else if (op == '*') {
    return left * right;
  } else if (op == '/') {
    return left / right;
  } else {
    return 0;
  }
}

double eval_calc_tree(node* root) {
  node* n = root;
  while (!(n->is_evaluated)) {
    if (n->left == NULL || n->right == NULL) {
      printf("Error - eval loop passed through leaf node");
    } else if (!(n->left->is_evaluated)) {
      n = n->left;
    } else if (!(n->right->is_evaluated)) {
      n = n->right;
    } else {
      n->value = eval_operation(n->op, n->left->value, n->right->value);
      n->is_evaluated = true;
      if (n->parent != NULL) {
        n = n->parent;
      }
    }
  }

  if (n != root) {
    printf("ERROR: eval loop exited on non-root.");
  }
  return n->value;
}

int main(void) {
  // REPL
  while (1) {
    node* root;
    do {
      printf("%s", "calc > ");
    } while ((root = read_calc_input()) == NULL);
    print_tree(root, 0);
    printf("\n");

    double result = eval_calc_tree(root);
    printf("%f\n", result);
    free_node(root);
  }
}
