#pragma once

#include "brain/IInput.hpp"
#include "brain/IOutput.hpp"
#include "brain/OllamaBrain.hpp"
#include "brain/IBrain.hpp"
#include "argsParse.hpp"

class KardCore {
    private:
        IBrain& brain;
        IInput& input;
        IOutput& output;
    public:
        KardCore(IInput& _input, IOutput& _output, IBrain& _brain) : input(_input), output(_output), brain(_brain) {}
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
