/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the Kinect Sensor
 **/

#include <gtest/gtest.h>
#include "Student/KinectSensor.h"

namespace Primitives {

/**
 * @todo    (Thad): Create an actual unit test here
 **/
class KinectSensorTest : public testing::Test {
 protected:
  KinectSensorTest() {}

  virtual ~KinectSensorTest() {}
};

/**
 * @test    Listen for incoming Kinect feedback
 **/
TEST_F(KinectSensorTest, CapturePrimitives) {
}

}  // namespace Primitives

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
