#include "voice/voice.hpp"

#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

pid_t VoiceOutput::spawnProcess(const char *argv[]) {
  pid_t pid;
  if (posix_spawn(&pid, argv[0], nullptr, nullptr,
                  const_cast<char *const *>(argv), environ) == 0) {
    return pid;
  }
  return -1;
}

void VoiceOutput::show(const std::string &text) {
#ifdef __APPLE__
  const char *argv[] = {"say", text.c_str(), nullptr};
#else
  const char *argv[] = {"espeak-ng", text.c_str(), nullptr};
#endif
  pid_t pid = spawnProcess(argv);
  waitpid(pid, nullptr, 0);
}
