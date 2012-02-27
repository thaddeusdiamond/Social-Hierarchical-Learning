/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the Feedback Sensor
 **/

#include <gtest/gtest.h>
#include "Student/FeedbackSensor.h"

namespace Primitives {

class FeedbackSensorTest : public testing::Test {
 protected:
  FeedbackSensorTest() {
    sensor_ = new FeedbackSensor(20000);
  }

  virtual ~FeedbackSensorTest() {}

  FeedbackSensor* sensor_;
};

/**
 * @test    Listen for incoming feedback
 **/
TEST_F(FeedbackSensorTest, CaptureFeedback) {
  sleep(5);
  ASSERT_TRUE(sensor_->Poll());
  EXPECT_FALSE(sensor_->Poll());
  EXPECT_EQ(sensor_->GetValues()[0], 0);
}

}  // namespace Primitives

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
