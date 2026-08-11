#pragma once

#include "argsParse.hpp"
#include "contracts/IBrain.hpp"
#include "contracts/IInput.hpp"
#include "contracts/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "voice/voice.hpp"
#include <atomic>

struct ReplInterfaces {
  IInput &input;
  IOutput &output;
  IBrain &brain;
};

class KardCore {
private:
  ReplInterfaces interfaces;
  std::atomic<bool> &cancelRequesting;

public:
  KardCore(ReplInterfaces &_interfaces, std::atomic<bool> &_cancelRequesting)
      : interfaces(_interfaces), cancelRequesting(_cancelRequesting) {}
  void run();
};

int executeArgs(CliArguments arguments);
int startDaemon();
int stopDaemon();
int getDaemonStatus();
int getBasicUpdate();
int getFullUpdate();
void testArgument();
void printHelpPage();
