#pragma once

#include <string>

class InteractiveSession {
public:
  InteractiveSession();
  ~InteractiveSession();

  void Run();

private:
  void MainLoop();
  std::string GetUserInput() const;
  void OnStart();
  void OnStop();
};
