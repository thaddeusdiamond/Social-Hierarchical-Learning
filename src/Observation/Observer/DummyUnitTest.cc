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
#include "Observer/Dummy.h"
#include "Common/Utils.h"

namespace Observation {

class DummyObserverTest : public ::testing::Test {
 public:
  // Create an Observer object before each test
  DummyObserverTest() {
    dummy_ = new Dummy();
  }

  // Destroy Observer memory
  virtual ~DummyObserverTest() {
    delete dummy_;
  }

  // Create a member variable for the sample observer
  Dummy* dummy_;
};

/**
 * @test    Dummy observer simple start/stop
 **/
TEST_F(DummyObserverTest, StartAndStopObserver) {
  EXPECT_TRUE(dummy_->Observe(NULL));
  EXPECT_TRUE(dummy_->StopObserving());
}

}  // namespace Observation

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
