/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the Utils class
 **/

// Hack for making logs work from "make Test"
#define LOGDIR        string("../logs/")

#include <gtest/gtest.h>
#include "Common/Utils.h"

class UtilsTest : public ::testing::Test {
 protected:
  UtilsTest() {}
  virtual ~UtilsTest() {}
};

/**
 * @test    Simple unit test for logging to standard logfiles
 **/
TEST_F(UtilsTest, Logging) {
  char buffer[4096] = "MESSAGE";
  Utils::Log(SUCCESS, "This is written as a success to the logs: %s", buffer);
  Utils::Log(DEBUG,   "This is written as a debug to the logs: %s", buffer);
  Utils::Log(WARNING, "This is written as a warning to the logs: %s", buffer);
  Utils::Log(ERROR,   "This is written as an error to the logs: %s", buffer);
  Utils::Log(FATAL,   "This is written as a fatal to the logs: %s", buffer);
}

/**
 * @test    Unit test for loggin out to specific files
 **/
TEST_F(UtilsTest, FileLogging) {
  ASSERT_FALSE(Utils::Log("this/path/does/not/exist", DEBUG,
                          "This won't print out"));

  char buffer[4096] = "MESSAGE";
  ASSERT_TRUE(Utils::Log((LOGDIR + "miscellaneous.log").c_str(), DEBUG,
                          "This will print out: %s", buffer));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


