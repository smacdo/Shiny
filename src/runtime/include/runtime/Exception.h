#pragma once
#include <stdexcept>

#define EXCEPTION_CALLSITE_PARAMS                                              \
  const char *function, const char *fileName, int lineNumber
#define EXCEPTION_CALLSITE_ARGS __func__, __FILE__, __LINE__
#define EXCEPTION_INIT_BASE_ARGS function, fileName, lineNumber

namespace Shiny {
  /** Base exception class for all Shiny exceptions. */
  class Exception : public std::runtime_error {
  public:
    /** Constructor. */
    Exception(std::string message, EXCEPTION_CALLSITE_PARAMS);

    /** Get message provided by user. */
    std::string message() const { return message_; }

    /** Get the function name that threw the exception. */
    const char* function() const { return function_; }

    /** Get the name of the file where the exception was thrown. */
    const char* fileName() const { return fileName_; }

    /** Get the line number in the file where the exception was thrown. */
    int lineNumber() const { return lineNumber_; }

  private:
    static std::string format(
        const std::string& message,
        const char* function,
        const char* fileName,
        int lineNumber);

  private:
    std::string message_;
    const char* function_ = nullptr;
    const char* fileName_ = nullptr;
    int lineNumber_ = 0;
  };
} // namespace Shiny