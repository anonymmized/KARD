#include "brain/IOutput.hpp"
#include "voice/voice.hpp"

#include <spawn.h>
#include <sys/wait.h>
extern char** environ;

void VoiceOutput::show(const std::string& text) {
#ifdef __APPLE__
    const char* argv[] = {"say", text.c_str(), nullptr};
#else
    const char* argv[] = {"espeak-ng", text.c_str(), nullptr};
#endif
    pid_t pid;
    if (posix_spawnp(&pid, argv[0], nullptr, nullptr, const_cast<char* const*>(argv), environ) == 0) {
        waitpid(pid, nullptr, 0);
    }
}
