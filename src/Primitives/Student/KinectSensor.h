/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a sensor that receives feedback from the human controller.
 */

#ifndef _SHL_PRIMITIVES_STUDENT_KINECTSENSOR_H_
#define _SHL_PRIMITIVES_STUDENT_KINECTSENSOR_H_

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <float.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "Proto/KinectData.pb.h"
#include "Common/Utils.h"
#include "Common/Signal.h"
#include "Student/Sensor.h"

namespace Primitives {

using std::string;
using Utils::Die;

class KinectSensor : public Sensor {
 public:
  /**
   * The constructor for a KinectSensor takes a port and spawns a thread
   * to poll on that port for TCP packets.
   *
   * @param   port            The port to listen for controller sensor data on
   **/
  explicit KinectSensor(int port);

  /**
   * The destructor for a KinectSensor must free all memory aggregated
   **/
  virtual ~KinectSensor() {
    close(polling_socket_);
    pthread_join(polling_thread_, NULL);
  }

  /**
   * The skeleton positions are defined according to the following enum
   * (taken from the Kinect API):
   **/
  enum _NUI_SKELETON_POSITION_INDEX {
    NUI_SKELETON_POSITION_HIP_CENTER = 0,
    NUI_SKELETON_POSITION_SPINE,
    NUI_SKELETON_POSITION_SHOULDER_CENTER,
    NUI_SKELETON_POSITION_HEAD,
    NUI_SKELETON_POSITION_SHOULDER_LEFT,
    NUI_SKELETON_POSITION_ELBOW_LEFT,
    NUI_SKELETON_POSITION_WRIST_LEFT,
    NUI_SKELETON_POSITION_HAND_LEFT,
    NUI_SKELETON_POSITION_SHOULDER_RIGHT,
    NUI_SKELETON_POSITION_ELBOW_RIGHT,
    NUI_SKELETON_POSITION_WRIST_RIGHT,
    NUI_SKELETON_POSITION_HAND_RIGHT,
    NUI_SKELETON_POSITION_HIP_LEFT,
    NUI_SKELETON_POSITION_KNEE_LEFT,
    NUI_SKELETON_POSITION_ANKLE_LEFT,
    NUI_SKELETON_POSITION_FOOT_LEFT,
    NUI_SKELETON_POSITION_HIP_RIGHT,
    NUI_SKELETON_POSITION_KNEE_RIGHT,
    NUI_SKELETON_POSITION_ANKLE_RIGHT,
    NUI_SKELETON_POSITION_FOOT_RIGHT,
    NUI_SKELPOS_COUNT
  } NUI_SKELETON_POSITION_INDEX;

  virtual bool SetValues(double const * const values, int num_values);
  virtual double const * const GetValues();

  /**
   * The ListenOnSocket() method is called by the PollForFeedback() method
   * to start the listener and TCP receiver
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
static inline void* PollForKinect(void* object) {
  KinectSensor* feedback_sensor = reinterpret_cast<KinectSensor*>(object);
  feedback_sensor->ListenOnSocket();
  return NULL;
}

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_STUDENT_KINECTSENSOR_H_
