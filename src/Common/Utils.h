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

#ifndef _SHL_SRC_COMMON_UTILS_H_
#define _SHL_SRC_COMMON_UTILS_H_

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using std::string;

/**
 * We represent the various levels of the log as global integers so that
 * we can perform a fast switch when writing out
 */
#define NUMBER_OF_LEVELS 5
enum Level {
  SUCCESS = 0,
  DEBUG = 1,
  WARNING = 2,
  ERROR = 3,
  FATAL = 4
};

/**
 * We represent the files of the output as an array for fast switching
 */
#define LOGDIR        string("logs/")
#define LOGFILE       string("log")
static string level_files[] = {
  "log.success",
  "log.debug",
  "log.warning",
  "log.error",
  "log.fatal"
};

/**
 * We represent the colors of the output as an array for fast switching
 */
#define COLOR_OUT
#define CLEAR_COLOR   "\033[0m"
static string level_colors[] = {
  "\033[1;32m",
  "\033[1;34m",
  "\033[1;33m",
  "\033[1;31m",
  "\033[1;39m"
};

/**
 * We may want to prepend the log level to the actual print out message
 */
#define PREPENDED
static string level_descriptors[] = {
  "SUCCESS",
  "DEBUG",
  "WARNING",
  "ERROR",
  "FATAL"
};

/**
 * The Utils namespace defines several utility functions that ease application
 * writing including logging and automatic exit.
 *
 * @addtogroup  Utilities
 */
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

  /** @cond PRIVATE_NAMESPACE_MEMBERS **/
    /**
     * We can keep the log files open as static variables to prevent having to
     * slow down from opening and closing them on every log write
     */
    static FILE* log_file = NULL;
    static FILE* level_file_handles[] = { NULL, NULL, NULL, NULL, NULL };

    /**
     * If all of the logfiles are closed we will make a call to the OpenLogFiles
     * function internally to open all the logfiles and assign static handles to
     * them.
     */
    static void OpenLogFiles() {
      log_file = fopen((LOGDIR + LOGFILE).c_str(), "w");

      for (int i = 0; i < NUMBER_OF_LEVELS; i++) {
        level_file_handles[i] = fopen((LOGDIR + level_files[i]).c_str(), "w");
        if (!level_file_handles[i])
          Die("There was an error opening the log files for writing");
      }
    }
  /** @endcond **/

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

    // Prepend level statement and add color to the output if macros defined
    char log_format[4096];
    memset(log_format, 0, sizeof(log_format));
#ifdef PREPENDED
  #ifdef COLOR_OUT
    snprintf(log_format, sizeof(log_format), "%s[%s]%s ",
             level_colors[level].c_str(), level_descriptors[level].c_str(),
             CLEAR_COLOR);
  #else
    snprintf(log_format, sizeof(log_format), "[%s] ",
             level_descriptors[level].c_str());
  #endif
#endif
    strncat(log_format, format, strlen(format));

    // Make sure it's appended with a newline
    fprintf(log_file, log_format, arguments);
    fprintf(log_file, "\n");
    fprintf(level_file_handles[level], log_format, arguments);
    fprintf(level_file_handles[level], "\n");
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

#ifdef PREPENDED
  #ifdef COLOR_OUT
    fprintf(log, "%s[%s]%s ", level_colors[level].c_str(),
            level_descriptors[level].c_str(), CLEAR_COLOR);
  #else
    fprintf(log, "[%s] ", level_descriptors[level].c_str());
  #endif
#endif
    fprintf(log, format, arguments);
    fprintf(log, "\n");

    va_end(arguments);
    return true;
  }
};


#endif  // _SHL_SRC_COMMON_UTILS_H_
