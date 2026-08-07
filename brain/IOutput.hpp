#pragma once

#include <atomic>
#include <string>
#include <thread>

constexpr int DELAY = 35;
const std::string GREY = "\033[90m";
const std::string ESC = "\033[0m";
const std::string SPACING = "  ";

class IOutput {
public:
  virtual ~IOutput() = default;
  virtual void show(const std::string &text) = 0;
  virtual void startThinking() {}
  virtual void stopThinking() {}
};

class TerminalOutput : public IOutput {
private:
  std::atomic<bool> thinking{false};
  std::thread spinner;
  std::string doSpacesInText(const std::string &text);

public:
  void show(const std::string &text);
  void startThinking();
  void stopThinking();
};

class CompositeOutput : public IOutput {
private:
  IOutput &terminalOutputLink;
  IOutput &voiceOutputLink;

public:
  CompositeOutput(IOutput &terminalOut, IOutput &voiceOut)
      : terminalOutputLink(terminalOut), voiceOutputLink(voiceOut) {}
  void show(const std::string &text);
  void startThinking();
  void stopThinking();
};
