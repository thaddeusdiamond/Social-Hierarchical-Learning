/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a library for using signal interrupts in our application.  As long
 * as each person is careful, there should be no problems.  However, in
 * each executable only one portion of the program can use the signal handler
 * at a time.
 **/

#ifndef _SHL_COMMON_SIGNAL_H_
#define _SHL_COMMON_SIGNAL_H_

#include <signal.h>

namespace Signal {
  static bool ProgramIsRunning = true;

  static inline void StopProgram(int status) {
    ProgramIsRunning = false;
  }

  static inline void ResetProgram() {
    ProgramIsRunning = true;
  }
}

#endif  // _SHL_COMMON_SIGNAL_H_
