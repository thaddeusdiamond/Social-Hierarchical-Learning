/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a dummy student while we finish enumerating
 * functions/features.
 **/

#ifndef _SHL_PRIMITIVES_STUDENT_DUMMY_H_
#define _SHL_PRIMITIVES_STUDENT_DUMMY_H_

#include <string>
#include <vector>
#include "Common/Utils.h"
#include "QLearner/QLearner.h"
#include "Student/Student.h"

namespace Primitives {

using std::string;
using std::vector;

class Dummy : public Student {
 public:
  /**
   * The default constructor initializes a null learner object
   **/
  explicit Dummy() {}

  /**
   * Destructor for dummy student must free all memory it received from I/O and
   * had buffered
   **/
  virtual ~Dummy() {}

  virtual bool SetEnvironment(std::vector<Sensor*> const &sensors);
  virtual bool SetMotors(std::vector<Sensor*> const &motors);
  virtual bool SetFeedbackHandler(Sensor const &feedback_device);
  virtual bool Learn(std::string primitive_id,
                     std::vector<Sensor*> active_motors,
                     std::vector<Sensor*> active_sensors,
                     std::vector<Sensor*> active_environment,
                     std::vector<Object> objects);
  virtual bool StopLearning(void);
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_STUDENT_DUMMY_H_
