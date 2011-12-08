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
  virtual bool SetLearningMethod(QLearner const &learner);
  virtual bool SetFeedbackHandler(Sensor const &feedback_device);
  virtual bool LoadComparables(Primitive* prim, Key* tables, size_t tables_len);
  virtual bool Learn(Primitive* prim, Motor* active_motors,
                     size_t motors_len, Object* objects,
                     size_t objects_len);
  virtual bool StopLearning(void);
};

#endif  // _SHL_PRIMITIVES_STUDENT_DUMMY_H_
