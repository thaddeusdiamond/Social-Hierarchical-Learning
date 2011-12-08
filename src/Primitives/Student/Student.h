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

using std::string;
using std::vector;

/**
 * @todo Move the definition of Key out of Student.h and into somewhere common
 *       or relating to the database layer
 **/
#define Key std::string

class Object;
class Primitive;
class Motor;
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
  virtual bool SetEnvironment(std::vector<Sensor*> const &sensors) = 0;

  /**
   * Sets the internal list of motors that the student has access to. These
   * should all be motors the student has control of, meaning can be set
   * as well as read.
   *
   * @param     motors A pointer to a set of 'motor' sensors
   *
   * @return    True unless an error occurred in setup
   **/
  virtual bool SetMotors(std::vector<Sensor*> const &motors) = 0;

  /**
   * Assigns this student a sensor whose value is to be interpreted as
   * a reinforcement signal. It will be polled along with the environment
   * and self sensors, but its value will be passed to the CreditAssignment
   * object of the internal QLearner.
   * 
   **/
  virtual bool SetFeedbackHandler(Sensor const &feedback_device) = 0;

  /**
   * Sets the learning method to use
   *
   * @param learner QLearner or compatible class to perform learning
   *
   * @return True if compatible learner provided, false if not
   **/
  virtual bool SetLearningMethod(QLearner const &learner) = 0;

  /**
   * Sometimes we will try to perform AB testing with the student, but in order
   * to do so we will need to explicitly go to the datastore and get that
   * learned Q-Table.
   *
   * @param     primitive     The primitive whose alternate tables are to be
   *                          compared
   * @param     tables        A set of IDs for the Q-Tables we want to load from
   *                          within the provided primitive
   * @param     tables_len    How many tables we are performing a comparison w/
   *
   * @return    True unless an error occurred during load time
   **/
  virtual bool LoadComparables(Primitive* primitive, Key* tables,
                               size_t tables_len) = 0;

  /**
   * We explicitly alert the student, which can be used on a one-off basis or
   * reused between trials, that a specific primitive is now being trained
   * against.
   *
   * @param     prim          A pointer to a primitive to train against
   * @param     active_motors A pointer to a set of motors involved in trial
   * @param     motors_len    How many motors are expected to be used
   * @param     objects       A pointer to a set of objects
   * @param     objects_len   How many objects have been passed to the setup
   *
   * @return    True if the STUDENT succeeds in task, fail otherwise
   **/
  virtual bool Learn(Primitive* prim, Motor* active_motors,
                     size_t motors_len, Object* objects,
                     size_t objects_len) = 0;

  /**
   * Exits the learning state that is entered by calling the Learn function
   *
   * @return True if successfully stopped, false on error
   **/
  virtual bool StopLearning(void) = 0;


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

 protected:
  std::vector<Sensor*> sensors_;
  std::vector<Sensor*> environment_;
  std::vector<Sensor*> motors_;
};

#endif  // _SHL_PRIMITIVES_STUDENT_STUDENT_H_
