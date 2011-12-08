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
  EXPECT_TRUE(dummy_->Observe(NULL, NULL, 0, vector<Sensor*>()));
  EXPECT_TRUE(dummy_->StopObserving());
}

/**
 * @test    Dummy observer attempting a match
 **/
TEST_F(DummyObserverTest, AttemptMatch) {
  EXPECT_EQ(dummy_->AttemptMatch(QTable(), vector<Sensor*>(), 0, 0), 0.0);
}

}  // namespace Observation

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
