#pragma once

#include "core/terminal.hpp"
#include "rawGuard.hpp"
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
  virtual void showUserText(const std::string& text) {}
};

class TerminalOutput : public IOutput {
private:
  TerminalSetup& setup;
  std::atomic<bool> thinking{false};
  std::atomic<bool> &cancelRequesting;
  std::optional<RawMode> rawMode;
  std::thread spinner;
  std::string doSpacesInText(const std::string &text);
  void typewriteText(const std::string& textToShow);
public:
  TerminalOutput(std::atomic<bool> &_cancelRequesting, TerminalSetup& _setup)
      : cancelRequesting(_cancelRequesting), setup(_setup) {}
  void show(const std::string &text) override;
  void startThinking() override;
  void stopThinking() override;
  void showUserText(const std::string& text) override;
};

class CompositeOutput : public IOutput {
private:
  IOutput &terminalOutputLink;
  IOutput &voiceOutputLink;

public:
  CompositeOutput(IOutput &terminalOut, IOutput &voiceOut)
      : terminalOutputLink(terminalOut), voiceOutputLink(voiceOut) {}
  void show(const std::string &text) override;
  void startThinking() override;
  void stopThinking() override;
};
