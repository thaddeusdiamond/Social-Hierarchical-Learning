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

class PlaybackSensorTest : public testing::Test {
 protected:
  PlaybackSensorTest() {
    sensor_ = new PlaybackSensor("../asl_data/coat.csv", 6);
  }

  virtual ~PlaybackSensorTest() {
    delete sensor_;
  }

  PlaybackSensor* sensor_;
};

/**
 * @test    Start polling and check that it is running, times are updating right
 **/
TEST_F(PlaybackSensorTest, CheckUpdatedTimesAndRunning) {
  EXPECT_FALSE(sensor_->running());

  // Get current time to check against polling
  timespec time_holder;
  clock_gettime(CLOCK_REALTIME, &time_holder);
  double current_time_ms = (static_cast<double>(time_holder.tv_nsec) / 1000000);

  sensor_->GetValues();
  EXPECT_TRUE(sensor_->running());
  EXPECT_EQ(sensor_->last_poll_frame(), 1);
  EXPECT_LE(current_time_ms - sensor_->last_poll_time(), 10);

  sleep(3);

  sensor_->GetValues();
  EXPECT_FALSE(sensor_->running());
  EXPECT_EQ(sensor_->last_poll_frame(), 36);
  EXPECT_LE(current_time_ms + 3000 - sensor_->last_poll_time(), 10);
}

/**
 * @test    Play a file and poll sensors, ensuring known values
 *          The file we use here is "asl_data/coat.csv" and so we can compare
 *          pre-canned values to that given to us from the sensor.
 **/
TEST_F(PlaybackSensorTest, PollKnownFile) {
  const double* observed_sensors = sensor_->GetValues();
  EXPECT_EQ(observed_sensors[0], -0.04426000);
  EXPECT_EQ(observed_sensors[1],  0.10709000);
  EXPECT_EQ(observed_sensors[2], -1872.00000);
  EXPECT_EQ(observed_sensors[3],  0.04795000);
  EXPECT_EQ(observed_sensors[4],  0.10749000);
  EXPECT_EQ(observed_sensors[5], -1600.00000);

  sleep(3);

  observed_sensors = sensor_->GetValues();
  EXPECT_EQ(observed_sensors[0], -0.07091000);
  EXPECT_EQ(observed_sensors[1],  0.38668000);
  EXPECT_EQ(observed_sensors[2], -1544.00000);
  EXPECT_EQ(observed_sensors[3],  0.07953000);
  EXPECT_EQ(observed_sensors[4],  0.35842000);
  EXPECT_EQ(observed_sensors[5], -1128.00000);
  EXPECT_FALSE(sensor_->stale());
  
  observed_sensors = sensor_->GetValues();
  EXPECT_EQ(observed_sensors[0], -0.07091000);
  EXPECT_EQ(observed_sensors[1],  0.38668000);
  EXPECT_EQ(observed_sensors[2], -1544.00000);
  EXPECT_EQ(observed_sensors[3],  0.07953000);
  EXPECT_EQ(observed_sensors[4],  0.35842000);
  EXPECT_EQ(observed_sensors[5], -1128.00000);
  EXPECT_TRUE(sensor_->stale());
}

}  // namespace Observation

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
