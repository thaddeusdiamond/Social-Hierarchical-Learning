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

#include "Student/FeedbackSensor.h"

namespace Primitives {

FeedbackSensor::FeedbackSensor(int port)
    : name_(string("Feedback").c_str()), num_values_(1), received_(false) {
  // Create vanilla socket
  polling_socket_ = socket(PF_INET, SOCK_DGRAM, 0);
  if (polling_socket_ < 0)
    Die("Could not create a polling socket");

  // Bind the socket to listen
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = PF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
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
  pthread_create(&polling_thread_, NULL, &PollForFeedback, this);
}

void FeedbackSensor::ListenOnSocket() {
  while (true) {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    recv(polling_socket_, buffer, sizeof(buffer), 0);

    received_ = true;
    double status[1] = { buffer[0] - '0' };
    values_ = status;
  }
}

bool FeedbackSensor::SetValues(double const * const values, int num_values) {
  // This sensor cannot be set
  return false;
}

double const * const FeedbackSensor::GetValues() {
  // Return a one-element array to most recent value
  Poll();
  return values_;
}

bool FeedbackSensor::Poll() {
  if (received_) {
    received_ = false;
    return true;
  }

  return false;
}

}  // namespace Primitives
