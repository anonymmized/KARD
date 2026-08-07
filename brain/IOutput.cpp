#include "IOutput.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void TerminalOutput::show(const std::string &text) {
  std::string spacedText = doSpacesInText(text);
  std::cout << SPACING + "Assistant: " << text << '\n'
            << SPACING + "user: "; // issue №22
}

std::string TerminalOutput::doSpacesInText(const std::string &text) {
  std::string result = SPACING;
  for (char symbol : text) {
    result += symbol;
    if (symbol == '\n') {
      result += SPACING;
    }
  }
  return result;
}

void TerminalOutput::stopThinking() {
  thinking = false;
  if (spinner.joinable())
    spinner.join();
}

void TerminalOutput::startThinking() {
  thinking = true;
  spinner = std::thread([this] {
    const char *frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    int i = 0;
    while (thinking) {
      std::cout << '\r' << GREY + frames[i++ % 10] + ESC << std::flush;
      std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }
    std::cout << "\r\033[K" << std::flush;
  });
}

void CompositeOutput::show(const std::string &text) {
  terminalOutputLink.show(text);
  voiceOutputLink.show(text);
}

void CompositeOutput::startThinking() {
  terminalOutputLink.startThinking();
  voiceOutputLink.startThinking();
}

void CompositeOutput::stopThinking() {
  terminalOutputLink.stopThinking();
  voiceOutputLink.stopThinking();
}
