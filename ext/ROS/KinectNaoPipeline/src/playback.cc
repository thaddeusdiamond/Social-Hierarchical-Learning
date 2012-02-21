/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This executable in the ROS package is responsible for playing back TF
 * frames from a file that has already been captured.
 **/

#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <cstdio>
#include <vector>
#include "utils.h"

using ROSUtils::die;
using std::vector;

int main(int argc, char* argv[]) {
  // Std call to ROS_INIT, making of a publisher node
  ros::init(argc, argv, "knp_playback");
  ros::NodeHandle n;
  ros::Rate loop_rate(LOOP_RATE);
  tf::TransformBroadcaster broadcaster;

  // Prompt user for filename
  fprintf(stdout, "Please enter the name of the file being played back: ");
  char c, filename[4096];
  int i = 0;
  while ((c = getchar()) != '\n')
    filename[i++] = c;
  filename[i] = '\0';

  // Try to open that file
  FILE* file_handle;
  if (!(file_handle = fopen(filename, "r")))
    die(EXIT_FAILURE, "Could not open specified file.\n");

  // Wait on the user to start the capture
  fprintf(stdout, "LOADED.  Press any button to begin playback...");
  getchar();

  // Run the actual TF capture
  fprintf(stdout, "Beginning playback, press CTRL-C to exit.\n");

  // Get the next line from the file
  char* line = new char[SIZE_OF_LINE];
  while (fgets(line, SIZE_OF_LINE, file_handle)) {
    // Use strtod to read in CSV values
    char *leftover,
         *line_reader = line;
    double* values = new double[NumJoints * 3];
    for (int i = 0; i < NumJoints * 3; i++) {
      values[i] = strtod(line_reader, &leftover);
      if (i < NumJoints * 3 - 1 && !leftover)
        die(EXIT_FAILURE, "The file format was incorrect\n");
      line_reader = leftover + 1;
    }

    // Use transform broadcaster
    vector<tf::StampedTransform> transforms;
    for (int i = 0; i < NumJoints; i++) {
      tf::Transform transform;
      transform.setRotation(tf::Quaternion(0, 0, 0));
      transform.setOrigin(
        tf::Vector3(values[i * 3], values[i * 3 + 1], values[i * 3 + 2])); 
      transforms.push_back(
        tf::StampedTransform(transform, ros::Time::now(), JointRef, Joints[i]));
    }
    broadcaster.sendTransform(transforms);

    // Free used memory and sleep for appropriate 30Hz duration
    delete[] values;
    ros::spinOnce();
    loop_rate.sleep();
  }

  delete[] line;
  die(EXIT_SUCCESS, "Data successfully played back!\n");
}
