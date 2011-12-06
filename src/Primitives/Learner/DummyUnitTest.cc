/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the dummy student class we set up
 **/

#include <gtest/gtest.h>
#include "Learner/Dummy.h"

class DummyTest : public ::testing::Test {
 protected:
  // Create a TAMER object before each test
  DummyTest() {
    dummy_ = new Dummy();
  }

  // Destroy TAMER memory
  virtual ~DummyTest() {
    delete dummy_;
  }

  // Unused virtual methods
  virtual void SetUp() {}
  virtual void TearDown() {}

  // Create a member variable for the TAMER learner
  Dummy* dummy_;
  std::vector<Sensor*> env_;
  std::vector<Sensor*> motors_;
};

/**
 * @test    TAMER setup unit test
 **/
TEST_F(DummyTest, Setup_Env_and_motors) {
  EXPECT_TRUE(dummy_->SetEnvironment(env_));
  EXPECT_TRUE(dummy_->SetMotors(motors_));
  EXPECT_TRUE(dummy_->StopLearning());
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
