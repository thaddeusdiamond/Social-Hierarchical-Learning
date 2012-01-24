/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is the implementation for a playback sensor (read from file I/O)
 */

#include "Playback/PlaybackSensor.h"

namespace Observation {

bool PlaybackSensor::SetValues(double const * const values, int num_values) {
  // This sensor cannot be set
  return false;
}

double const * const PlaybackSensor::GetValues() {
  // Return a one-element array to most recent value
  stale_ = !Poll();
  return values_;
}

bool PlaybackSensor::Poll() {
  // If file not opened, open it and fail if it's invalid
  if (!file_handle_) {
    running_ = true;

    timespec time_holder;
    clock_gettime(CLOCK_REALTIME, &time_holder);
    base_time_ = time_holder.tv_sec;

    file_handle_ = fopen(filename_, "r");
    if (!file_handle_) {
      Log(stderr, ERROR, 
        string("Could not load file for playback").c_str()); 
      return false;
    }
  }

  // DO YOU KNOW WHAT TIME IT ISSSS???
  timespec time_holder;
  clock_gettime(CLOCK_REALTIME, &time_holder);
  double current_time_ms = ((time_holder.tv_sec - base_time_) * 1000) +
                           (static_cast<double>(time_holder.tv_nsec) / 1000000);

  // We want to do the I/O all at once, so we dynamically determine how many
  // frames we've "received" since the last poll. 0 indicates none, some o/w
  int num_frames_recvd;

  // If we're just starting...
  if (!last_poll_frame_)
    num_frames_recvd = 1;

  // Otherwise, we're polling at some arbitrary point
  else
    num_frames_recvd = floor((current_time_ms - last_poll_time_) * 3 / 100);

  // Shove the frames in a buffer so that we can back up one if need be (EOF)
  int num_frames_read = 0;
  char line_buffer[4096];
  for (int i = 0; i < num_frames_recvd; i++) {
    if (!fgets(line_buffer, sizeof(line_buffer), file_handle_)) {
      running_ = false;
      break;
    }
    num_frames_read++;
  }

  // Nothing's come from the sensor (polled too fast), don't update
  if (!num_frames_read)
    return false;

  char* scanner = &line_buffer[0],
      * leftover;
  for (int i = 0; i < num_values_; i++) {
    if (!scanner)
      return false;                             // Invalid line
    values_[i] = strtod(scanner, &leftover);
    scanner = leftover + 1;
  }

  // Only update the frame # and time if we successfully got data
  last_poll_frame_ += MIN(num_frames_read, num_frames_recvd);
  last_poll_time_   = num_frames_read == 0 ? last_poll_time_ : current_time_ms;
  return (num_frames_read > 0);
}

}  // namespace Observation
