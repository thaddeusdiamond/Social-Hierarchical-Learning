/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a sensor that receives feedback from the human controller.
 */

#ifndef _SHL_PRIMITIVES_STUDENT_FEEDBACKSENSOR_H_
#define _SHL_PRIMITIVES_STUDENT_FEEDBACKSENSOR_H_

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "Common/Utils.h"
#include "Student/Sensor.h"

using std::string;
using Utils::Die;

class FeedbackSensor : public Sensor {
 public:
  /**
   * The constructor for a FeedbackSensor takes a port and spawns a thread
   * to poll on that port for UDP packets.
   **/
  explicit FeedbackSensor(int port);

  /**
   * The destructor for a FeedbackSensor must free all memory aggregated
   **/
  virtual ~FeedbackSensor() {
    pthread_join(polling_thread_, NULL);
  }

  /**
   * Possible values for feedback (must match the FeedbackController.java enum)
   **/
  enum {
    GOOD = 0,
    BAD = 1,
    PANIC = 2,
    DONE = 3
  };

  virtual bool SetValues(double const * const values, int num_values);
  virtual double const * const GetValues();

  /**
   * The ListenOnSocket() method is called by the PollForFeedback() method
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
   * Most recent value
   **/
  double const * values_;

  /**
   * Number of values encapsulated by sensor
   **/
  int num_values_;

  /**
   * Keep track of whether we've recently received anything
   **/
  bool received_;

  /**
   * Maintain the socket we are polling for feedback on
   **/
  int polling_socket_;

  /**
   * We keep track of the thread performing the sensor polling asynchronously
   **/
  pthread_t polling_thread_;
};

// Due to pthread issues we create the poller as a non-member function
void* PollForFeedback(void* object) {
  FeedbackSensor* feedback_sensor = reinterpret_cast<FeedbackSensor*>(object);
  feedback_sensor->ListenOnSocket();
  return NULL;
}

#endif  // _SHL_PRIMITIVES_STUDENT_FEEDBACKSENSOR_H_
