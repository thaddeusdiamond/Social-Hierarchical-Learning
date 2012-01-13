/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a dummy student that can learn
 **/

#include "Student/Dummy.h"

namespace Primitives {

bool Dummy::SetEnvironment(std::vector<Sensor*> const &sensors) {
  return true;
}

bool Dummy::SetMotors(std::vector<Sensor*> const &motors) {
  return true;
}

bool Dummy::SetFeedbackHandler(Sensor const &feedback_device) {
  return true;
}

bool Dummy::Learn(std::string primitive_id, std::vector<Sensor*> active_motors,
                   std::vector<Sensor*> active_sensors,
                   std::vector<Sensor*> active_environment,
                   std::vector<Object> objects) {
  return true;
}

bool Dummy::StopLearning(void) {
  return true;
}

}  // namespace Primitives
