/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a library for using logging in our application.  We create various
 * levels of protection (success, debug, warning, error, fatal) that are
 * simultaneously written to individual logs and one main stream.
 */

#ifndef _SHL_COMMON_UTILS_H_
#define _SHL_COMMON_UTILS_H_

#define LOGDIR        string("logs/")
#define LOGFILE       string("log")
#define SUCCESS_FILE  string("log.success")
#define DEBUG_FILE    string("log.debug")
#define WARNING_FILE  string("log.warning")
#define ERROR_FILE    string("log.error")
#define FATAL_FILE    string("log.fatal")

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>

using std::string;

/**
 * We represent the various levels of the log as global integers so that
 * we can perform a fast switch when writing out
 */
enum Level {
  SUCCESS,
  DEBUG,
  WARNING,
  ERROR,
  FATAL
};

namespace Utils {
  /**
   * The Utils class has a generic die mechanism by which a program at any
   * point of its execution can cause the program to exit with an error. Note
   * that due to problems with being cyclic, the message is simply printed to
   * stderr instead of being logged
   *
   * @param       format      The message to print out upon exiting
   * @param       ...         The arguments to the message format being printed
   */
  static inline void Die(const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);

    fprintf(stderr, format, arguments);
    va_end(arguments);
    exit(EXIT_FAILURE);
  }

  /**
   * We can keep the log files open as static variables to prevent having to
   * slow down from opening and closing them on every log write
   */
  static FILE* log_file;
  static FILE* success_file;
  static FILE* debug_file;
  static FILE* warning_file;
  static FILE* error_file;
  static FILE* fatal_file;

  /**
   * If all of the logfiles are closed we will make a call to the OpenLogFiles
   * function internally to open all the logfiles and assign static handles to
   * them.
   */
  static void OpenLogFiles() {
    log_file     = fopen((LOGDIR + LOGFILE).c_str(),       "w");
    success_file = fopen((LOGDIR + SUCCESS_FILE).c_str(),  "w");
    debug_file   = fopen((LOGDIR + DEBUG_FILE).c_str(),    "w");
    warning_file = fopen((LOGDIR + WARNING_FILE).c_str(),  "w");
    error_file   = fopen((LOGDIR + ERROR_FILE).c_str(),    "w");
    fatal_file   = fopen((LOGDIR + FATAL_FILE).c_str(),    "w");

    if (!log_file || !success_file || !debug_file || !warning_file ||
        !error_file || !fatal_file)
      Die("There was an error opening the log files for writing");
  }

  /**
   * The generic Log function takes a specific level and, based on what it
   * was given, writes out the information to the proper file.  No matter what
   * the information is always written to the root log.
   *
   * @param       level     The level of the debug information
   * @param       format    The format of the message to print
   * @param       ...       The contents of the format used with args lib
   */
  static inline void Log(Level level, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);

    if (log_file == NULL)
      OpenLogFiles();

    fprintf(log_file, format, arguments);
    switch (level) {
      case SUCCESS:
        fprintf(success_file, format, arguments);
        break;
      case DEBUG:
        fprintf(debug_file, format, arguments);
        break;
      case WARNING:
        fprintf(warning_file, format, arguments);
        break;
      case ERROR:
        fprintf(error_file, format, arguments);
        break;
      case FATAL:
        fprintf(fatal_file, format, arguments);
        break;
    }

    va_end(arguments);
  }

  /**
   * We overload the log function to take a filename so that we can do
   * what we do normally, but to a specific file.
   *
   * @param       filename  File to have log dumped to
   * @param       level     The level of the debug information
   * @param       format    The format of the message to print
   * @param       ...       The contents of the format used with args lib
   *
   * @return      True on successful finish, false if error
   */
  static inline bool Log(const string& filename, Level level,
                         const char* format, ...) {
    FILE* log;
    log = fopen(filename.c_str(), "w");
    if (log == NULL)
      return false;

    va_list arguments;
    va_start(arguments, format);

    fprintf(log, format, arguments);

    va_end(arguments);
    return true;
  }
};

#endif  // _SHL_COMMON_UTILS_H_
