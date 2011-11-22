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
#include "Common/Primitive.pb.h"
#include "Common/Utils.h"

using Utils::Log;
using Utils::SerializeToFile;
using Utils::ParseFromFile;

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
  Log(SUCCESS, "This is written as a success to the logs: %s", buffer);
  Log(DEBUG,   "This is written as a debug to the logs: %s", buffer);
  Log(WARNING, "This is written as a warning to the logs: %s", buffer);
  Log(ERROR,   "This is written as an error to the logs: %s", buffer);
  Log(FATAL,   "This is written as a fatal to the logs: %s", buffer);
}

/**
 * @test    Unit test for logging out to specific files
 **/
TEST_F(UtilsTest, FileLogging) {
  ASSERT_FALSE(Log("this/path/does/not/exist", DEBUG,
                          "This won't print out"));

  char buffer[4096] = "MESSAGE";
  ASSERT_TRUE(Log((LOGDIR + "miscellaneous.log").c_str(), DEBUG,
                          "This will print out: %s", buffer));
}

/**
 * @test    Unit test for serializing an arbitrary protocol buffer to file
 **/
TEST_F(UtilsTest, FileSerialization) {
  Primitive* primitive = new Primitive();
  primitive->set_id(2);
  primitive->add_relevant_motors(9);
  
  EXPECT_FALSE(SerializeToFile("this/file/does/not/exist", primitive));
  EXPECT_TRUE(SerializeToFile("../db/1.primitive", primitive));
  
  Message* message;
  ASSERT_TRUE((message = ParseFromFile("../db/1.primitive")));
  EXPECT_EQ(primitive->id(), reinterpret_cast<Primitive*>(message)->id());
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


