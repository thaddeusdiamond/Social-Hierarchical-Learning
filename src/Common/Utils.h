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
 **/

#ifndef _SHL_COMMON_UTILS_H_
#define _SHL_COMMON_UTILS_H_

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Proto/Primitive.pb.h"

using std::string;
using ::google::protobuf::Message;

/**
 * We represent the various levels of the log as global integers so that
 * we can perform a fast switch when writing out
 **/
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
 **/
#ifndef LOGDIR
  #define LOGDIR      string("logs/")
#endif
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
 **/
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
 **/
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
 **/
namespace Utils {
  /**
   * The Utils class has a generic die mechanism by which a program at any
   * point of its execution can cause the program to exit with an error. Note
   * that due to problems with being cyclic, the message is simply printed to
   * stderr instead of being logged
   *
   * @param       format      The message to print out upon exiting
   * @param       ...         The arguments to the message format being printed
   **/
  static inline void Die(const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);

    // There are issues with printing using va_args so we use vsprintf
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    vsprintf(buffer, format, arguments);

    // Print to the stderr
    fprintf(stderr, "%s", buffer);
    perror(" ");
    va_end(arguments);

    exit(EXIT_FAILURE);
  }

  /** @cond PRIVATE_NAMESPACE_MEMBERS **/
    /**
     * We can keep the log files open as static variables to prevent having to
     * slow down from opening and closing them on every log write
     **/
    static FILE* log_file = NULL;
    static FILE* level_file_handles[] = { NULL, NULL, NULL, NULL, NULL };

    /**
     * If all of the logfiles are closed we will make a call to the OpenLogFiles
     * function internally to open all the logfiles and assign static handles to
     * them.
     **/
    static void OpenLogFiles() {
      log_file = fopen((LOGDIR + LOGFILE).c_str(), "w");
      if (!log_file)
        Die("There was an error opening the root log file for writing");

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
   **/
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

    // There are issues with printing using va_args so we use vsprintf
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    vsprintf(buffer, format, arguments);
    strncat(log_format, buffer, strlen(buffer));

    // Make sure it's appended with a newline
    fprintf(log_file, "%s", log_format);
    fprintf(log_file, "\n");
    fprintf(level_file_handles[level], "%s", log_format);
    fprintf(level_file_handles[level], "\n");
    va_end(arguments);
  }

  /**
   * We overload the log function to take a file descriptor so that we can do
   * what we do normally, but to a specific file (used with stderr logging).
   *
   * @param       filedes   File descriptor to have log dumped to
   * @param       level     The level of the debug information
   * @param       format    The format of the message to print
   * @param       ...       The contents of the format used with args lib
   *
   * @return      True on successful finish, false if error
   **/
  static inline bool Log(FILE* filedes, Level level,
                         const char* format, ...) {
    if (!filedes) return false;

#ifdef PREPENDED
  #ifdef COLOR_OUT
    fprintf(filedes, "%s[%s]%s ", level_colors[level].c_str(),
            level_descriptors[level].c_str(), CLEAR_COLOR);
  #else
    fprintf(filedes, "[%s] ", level_descriptors[level].c_str());
  #endif
#endif

    va_list arguments;
    va_start(arguments, format);

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    vsprintf(buffer, format, arguments);

    fprintf(filedes, "%s", buffer);
    fprintf(filedes, "\n");
    va_end(arguments);

    return true;
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
   **/
  static inline bool Log(string const& filename, Level level,
                         const char* format, ...) {
    FILE* log = fopen(filename.c_str(), "w");
    if (!log)
      return false;

    va_list arguments;
    va_start(arguments, format);

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    vsprintf(buffer, format, arguments);

    return Log(log, level, buffer);
  }

  /**
   * We overload the SerializeToFile() function to accept direct file pointers
   *
   * @param       filedes       The file handle to write the protobuf to
   * @param       message       The protocol buffer to be serialized to a file
   * @param       should_close  True if the file handle should be closed on exit
   *
   * @returns     True if the protobuf was successfully serialized and written,
   *              false otherwise
   **/
  static inline bool SerializeToFile(FILE* filedes, Message* message,
                                     bool should_close = false) {
    if (filedes == NULL)
      return false;

    string serialization;
    if (!message->SerializeToString(&serialization))
      return false;
    string name = message->GetTypeName();

    // The format used in printing to a file is this:
    //   [Size of name - 2 bytes][Protobuf type name - Var Len]
    //   [Size of serialization - 2 bytes][Serialization - Var Len]
    fprintf(filedes, "%c%c%s%c%c%s", static_cast<char>(name.size() >> 8),
            static_cast<char>(name.size()), name.c_str(),
            static_cast<char>(message->ByteSize() >> 8),
            static_cast<char>(message->ByteSize()), serialization.c_str());

    if (should_close)
      fclose(filedes);
    return true;
  }

  /**
   * We provide a convenient, polymorphic manner in which the user can
   * serialize a protocol buffer to a string and then output to file
   *
   * @param       filename  The file name to be writing the protobuf to
   * @param       message   The protocol buffer to be serialized to a file
   *
   * @returns     True if the protobuf was successfully serialized and written,
   *              false otherwise
   **/
  static inline bool SerializeToFile(string const& filename, Message* message) {
    FILE* log = fopen(filename.c_str(), "w");
    if (!log)
      return false;
    return SerializeToFile(log, message, true);
  }

  /**
   * We provide a convenient manner to deserialize the primitive from a disk
   * record that we just wrote out using SerializeToFile().
   *
   * @param       filedes   The file handle to be read from
   * @param       should_close  True if the file handle should be closed on exit
   *
   * @returns     A generic protocol buffer fully populated.
   **/
  static inline Message* ParseFromFile(FILE* filedes,
                                       bool should_close = false) {
    if (filedes == NULL)
      return NULL;

    // Read in the message type into a buffer
    char message_type[4096];
    memset(message_type, 0, sizeof(message_type));
    fread(message_type, sizeof(message_type[0]), 2, filedes);
    int name_length = (static_cast<int>(message_type[0]) << 8) +
      static_cast<int>(message_type[1]);
    fread(message_type, sizeof(message_type[0]), name_length, filedes);

    // Read in the serialized form for future use
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    fread(buffer, sizeof(buffer[0]), 2, filedes);
    int serialized_length = (static_cast<int>(buffer[0]) << 8) +
      static_cast<int>(buffer[1]);
    fread(buffer, sizeof(buffer[0]), serialized_length, filedes);

    // Switch how we parse based on message type
    Message* message;
    if (!strcmp(message_type, "Primitive")) {
      message = new Primitive();
      if (!message->ParseFromArray(buffer, serialized_length))
        return NULL;
    }

    if (should_close)
      fclose(filedes);
    return message;
  }

  /**
   * We overload the ParseFromFile() method to allow the method to take an
   * arbitrary string filename
   *
   * @param       filename  The filename to be read in
   *
   * @returns     A generic protocol buffer fully populated.
   **/
  static inline Message* ParseFromFile(string const& filename) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file)
      return NULL;
    return ParseFromFile(file, true);
  }
};


#endif  // _SHL_COMMON_UTILS_H_
