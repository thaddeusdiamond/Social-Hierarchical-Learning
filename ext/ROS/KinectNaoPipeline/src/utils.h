/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Simple set of utilities for our ROS package
 **/

#include <string>

using std::string;

#define SIZE_OF_LINE 4096
#define LOOP_RATE 30

// Kinect on ROS can blow
static string suffix = "_1";

// How many joint angles we're recording, the output
// is in the format of X,Y,Z for each joint specified in Joints
static const int NumJoints = 6;
static string JointRef = "openni_depth_frame";
static string Joints[] = {
  "right_hand", "right_elbow", "right_shoulder",
  "left_hand", "left_elbow", "left_shoulder"
};

namespace ROSUtils {

static inline void die(int return_code, const char* format, ...) {
  va_list arguments;
  va_start(arguments, format);
  fprintf(stdout, format, arguments);
  exit(return_code);
}

}  // namespace ROSUtils
