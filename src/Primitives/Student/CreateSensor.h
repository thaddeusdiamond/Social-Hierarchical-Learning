/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Sensor used by the QLearner
 */

#ifndef _SHL_PRIMITIVES_STUDENT_CREATESENSOR_H_
#define _SHL_PRIMITIVES_STUDENT_CREATESENSOR_H_

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include "Common/Utils.h"
#include "Student/Sensor.h"
#include "Proto/CreateController.pb.h"

namespace Primitives {

using std::string;
using Utils::Die;

class CreateSensor : public Sensor {
 public:
  /**
   * The constructor is responsible for setting up a connection to the off-
   * board controller and begin a thread for polling/sending
   *
   * @param   port            The port to listen for controller sensor data on
   * @param   recipient_ipv4  The IPv4 address the Create controller is located
   * @param   recipient_port  Port of Create controller to send msgs to
   **/
  CreateSensor(int port, int recipient_ipv4, int recipient_port);

  /**
   * Default destructor, does nothing
   **/
  virtual ~CreateSensor() {
    pthread_join(controller_thread_, NULL);
  }

  virtual bool SetValues(double const * const values, int num_values);
  virtual double const * const GetValues();

  /**
   * The ListenOnSocket() method is called by the CommunicateWithCreate() method
   * to start the listener and UDP receiver
   **/
  void ListenOnSocket();

 protected:
  virtual bool Poll();

 private:
  /**
   * Sensor plaintext name
   **/
  char const * name_;

  /**
   * Current values
   **/
  double * values_;

  /**
   * Number of values encapsulated by sensor
   **/
  int num_values_;

  /**
   * Max allowed value for sensor
   **/
  double max_value_;

  /**
   * Min allowed value for sensor
   **/
  double min_value_;

  /**
   * Describes the number of decimal places to record (truncate after # digits)
   **/
  int significant_figures_;

  /**
   * Describes the max variation to search when looking for "nearby" values
   **/
  double nearby_threshold_;

  /**
   * Keep track of whether we've recently received anything
   **/
  bool received_;

  /**
   * Maintain the socket we are sending/receiving sensor messages on
   **/
  int controller_socket_;

  /**
   * Since we send commands we need to have a peer struct to send on
   **/
  struct sockaddr* controller_address_;

  /**
   * We keep track of the thread performing the sensor msg I/O asynchronously
   **/
  pthread_t controller_thread_;
};

// Due to pthread issues we create the poller as a non-member function
void* CommunicateWithCreate(void* object) {
  CreateSensor* create_sensor = reinterpret_cast<CreateSensor*>(object);
  create_sensor->ListenOnSocket();
  return NULL;
}

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_STUDENT_CREATESENSOR_H_
