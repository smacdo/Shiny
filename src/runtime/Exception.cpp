#include "runtime/Exception.h"

#include <cassert>
#include <fmt/format.h>

using namespace Shiny;

//------------------------------------------------------------------------------
Exception::Exception(std::string message, EXCEPTION_CALLSITE_PARAMS)
    : std::runtime_error(format(message, function, fileName, lineNumber)),
      message_(message),
      function_(function),
      fileName_(fileName),
      lineNumber_(lineNumber) {}

//------------------------------------------------------------------------------
std::string Exception::format(
    const std::string& message,
    const char* function,
    const char* fileName,
    int lineNumber) {
  const static std::string defaultMessage = "No exception message provided";
  const auto& safeMessage = (message.empty() ? defaultMessage : message);

  if (fileName != nullptr) {
    assert(lineNumber > 0 && "Line numbers are one indexed not zero");

    if (function != nullptr) {
      return fmt::format(
          "{}:{} ({}): {}", fileName, lineNumber, function, safeMessage);
    } else {
      return fmt::format("{}:{}: {}", fileName, lineNumber, safeMessage);
    }
  } else {
    return safeMessage;
  }
}