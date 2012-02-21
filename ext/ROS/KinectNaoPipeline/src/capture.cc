/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This executable in the ROS package is responsible for capturing TF frames
 * up until a specified time.
 **/

#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <cstdio>
#include "utils.h"

using ROSUtils::die;

int main(int argc, char* argv[]) {
  // Std call to ROS_INIT, making of a subscriber node
  ros::init(argc, argv, "knp_capture");
  ros::NodeHandle node;
  ros::Rate loop_rate(LOOP_RATE);
  tf::TransformListener listener;

  // Prompt user for filename
  fprintf(stdout, "Please enter the name of the file to write to: ");
  char c, filename[4096];
  int i = 0;
  while ((c = getchar()) != '\n')
    filename[i++] = c;
  filename[i] = '\0';

  // Try to open that file
  FILE* file_handle;
  if (!(file_handle = fopen(filename, "w")))
    die(EXIT_FAILURE, "Could not open specified file.\n");

  // Wait on the user to start the capture
  fprintf(stdout, "OPENED.  Press any button to begin capturing...");
  getchar();

  // Run the actual TF capture
  fprintf(stdout, "Beginning Kinect capture, press CTRL-C to exit.\n");
  while (ros::ok()) {
    // Want to get the x, y, and z of appropriate joints
    tf::StampedTransform* transforms = new tf::StampedTransform[NumJoints];
    try {
      for (int i = 0; i < NumJoints; i++) {
        listener.lookupTransform(JointRef, Joints[i] + suffix,
                                 ros::Time(0), transforms[i]);
        fprintf(file_handle, "%5.3f,%5.3f,%5.3f,", transforms[i].getOrigin().x(),
                transforms[i].getOrigin().y(), transforms[i].getOrigin().z());
      }
      fprintf(file_handle, "\n");
    } catch (...) {
      ROS_WARN("Could not get tf capture");
    }
    delete[] transforms;
  
    // Sleep for appropriate 30Hz duration
    ros::spinOnce();
    loop_rate.sleep();
  }

  die(EXIT_SUCCESS, "Data successfully recorded!\n");
}
