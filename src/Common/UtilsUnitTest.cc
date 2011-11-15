/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the Utils class
 */

#include <gtest/gtest.h>
#include "Common/Utils.h"

class UtilsTest : public ::testing::Test {
 protected:
  UtilsTest() {}
  virtual ~UtilsTest() {}
};

/**
 * @test    Simple unit test for logging output
 */
TEST_F(UtilsTest, Logging) {
  Utils::Log(SUCCESS, "This is written as a success to the logs");
  Utils::Log(DEBUG,   "This is written as a debug to the logs");
  Utils::Log(WARNING, "This is written as a warning to the logs");
  Utils::Log(ERROR,   "This is written as an error to the logs");
  Utils::Log(FATAL,   "This is written as a fatal to the logs");
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

