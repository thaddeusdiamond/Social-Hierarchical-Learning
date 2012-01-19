/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a sensor that receives feedback from the human controller.
 */

#ifndef _SHL_OBSERVATION_PLAYBACK_PLAYBACKSENSOR_H_
#define _SHL_OBSERVATION_PLAYBACK_PLAYBACKSENSOR_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "Common/Utils.h"
#include "Common/Signal.h"
#include "Student/Sensor.h"

/**
 * @todo    (Thad): Random thought, you need to go through SHL.error at some
 *          point and see how we're screwing up Doxygen.  NBD in short-term
 **/
namespace Observation {

using std::string;
using Primitives::Sensor;

class PlaybackSensor : public Sensor {
 public:
  /**
   * The constructor for a PlaybackSensor takes a single CSV filename for
   * the playback output it is supposed to read.
   *
   * @param   filename      The CSV file this sensor is playing back
   * @param   num_sensors   The number of sensors (doubles) per line of file
   **/
  PlaybackSensor(char const * filename, int num_sensors)
    : Sensor("Playback"), filename_(filename), values_(NULL),
      num_values_(num_sensors) {}

  /**
   * The destructor for a PlaybackSensor must free all memory aggregated
   **/
  virtual ~PlaybackSensor() {
    if (file_handle_ != NULL)
      fclose(file_handle_);
  }

  virtual bool SetValues(double const * const values, int num_values);
  virtual double const * const GetValues();

  /**
   * We create an accessor method for the Poll() time...
   **/
  virtual int last_poll_time() { return last_poll_time_; }

  /**
   * ... and for the poll frame
   **/
  virtual int last_poll_frame() { return last_poll_frame_; }

 protected:
  /**
   * Note that the Poll() method operates slightly differently in the
   * PlaybackSensor.  The first call to poll actually starts the playback
   * and returns the value at line 1 of the file.  Subsequent calls to Poll()
   * establish the current time and seek to the appropriate line of the file.
   **/
  virtual bool Poll();

 private:
  /**
   * Sensor plaintext name
   **/
  char const * name_;

  /**
   * Separately keep track of the file name we're reading in from
   **/
  char const * filename_;

  /**
   * The values pointer points to a dynamic set of doubles read in from the
   * file on a call to Poll().  The length of the array is predetermined in
   * the constructor.
   **/
  double const * values_;

  /**
   * Number of values encapsulated by sensor
   **/
  int num_values_;

  /**
   * Keep track of the most recent poll time
   **/
  int last_poll_time_;

  /**
   * Keep track of the most recent frame number polled
   **/
  int last_poll_frame_;

  /**
   * Creation of a global file handle removes wasted time opening/closing I/O
   **/
  FILE* file_handle_;
};

}  // namespace Observation

#endif  // _SHL_OBSERVATION_PLAYBACK_PLAYBACKSENSOR_H_
