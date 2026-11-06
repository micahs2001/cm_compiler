#include <stdexcept>

class UndeclaredVarException : public std::runtime_error
{
public:
  // Pass the message to the base class constructor
  explicit UndeclaredVarException (const std::string& message)
      : std::runtime_error (message)
  {
  }
};