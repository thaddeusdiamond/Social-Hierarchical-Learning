/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is the implementation for create controller/sensor
 */

/** @todo This should really have a unit test, but it's not super important
 *        since this is a pretty dumb sensor **/

#include "Student/CreateSensor.h"

namespace Primitives {

CreateSensor::CreateSensor(int port, int recipient_ipv4, int recipient_port)
    : Sensor(std::string("Create Controller")), num_values_(4),
    received_(false)  {
  /** @todo max/min values, sig figs, and nearby thresholds need to be set **/

  // Create vanilla socket
  controller_socket_ = socket(PF_INET, SOCK_DGRAM, 0);
  if (controller_socket_ < 0)
    Die("Could not create a Create I/O socket");

  // Bind the socket to listen
  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = PF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  if (bind(controller_socket_, reinterpret_cast<struct sockaddr*>(&server),
           sizeof(server)))
    Die("Could not bind Create socket to specified port");

  // Set the socket to be reusable
  int on = 1;
  if (setsockopt(controller_socket_, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<char*>(&on), sizeof(on)) < 0)
    Die("Failed to make Create socket reusable");

  // Who are we communicating with?
  struct sockaddr_in controller_ipv4;
  memset(&controller_ipv4, 0, sizeof(controller_ipv4));
  controller_ipv4.sin_family = PF_INET;
  controller_ipv4.sin_addr.s_addr = recipient_ipv4;
  controller_ipv4.sin_port = htons(recipient_port);
  controller_address_ = reinterpret_cast<struct sockaddr*>(&controller_ipv4);

  // Spawn thread to poll
  pthread_create(&controller_thread_, NULL, &CommunicateWithCreate, this);
}

void CreateSensor::ListenOnSocket() {
  while (true) {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    recv(controller_socket_, buffer, sizeof(buffer), 0);
    received_ = true;

    CreateController* controller = new CreateController();
    assert(controller->ParseFromString(buffer));
    double new_values[4] =
      { controller->right_motor(), controller->left_motor(),
        controller->right_led(), controller->left_led() };
    values_ = new_values;
  }
}

bool CreateSensor::SetValues(double const * const values, int num_values) {
  // Must be exactly four motors (R/L and two LEDs)
  if (num_values != 4)
    return false;

  // Create a serializable protobuf and send it
  CreateController* controller = new CreateController();
  controller->set_right_motor(values[0]);
  controller->set_left_motor(values[1]);
  controller->set_right_led(values[2]);
  controller->set_left_led(values[3]);

  // Send on wire
  string message;
  assert(controller->SerializeToString(&message));
  sendto(controller_socket_, message.c_str(), message.length() + 1, 0,
         controller_address_, sizeof(*controller_address_));
  return true;
}

double const * const CreateSensor::GetValues() {
  // Return a one-element array to most recent value
  Poll();
  return values_;
}

bool CreateSensor::Poll() {
  if (received_) {
    received_ = false;
    return true;
  }

  return false;
}

}  // namespace Primitives
