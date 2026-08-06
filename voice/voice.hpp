#pragma once

#include "brain/IOutput.hpp"
#include <string>
class VoiceOutput : public IOutput {
private:
  int spawnProcess(const char *argv[]);

public:
  void show(const std::string &text) override;
};
