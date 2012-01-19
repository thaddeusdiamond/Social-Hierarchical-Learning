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
#include "Student/Dummy.h"

namespace Primitives {

class DummyTest : public testing::Test {
 protected:
  DummyTest() {
    dummy_ = new Dummy();
  }

  virtual ~DummyTest() {
    delete dummy_;
  }

  Dummy* dummy_;
  std::vector<Sensor*> env_;
  std::vector<Sensor*> motors_;
};

/**
 * @test    Student setup unit test
 **/
TEST_F(DummyTest, Setup_Env_and_motors) {
  EXPECT_TRUE(dummy_->SetEnvironment(env_));
  EXPECT_TRUE(dummy_->SetMotors(motors_));
  EXPECT_TRUE(dummy_->StopLearning());
}

}  // namespace Primitives

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
