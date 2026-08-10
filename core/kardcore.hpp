#pragma once

#include "argsParse.hpp"
#include "brain/IBrain.hpp"
#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "voice/voice.hpp"
#include <atomic>

struct ReplComponents {
  TerminalInput terminalInput;
  IOutput &output;
  IBrain &brain;

  ReplComponents(IBrain &brainDependency, IOutput &out)
      : output(out), brain(brainDependency) {}
};

struct ReplInterfaces {
  IInput &input;
  IOutput &output;
  IBrain &brain;
};

ReplInterfaces bindInterfaces(ReplComponents &components);

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
