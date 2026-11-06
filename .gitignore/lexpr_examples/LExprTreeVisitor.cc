/*
  The Tree Lisperer
  =================
  Simple Lisp expression parser
    by Gary M. Zoppetti

  Produce an AST that uses a Node IH.
  Uses the Visitor design pattern to evaluate the tree.

  lexp     --> number | ( op lexp-seq )
  op       --> + | - | *
  lexp-seq --> lexp { lexp }

  number = digit+
*/

/*********************************************************************/

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>

using std::vector;

/*********************************************************************/

int g_token;

/*********************************************************************/

char
getToken ();

void
error (const char* error);

void
match (char expectedToken);

/*********************************************************************/

// Forward declarations
struct OperatorNode;
struct NumberNode;

// Visitor interface
// Concrete visitors will implement this interface
struct IVisitor
{
  virtual void
  visit (OperatorNode* pOperator) = 0;
  virtual void
  visit (NumberNode* pNumber) = 0;
};

// Abstract base class
// Plain Node's can't be constructed
struct Node
{
  // Base classes should have virtual dtor's
  // All derived classes will also have virtual dtor's
  //   without having to declare them as such
  virtual ~Node () {}

  // A pure virtual function makes a class abstract
  virtual void
  accept (IVisitor* visitor) = 0;
};

struct OperatorNode : Node
{
  OperatorNode (char pOperation)
      : operation (pOperation)
  {
  }

  virtual ~OperatorNode ()
  {
    for (Node* child : children)
      delete child;
  }

  // All "accept" methods have this form!
  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  char operation;
  vector<Node*> children;
};

struct NumberNode : Node
{
  NumberNode (int pNumber)
      : number (pNumber)
  {
  }

  virtual ~NumberNode () {}

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  int number;
};

struct EvaluationVisitor : IVisitor
{
  // Write a "visit" method for each type of Node we need to process
  virtual void
  visit (NumberNode* node)
  {
    result = node->number;
  }

  virtual void
  visit (OperatorNode* node)
  {
    int localResult;
    switch (node->operation)
    {
      case '+':
        localResult = 0;
        for (Node* child : node->children)
        {
          // To process other nodes call the "accept" method!
          child->accept (this);
          localResult += result;
        }
        break;
      case '-':
      {
        node->children[0]->accept (this);
        localResult = result;
        size_t childNum = 1;
        for (; childNum < node->children.size (); ++childNum)
        {
          Node* child = node->children[childNum];
          child->accept (this);
          localResult -= result;
        }
        if (childNum == 1)
          localResult = -localResult;
      }
      break;
      case '*':
        localResult = 1;
        for (Node* child : node->children)
        {
          child->accept (this);
          localResult *= result;
        }
        break;
    }

    result = localResult;
  }

  int result;
};

/*********************************************************************/

Node*
lexp ();

OperatorNode*
op ();

Node*
lexpSeq (OperatorNode* opNode);

/*********************************************************************/

int
main ()
{
  g_token = getToken ();

  Node* tree = lexp ();
  if (g_token == '$')
  {
    EvaluationVisitor visitor;
    tree->accept (&visitor);
    printf ("Result = %d\n", visitor.result);
  }
  else
    error ("main");

  delete tree;

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
// lexp --> number | ( op lexp-seq )

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
    OperatorNode* opNode = op ();
    tree = lexpSeq (opNode);
    match (')');
  }
  else
    error ("lexp: expected number or '('");

  return tree;
}

/*********************************************************************/
// op --> + | - | *

OperatorNode*
op ()
{
  OperatorNode* opNode;
  if (strchr ("+-*", g_token))
  {
    opNode = new OperatorNode (g_token);
    match (g_token);
  }
  else
    error ("op not one of { +, -, * }");

  return opNode;
}

/*********************************************************************/
// lexp-seq --> lexp { lexp }

Node*
lexpSeq (OperatorNode* opNode)
{
  // FIRST(lexp-seq) = FIRST(lexp) = { number, ( }
  do
  {
    Node* arg = lexp ();
    opNode->children.push_back (arg);
  } while (isdigit (g_token) || g_token == '(');

  return opNode;
}
