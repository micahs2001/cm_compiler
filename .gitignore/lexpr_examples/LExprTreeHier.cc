/*
  Simple Lisp expression parser
    by Gary M. Zoppetti

  Produces an AST that uses a Node IH, and evaluates it.
  No visitors.

  lexp     -> Number | ( op lexp-seq )
  lexp-seq -> lexp { lexp }
*/

/*********************************************************************/

#include <cctype>
#include <cstdio>
#include <cstdlib>

#include <vector>

/*********************************************************************/

// Global token variable
int g_token;

/*********************************************************************/

char
getToken ();

void
error (const char* error);

void
match (char expectedToken);

/*********************************************************************/

struct Node
{
  virtual ~Node () {}

  virtual int
  evaluate () = 0;
};

struct OperatorNode : Node
{
  OperatorNode (char op)
      : operation (op)
  {
  }
  virtual ~OperatorNode () {}

  virtual int
  evaluate ()
  {
    int result = children[0]->evaluate ();
    size_t childNum = 1;
    for (; childNum < children.size (); ++childNum)
    {
      Node* child = children[childNum];
      switch (operation)
      {
        case '+':
          result += child->evaluate ();
          break;
        case '-':
          result -= child->evaluate ();
          break;
        case '*':
          result *= child->evaluate ();
          break;
      }
    }
    if (childNum == 1 && operation == '-')
      result = -result;

    return result;
  }

  char operation;
  std::vector<Node*> children;
};

struct NumberNode : Node
{
  NumberNode (int num)
      : number (num)
  {
  }
  virtual ~NumberNode () {}

  virtual int
  evaluate ()
  {
    return number;
  }

  int number;
};

/*********************************************************************/

Node*
lexp ();

Node*
lexpSeq (char op);

/*********************************************************************/

int
main ()
{
  g_token = getToken ();

  Node* tree = lexp ();
  if (g_token == '$')
  {
    int result = tree->evaluate ();
    printf ("Result = %d\n", result);
  }
  else
    error ("main");

  return EXIT_SUCCESS;
}

/*********************************************************************/

char
getToken ()
{
  char ch;
  while (isspace (ch = getchar ()))
    ;

  return ch;
}

/*********************************************************************/

void
error (const char* error)
{
  fprintf (stderr, "Error: %s\n", error);
  exit (1);
}

/*********************************************************************/

void
match (char expectedToken)
{
  if (g_token == expectedToken)
    g_token = getToken ();
  else
    error ("match");
}

/*********************************************************************/
// lexp -> Number | ( op lexp-seq )

Node*
lexp ()
{
  Node* tree;
  if (isdigit (g_token))
  {
    ungetc (g_token, stdin);
    int num;
    scanf ("%d", &num);
    tree = new NumberNode (num);
    g_token = getToken ();
  }
  else if (g_token == '(')
  {
    match ('(');
    switch (g_token)
    {
      case '+':
        match (g_token);
        tree = lexpSeq ('+');
        break;
      case '-':
        match (g_token);
        tree = lexpSeq ('-');
        break;
      case '*':
        match (g_token);
        tree = lexpSeq ('*');
        break;

      default:
        error ("lexp: expected token in { +, -, * }");
    }
    match (')');
  }
  else
    error ("lexp");

  return tree;
}

/*********************************************************************/
// lexp-seq -> lexp { lexp }

Node*
lexpSeq (char op)
{
  OperatorNode* opNode = new OperatorNode (op);
  do
  {
    Node* arg = lexp ();
    opNode->children.push_back (arg);
  } while (isdigit (g_token) || g_token == '(');

  return opNode;
}
