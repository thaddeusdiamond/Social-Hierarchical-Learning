/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This executable creates a KinectSensor and records data for the
 * ASL primitives in our AAAI paper to file.
 **/

#include "Student/KinectSensor.h"

using Primitives::KinectSensor;
using Utils::Log;

static int NUMBER_OF_PRIMITIVES = 8;
static string PRIMITIVE_LIST[] = {
  "coat", "shoes", "school", "store", "drive", "with", "in", "out"
};

int main(int argc, char* argv[]) {
  KinectSensor* sensor_ = new KinectSensor(18000);  

  for (int i = 0; i < NUMBER_OF_PRIMITIVES; i++) {
    // Set up the file handle
    string primitive = PRIMITIVE_LIST[i];
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

  delete sensor_;
  return true;
}
