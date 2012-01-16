/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is the implementation for sensor feedback
 */

/** @todo This should really have a unit test, but it's not super important
 *        since this is a pretty dumb sensor **/

#include "Student/KinectSensor.h"

namespace Primitives {

KinectSensor::KinectSensor(int port)
    : Sensor(string("Kinect")), values_(NULL), num_values_(6),
      received_(false), file_handle_(NULL) {
  // Create vanilla socket
  polling_socket_ = socket(PF_INET, SOCK_STREAM, 0);
  if (polling_socket_ < 0)
    Die("Could not create a polling socket");

  // Bind the socket to listen
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = PF_INET;
  server.sin_addr.s_addr = 786965632;
  server.sin_port = htons(port);
  if (bind(polling_socket_, reinterpret_cast<struct sockaddr*>(&server),
           sizeof(server)))
    Die("Could not bind socket to specified port");

  // Set the socket to be reusable
  int on = 1;
  if (setsockopt(polling_socket_, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<char*>(&on), sizeof(on)) < 0)
    Die("Failed to make socket reusable");

  // Spawn thread to poll
  pthread_create(&polling_thread_, NULL, &PollForKinect, this);
}

void KinectSensor::ListenOnSocket() {
  // Start listening
  if (listen(polling_socket_, 4096) < 0)
    Die("Couldn't listen on the socket");

  // Handle signal interrupts until we explicitly quit
  Signal::ResetProgram();
  signal(SIGINT, &Signal::StopProgram);

  // Take the first connection as the Kinect device
  int spawned_socket;
  Log(stderr, DEBUG, "Waiting for a connection from Kinect...");
  if ((spawned_socket = accept(polling_socket_, NULL, 0)) < 0)
    Die("There was an error accepting the connection");
  Log(stderr, SUCCESS, "Socket connected!  Hit ctrl-c to break");

  // Make the socket non-blocking
  int flags = fcntl(spawned_socket, F_GETFL, 0);
  if (fcntl(spawned_socket, F_SETFL, flags | O_NONBLOCK) < 0)
    Die("Failed to make socket non-blocking");

  // Set the socket to be reusable
  int on = 1;
  if (setsockopt(spawned_socket, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<char*>(&on), sizeof(on)) < 0)
    Die("Failed to make socket reusable");

  // Main receiver loop
  do {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    int bytes_received;
    if ((bytes_received = recv(spawned_socket, buffer, 4096, 0)) > 0) {
      KinectData* data = new KinectData();
      received_ = true;
      if (!data->ParseFromArray(buffer, bytes_received))
        Log(stderr, ERROR, "Failed to deserialize");

      // Assume only one active person at a time, calculate body pose
      for (int i = 0; i < data->people_size(); i++) {
        if (data->people(i).active()) {
          double new_values[6] = {
            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HAND_LEFT).x() -
            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HEAD).x(),

            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HAND_LEFT).y() -
            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HEAD).y(),

            static_cast<double>(
              data->people(i).skeleton_positions(
                NUI_SKELETON_POSITION_HAND_LEFT).z() -
              data->people(i).skeleton_positions(
                NUI_SKELETON_POSITION_HEAD).z()),

            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HAND_RIGHT).x() -
            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HEAD).x(),

            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HAND_RIGHT).y() -
            data->people(i).skeleton_positions(
              NUI_SKELETON_POSITION_HEAD).y(),

            static_cast<double>(
              data->people(i).skeleton_positions(
                NUI_SKELETON_POSITION_HAND_RIGHT).z() -
              data->people(i).skeleton_positions(
                NUI_SKELETON_POSITION_HEAD).z())
          };
          values_ = new_values;

          if (file_handle_ != NULL) {
            fprintf(file_handle_, "%5.5f,%5.5f,%5.5f,%5.5f,%5.5f,%5.5f\n",
              values_[0], values_[1], values_[2],
              values_[3], values_[4], values_[5]);
          }
        }
      }
    }
  } while (Signal::ProgramIsRunning);

  // Cleanup
  close(spawned_socket);
}

bool KinectSensor::SetValues(double const * const values, int num_values) {
  // This sensor cannot be set
  return false;
}

void KinectSensor::set_file_handle(char const * filename) {
  if (file_handle_ != NULL)
    fclose(file_handle_);
  file_handle_ = fopen(filename, "w");
} 

double const * const KinectSensor::GetValues() {
  // Return a one-element array to most recent value
  Poll();
  return values_;
}

bool KinectSensor::Poll() {
  if (received_) {
    received_ = false;
    return true;
  }

  return false;
}

}  // namespace Primitives
