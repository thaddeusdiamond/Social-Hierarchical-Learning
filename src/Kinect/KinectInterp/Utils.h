// Simple util methods for Kinect app

#include <Windows.h>
#include <cstdio>
#include <cstdarg>
#include <string>
#include <utility>

#define SKEL_EVT 0
#define VIDE_EVT 1
#define DEPT_EVT 2
#define STOP_EVT 3

using std::string;
using std::pair;

static inline void DIE(HRESULT hr, const char* format, ...) {
  va_list arguments;
  va_start(arguments, format);

  fprintf(stderr, format, arguments);
  fprintf(stderr, "  [Code - 0x%08X, Facility - 0x%08X, Severity - 0x%08X]\n",
          HRESULT_CODE(hr), HRESULT_FACILITY(hr), HRESULT_SEVERITY(hr));

  fprintf(stderr, "Press any key to exit...\n");
  getchar();
  exit(hr);
}

namespace Signal {
  bool ProgramIsRunning = true;

  static inline void StopProgram(int status) {
    ProgramIsRunning = false;
  }
}