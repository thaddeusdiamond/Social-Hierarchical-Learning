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

bool Dummy::SetLearningMethod(const QLearner& learner) {
  return true;
}

bool Dummy::SetFeedbackHandler(Sensor const &feedback_device) {
  return true;
}

bool Dummy::LoadComparables(Primitive* prim, Key* tables, size_t tables_len) {
  return true;
}


bool Dummy::Learn(Primitive* prim, Motor* active_motors, size_t motors_len,
          Object* objects, size_t objects_len) {
  return true;
}

bool Dummy::StopLearning(void) {
  return true;
}

}  // namespace Primitives
