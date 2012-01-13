/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a student that learns by ingesting recorded demonstrations of
 * primitives, turning them into skills.
 **/

#ifndef _SHL_PRIMITIVES_STUDENT_LBDSTUDENT_H_
#define _SHL_PRIMITIVES_STUDENT_LBDSTUDENT_H_

#include <string>
#include <vector>
#include <cstdlib>
#include "Student/Student.h"

using std::string;
using std::vector;

namespace Primitives {

class LBDStudent : public Student {
 public:
/**
   * Destructor for a Student must free all memory it received from I/O and
   * had buffered, also release the STUDENT it controls
   **/
  virtual ~LBDStudent() {}

  /**
   * No functionality in this student type
   * @param sensors
   * @return True
   **/
  virtual bool SetEnvironment(vector<Sensor*> const &sensors) { return true; }

  /**
   * No functionality in this student type
   * @param motors
   * @return True
   **/
  virtual bool SetMotors(vector<Sensor*> const &motors) { return true; }

  /**
   * No functionality in this student type
   * @return True
   **/
  virtual bool SetFeedbackHandler(Sensor const &feedback_device) {
    return true;
  }

  /**
   * No functionality in this student type
   *
   * @param     primitive_id
   * @param     active_motors
   * @param     active_sensors
   * @param     active_environment
   * @param     objects
   * @return    True
   **/
  virtual bool Learn(std::string primitive_id,
                     std::vector<Sensor*> active_motors,
                     std::vector<Sensor*> active_sensors,
                     std::vector<Sensor*> active_environment,
                     std::vector<Object> objects) { return true; }

  /**
   * No functionality in this student type
   *
   * @return True
   **/
  virtual bool StopLearning(void) { return true; }

  /**
  * Trains a QLearner based on file input. File must follow this format:
  * Line 1: "Unique Skill Name"
  * Line 2: Min Increment Value for each sensor
  * Line 3: Nearby-threshold Value for each sensor
  * Lines 4+: Comma separated sensor values
  * @param filename File to learn skill from
  * @return
  **/
  virtual QLearner *LearnSkillFromFile(std::string filename);

 private:
  /**
   * Adds more data to a QLearner based on file input.
   * File must follow this format:
   * Line 1: "Unique Skill Name"
   * Line 2: Min Increment Value for each sensor
   * Line 3: Nearby-threshold Value for each sensor
   * Lines 4+: Comma separated sensor values
   * @param filename File to learn skill from
   * @return
   **/
  virtual void AddTrainingToSkill(std::string primitive_id,
                                  std::string filename);
};


}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_STUDENT_LBDSTUDENT_H_
