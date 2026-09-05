#include "terminal/TerminalOutput.hpp"
#include "RawMode.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

namespace {
const std::unordered_map<std::string, std::string> METRIC_TITLES = {
    {"get_cpu", "CPU"},
    {"get_ram", "RAM"},
    {"get_disk", "Disk"},
    {"get_uptime", "Uptime"},
    {"get_temp", "Temperature"},
    {"get_network", "Network latency"},
    {"get_docker_status", "Docker"},
    {"get_docker_running", "Running containers"},
    {"get_docker_list", "Containers"}
};

std::string metricTitle(const std::string& name) {
    const auto title = METRIC_TITLES.find(name);
    return title == METRIC_TITLES.end() ? name : title->second;
}

std::string formatMetric(const Metric& metric) {
    std::string line = "- " + metricTitle(metric.name) + ": " + metric.value;

    if (!metric.unit.empty()) {
        line += metric.unit == "%" ? metric.unit : " " + metric.unit;
    }

    if (metric.state != MetricState::Unavailable) {
        line += " (";
        line += metricStateToString(metric.state);
        line += ")";
    }

    return line;
}
} // namespace

void TerminalOutput::showAnswer(const BrainAnswer& brainAnswer) {
    if (!brainAnswer.textAnswer.empty()) {
        show(brainAnswer.textAnswer);
    }

    if (brainAnswer.metrics.empty()) {
        return;
    }

    std::string metricsBlock = "Metrics:\n";
    for (const Metric& metric : brainAnswer.metrics) {
        metricsBlock += formatMetric(metric) + "\n";
    }
    metricsBlock.pop_back();
    show(metricsBlock);
}

void TerminalOutput::show(const std::string& text) {
    if (!setup.isInteractive()) {
        std::cout << text << '\n' << std::flush;
        return;
    }
    render.appendText(text + "\n\n");
}

void TerminalOutput::startThinking() {
    if (!setup.isInteractive()) {
        return;
    }
    rawMode.emplace();
    std::cout << setup.getGrey();
    spinner.start(cancelRequesting);
    std::cout << setup.getReset();
}

void TerminalOutput::stopThinking() {
    spinner.stop();
    rawMode.reset();
}

void TerminalOutput::showUserText(const std::string& text) {
    std::string newText = removeSpaces(text);
    std::cout << "\033[" << setup.getInputRow() << ";1H\033[2K";
    if (!setup.isInteractive()) {
        std::cout << "> " << newText << '\n' << std::flush;
        return;
    }
    render.appendText(setup.getGrey() + "> " + newText + "\n" + setup.getReset());
}

std::string TerminalOutput::removeSpaces(const std::string& baseline) {
    size_t firstNotSpace = baseline.find_first_not_of(' ');
    if (firstNotSpace == std::string::npos) {
        return "";
    }
    size_t lastNotSpace = baseline.find_last_not_of(' ');
    return baseline.substr(firstNotSpace, lastNotSpace - firstNotSpace + 1);
}

void CompositeOutput::show(const std::string& text) {
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

void CompositeOutput::showUserText(const std::string& text) {
    terminalOutputLink.showUserText(text);
    voiceOutputLink.showUserText(text);
}

void CompositeOutput::showAnswer(const BrainAnswer& brainAnswer) {
    terminalOutputLink.showAnswer(brainAnswer);
}
