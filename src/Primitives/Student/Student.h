/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for any reinforcement learning mechanism
 * operating in the primitives learning phase of SHL
 **/

#ifndef _SHL_PRIMITIVES_STUDENT_STUDENT_H_
#define _SHL_PRIMITIVES_STUDENT_STUDENT_H_

#include <string>
#include <vector>
#include <cstdlib>
#include "Student/Sensor.h"
#include "QLearner/QLearner.h"

using std::string;
using std::vector;

namespace Primitives {

/**
 * @todo Move the definition of Key out of Student.h and into somewhere common
 *       or relating to the database layer
 **/
#define Key std::string

class Object;
class QLearner;

class Student {
 public:
  /**
   * Destructor for a Student must free all memory it received from I/O and
   * had buffered, also release the STUDENT it controls
   **/
  virtual ~Student() {}

  /**
   * Before the student begins its training session, it should know about its
   * immediate environment through all available sensors.
   *
   * @param     sensors A pointer to a set of 'environment' sensors
   *
   * @return    True unless an error occurred in setup
   **/
  virtual bool SetEnvironment(vector<Sensor*> const &sensors) = 0;

  /**
   * Sets the internal list of motors that the student has access to. These
   * should all be motors the student has control of, meaning can be set
   * as well as read.
   *
   * @param     motors A pointer to a set of 'motor' sensors
   *
   * @return    True unless an error occurred in setup
   **/
  virtual bool SetMotors(vector<Sensor*> const &motors) = 0;

  /**
   * Assigns this student a sensor whose value is to be interpreted as
   * a reinforcement signal. It will be polled along with the environment
   * and self sensors, but its value will be passed to the CreditAssignment
   * object of the internal QLearner.
   *
   **/
  virtual bool SetFeedbackHandler(Sensor const &feedback_device) = 0;

  /**
   * We explicitly alert the student, which can be used on a one-off basis or
   * reused between trials, that a specific primitive is now being trained
   * against. If primitive_id doesn't identify any stored primitives, a new
   * QLearner is created and stored in the primitives_ vector.
   *
   * @param     primitive_id       A string identifying the primitive to learn
   * @param     active_motors      A vector of sensors (motors) involved
   *                               in trial
   * @param     active_sensors     A vector of sensors (on agent) involved
   *                               in trial
   * @param     active_environment A vector of sensors (environ) involved
   *                               in trial
   * @param     objects       A vector of objects
   *
   * @return    True if the STUDENT succeeds in task, fail otherwise
   **/
  virtual bool Learn(std::string primitive_id,
                     std::vector<Sensor*> active_motors,
                     std::vector<Sensor*> active_sensors,
                     std::vector<Sensor*> active_environment,
                     std::vector<Object> objects) = 0;

  /**
   * Exits the learning state that is entered by calling the Learn function
   *
   * @return True if successfully stopped, false on error
   **/
  virtual bool StopLearning(void) = 0;

  /**
   * Returns whether or not the student has a skill matching the given name
   * @param name String identifying the skill to be searched
   * @return Pointer to QLearner if found, NULL otherwise
   **/
  virtual QLearner* GetSkill(std::string name) {
    vector<QLearner*>::iterator iter;
    for (iter = primitives_.begin(); iter != primitives_.end(); ++iter) {
      std::string const &p_name = (*iter)->get_name();
      if (p_name.compare(name) == 0) return *iter;
    }
    return NULL;
  }

  /**
   * Adds the provided QLearner as a skill in the primitives vector
   * @param skill The skill to be added. Is not copied - uses passed pointer
   * @return true on success, false if skill with same name exists already
   **/
  virtual bool AddSkill(QLearner* skill) {
    if (GetSkill(skill->get_name())) return false;
    primitives_.push_back(skill);
    return true;
  }

  /**
   * Accessor for sensors associated with this student
   **/
  virtual vector<Sensor*> const * const  get_sensors() const {
    return &sensors_;
  };

  /**
   * Accessor for sensors this student's environment
   **/
  virtual vector<Sensor*> const * const  get_environment() const {
    return &environment_;
  };

  /**
   * Accessor for sensors this student's environment
   **/
  virtual vector<Sensor*> const * const  get_motors() const { return &motors_; }

  /**
   * Accessor for primitives this student knows
   **/
  virtual vector<QLearner*> * const get_primitives() {
    return &primitives_;
  }


  virtual bool IsStateAcceptable(State *) {
    return true;
  }

 protected:
  std::vector<Sensor*> sensors_;
  std::vector<Sensor*> environment_;
  std::vector<Sensor*> motors_;
  std::vector<QLearner*> primitives_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_STUDENT_STUDENT_H_
