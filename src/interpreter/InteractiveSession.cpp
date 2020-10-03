#include "InteractiveSession.h"

#include "runtime/Evaluator.h"
#include "runtime/Reader.h"
#include "runtime/Value.h"

#include <iostream>
#include <string>

using namespace Shiny;

//--------------------------------------------------------------------------------------------------
InteractiveSession::InteractiveSession() {}

//--------------------------------------------------------------------------------------------------
InteractiveSession::~InteractiveSession() {}

//--------------------------------------------------------------------------------------------------
void InteractiveSession::Run() {
  OnStart();
  MainLoop();
  OnStop();
}

//--------------------------------------------------------------------------------------------------
void InteractiveSession::MainLoop() {
  // Interpreter initialize.
  Evaluator evaluator;

  while (true) {
    Reader reader;
    auto result = evaluator.Evaluate(reader.read(GetUserInput()));
    std::cout << result << std::endl; // TODO: use stream operator.
  }
}

//--------------------------------------------------------------------------------------------------
std::string InteractiveSession::GetUserInput() const {
  std::string input;

  std::cout << "> ";
  std::cin >> input;

  return input;
}

//--------------------------------------------------------------------------------------------------
void InteractiveSession::OnStart() {
  std::cout << "Welcome to Shiny, a simple scheme inspired language. Use ctrl+c to exit."
            << std::endl;
}

//--------------------------------------------------------------------------------------------------
void InteractiveSession::OnStop() { std::cout << "Goodbye!" << std::endl; }