#include "InteractiveSession.h"

#include "runtime/Evaluator.h"
#include "runtime/Reader.h"
#include "runtime/Value.h"
#include "runtime/VmState.h"
#include "runtime/allocators/MallocAllocator.h"

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
  auto vmState = std::make_shared<VmState>(std::make_unique<MallocAllocator>());
  Evaluator evaluator(vmState);

  std::string line;

  while (true) {
    Reader reader{vmState};
    auto result = evaluator.evaluate(reader.read(GetUserInput()));
    std::cout << result << std::endl; // TODO: use stream operator.
  }
}

//--------------------------------------------------------------------------------------------------
std::string InteractiveSession::GetUserInput() const {
  std::string input;

  std::cout << "> ";
  std::getline(std::cin, input);

  return input;
}

//--------------------------------------------------------------------------------------------------
void InteractiveSession::OnStart() {
  std::cout << "Welcome to Shiny, a simple scheme inspired language. Use "
               "ctrl+c to exit."
            << std::endl;
}

//--------------------------------------------------------------------------------------------------
void InteractiveSession::OnStop() { std::cout << "Goodbye!" << std::endl; }