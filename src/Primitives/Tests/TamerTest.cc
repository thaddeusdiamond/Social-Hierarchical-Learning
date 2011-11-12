/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the TAMER framework
 */

#include <gtest/gtest.h>
#include "Learner/Tamer.h"

class TamerTest : public ::testing::Test {
 protected:
  // Create a TAMER object before each test
  TamerTest() {
    tamer_ = new Tamer();
  }

  // Destroy TAMER memory
  virtual ~TamerTest() {
    delete tamer_;
  }

  // Unused virtual methods
  virtual void SetUp() {}
  virtual void TearDown() {}

  // Create a member variable for the TAMER learner
  Tamer* tamer_;
};

/**
 * @test    TAMER setup unit test
 */
TEST_F(TamerTest, Setup) {
  EXPECT_TRUE(tamer_->Setup(NULL, 0));
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
