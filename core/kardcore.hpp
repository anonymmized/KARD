#pragma once

#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "voice/voice.hpp"
#include "brain/OllamaBrain.hpp"
#include "brain/IBrain.hpp"
#include "argsParse.hpp"

struct ReplComponents {
    TerminalInput terminalInput;
    IOutput& output;
    IBrain& brain;

    ReplComponents(IBrain& brainDependency, IOutput& out)
        : output(out),
          brain(brainDependency) {}
};

struct ReplInterfaces {
    IInput& input;
    IOutput& output;
    IBrain& brain;
};

ReplInterfaces bindInterfaces(ReplComponents& components);

class KardCore {
    private:
        ReplInterfaces interfaces;
    public:
        KardCore(ReplInterfaces& _interfaces) : interfaces(_interfaces) {}
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

