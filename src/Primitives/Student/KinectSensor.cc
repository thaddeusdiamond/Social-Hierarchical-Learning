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
    : Sensor(string("Kinect")), values_(NULL), received_(false) {
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
  fprintf(stdout, "Waiting for a connection from Kinect...\n");
  if ((spawned_socket = accept(polling_socket_, NULL, 0)) < 0)
    Die("There was an error accepting the connection");
  fprintf(stdout, "Socket connected!  Hit ctrl-c to break\n");

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
      fprintf(stdout, "Received %d bytes\n", bytes_received);

      KinectData* data = new KinectData();
      if (!data->ParseFromArray(buffer, bytes_received))
        fprintf(stdout, "Failed to deserialize\n");

      /// @todo: How does the person know who is active, and how easy access?
      double* new_val = new double[3 * NUI_SKELPOS_COUNT * data->people_size()];
      num_values_ = 3 * NUI_SKELPOS_COUNT * data->people_size();

      for (int i = 0; i < data->people_size(); i++) {
        for (int j = 0; j < NUI_SKELPOS_COUNT; j++) {
          // Person had tracking data
          if (data->people(i).active()) {
            new_val[(i * NUI_SKELPOS_COUNT) + (j * 3)] =
              data->people(i).skeleton_positions(j).x();
            new_val[(i * NUI_SKELPOS_COUNT) + (j * 3) + 1] =
              data->people(i).skeleton_positions(j).y();
            new_val[(i * NUI_SKELPOS_COUNT) + (j * 3) + 2] =
              static_cast<double>(data->people(i).skeleton_positions(j).z());

          // Person did not have tracking data
          } else {
            new_val[(i * NUI_SKELPOS_COUNT) + (j * 3)] =
            new_val[(i * NUI_SKELPOS_COUNT) + (j * 3) + 1] =
            new_val[(i * NUI_SKELPOS_COUNT) + (j * 3) + 2] =
              -1 * DBL_MAX;
          }
        }
      }

      // Update the member variables (const wierdness)
      if (values_)
        delete[] values_;
      values_ = new_val;
      received_ = true;
    }
  } while (Signal::ProgramIsRunning);

  // Cleanup
  close(spawned_socket);
}

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
  if (received_) {
    received_ = false;
    return true;
  }

  return false;
}

}  // namespace Primitives
