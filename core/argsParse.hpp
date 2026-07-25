#pragma once

#include <string>

enum class Mode {
    Repl,
    StartDaemon,
    StopDaemon,
    DaemonStatus,
    Update,
    FullUpdate,
    Test,
    Help,
    Unknown
};

struct CliArguments {
    Mode mode = Mode::Repl;
    bool voice = false;
    std::string errorOut;
};

class ArgumentParser {
    private:
        int argCount;
        char** argComponents;
    public:
        ArgumentParser(int _argCount, char** _argComponents) : argCount(_argCount), argComponents(_argComponents) {}
        ArgumentParser(char** _argComponents, int _argCount) : argComponents(_argComponents), argCount(_argCount) {}
        ~ArgumentParser() = default;
        bool checkIfArgumentsInstalled();
        CliArguments parseArguments();
};
