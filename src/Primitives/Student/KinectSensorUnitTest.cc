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

class KinectSensorTest : public testing::Test {
 protected:
  // Create a TAMER object before each test
  KinectSensorTest() {
    sensor_ = new KinectSensor(18000);
  }

  // Destroy TAMER memory
  virtual ~KinectSensorTest() {
    delete sensor_;
  }

  // Create a member variable for the actual sensor
  KinectSensor* sensor_;
};

/**
 * @test    Listen for incoming Kinect feedback
 **/
TEST_F(KinectSensorTest, BeginListening) {
  fprintf(stdout, "Beginning Kinect Test.  Press ctrl-\\ to exit\n");
  signal(SIGQUIT, &Signal::StopProgram);
  while (Signal::ProgramIsRunning);
}

}  // namespace Primitives

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
