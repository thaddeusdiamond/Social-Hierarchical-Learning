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
 * @todo    (Thad) Uncomment and move to executable
 **/
/*
static int NUMBER_OF_PRIMITIVES = 8;
static string Primitives[] = {
  "coat", "shoes", "school", "store", "drive", "with", "in", "out"
};
*/

class KinectSensorTest : public testing::Test {
 protected:
  // Create a TAMER object before each test
  KinectSensorTest() {
    /**
     * @todo    (Thad) Uncomment and move to executable
     **/
    // sensor_ = new KinectSensor(18000);
  }

  // Destroy TAMER memory
  virtual ~KinectSensorTest() {
    // delete sensor_;
  }

  // Create a member variable for the actual sensor
  // KinectSensor* sensor_;
};

/**
 * @test    Listen for incoming Kinect feedback
 *
 * @todo    (Thad) We need to put the Kinect test as a standalone executable 
 *          bc it's causing make Test to fail.
 **/
TEST_F(KinectSensorTest, CapturePrimitives) {
  /*
  for (int i = 0; i < NUMBER_OF_PRIMITIVES; i++) {
    // Set up the file handle
    string primitive = Primitives[i];
    sensor_->set_file_handle((string("learned_primitives/") + primitive +
                              string(".csv")).c_str());

    // Alert the user
    Log(stderr, DEBUG, (string("Beginning ") + primitive +
                        string(" primitive.  Press ctrl-\\ to end")).c_str());

    // Keep going till we get the signal to stop
    signal(SIGQUIT, &Signal::StopProgram);
    while (Signal::ProgramIsRunning);

    // Cleanup and I/O reset
    Signal::ResetProgram();
    fprintf(stdout, "\n");
  }
  */
}

}  // namespace Primitives

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
