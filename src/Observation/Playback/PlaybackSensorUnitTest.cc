/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Testing for the playback using file I/O
 **/

#include <gtest/gtest.h>
#include "Playback/PlaybackSensor.h"

namespace Observation {

class KinectSensorTest : public testing::Test {
 protected:
  KinectSensorTest() {}

  virtual ~KinectSensorTest() {}
};

/**
 * @test    Play a file and poll sensors, ensuring known values
 **/
TEST_F(KinectSensorTest, PollKnownFile) {
}

}  // namespace Observation

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
