/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is the implementation for a playback sensor (read from file I/O)
 */

namespace Observation {

bool KinectSensor::SetValues(double const * const values, int num_values) {
  // This sensor cannot be set
  return false;
}

double const * const KinectSensor::GetValues() {
  // Return a one-element array to most recent value
  Poll();
  return values_;
}

bool KinectSensor::Poll() {
  // @TODO (Thad): File I/O
  return false;
}

}  // namespace Observation
