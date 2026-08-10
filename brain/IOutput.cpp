#include "IOutput.hpp"
#include "rawGuard.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

void TerminalOutput::show(const std::string &text) {
  std::string spacedText = doSpacesInText(text);
  std::cout << SPACING + "Assistant: " << std::flush;
  typewriteText(spacedText);
  std::cout << '\n' << SPACING + "user: " << std::flush; // issue №22
}

std::string TerminalOutput::doSpacesInText(const std::string &text) {
  std::string result = "";
  for (char symbol : text) {
    result += symbol;
    if (symbol == '\n') {
      result += SPACING;
    }
  }
  return result;
}

void TerminalOutput::typewriteText(const std::string& textToShow) {
    std::cout << "\033[?25l";
    for (char symbol : textToShow) {
        std::cout << symbol << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    std::cout << "\033[?25h";
}

void TerminalOutput::stopThinking() {
  thinking = false;
  if (spinner.joinable()) {
    spinner.join();
  }
  rawMode.reset();
}

void TerminalOutput::startThinking() {
  cancelRequesting = false;
  thinking = true;
  rawMode.emplace();
  spinner = std::thread([this] {
    const char *frames[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    int i = 0;
    while (thinking) {
      std::cout << '\r' << GREY + frames[i++ % 10] + ESC << std::flush;
      char chr;
      if (read(STDIN_FILENO, &chr, 1) == 1 && chr == 27) {
        cancelRequesting = true;
      }
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

void TerminalOutput::clearTerminal() {
  std::cout << "\033[2J\033[H" << std::flush;
}

void CompositeOutput::clearTerminal() {
  std::cout << "\033[2J\033[H" << std::flush;
}
