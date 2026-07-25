#include "argsParse.hpp"

CliArguments ArgumentParser::parseArguments() {
    CliArguments args = {};
    if (!checkIfArgumentsInstalled()) {
        return args;
    }
    for (int i = 1; i < argCount; i++) {
        std::string currArg = argComponents[i];
        if (currArg == "--rund") { args.mode = Mode::StartDaemon; }
        else if (currArg == "--stopd") { args.mode = Mode::StopDaemon; }
        else if (currArg == "--statusd") { args.mode = Mode::DaemonStatus; }
        else if (currArg == "--update") { args.mode = Mode::Update; }
        else if (currArg == "--full-update") { args.mode = Mode::FullUpdate; }
        else if (currArg == "--test") { args.mode = Mode::Test; }
        else if (currArg == "--help" || currArg == "-h") { args.mode = Mode::Help; }
        else {
            args.mode = Mode::Unknown;
            args.errorOut = "There is no argument like " + currArg + ". Please try again with valid argument.\n";
        }
    }
    return args;
}

bool ArgumentParser::checkIfArgumentsInstalled() {
    return argCount && argComponents;
}
