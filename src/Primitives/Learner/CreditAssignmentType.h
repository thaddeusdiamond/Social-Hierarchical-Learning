/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Credit Assignment Function 
 * Implementation
 */

#ifndef _SHL_PRIMITIVES_LEARNER_CREDITASSIGNMENTTYPE_H_
#define _SHL_PRIMITIVES_LEARNER_CREDITASSIGNMENTTYPE_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class QLearner;
class Sensor;

class CreditAssignmentType {
 public:
  /**
   * Destructor for CreditAssignmentType must free all memory it allocated
   * interally, but is not responsible for freeing anything passed into it.
   */
  virtual ~CreditAssignmentType() {}

  /**
   * Given the current state and feedback received, apply the reward across
   * a set of relevant states as determined by this object
   *
   * @param     current_state   System state to explore around
   * @param     next_states     Plausible next states for the system
   *
   * @return    True on success, false on failure.
   */
  virtual bool GetNextSteps(State const& current_state,
                            std::vector<State const* const> &next_states) = 0;

 protected:
  /**
   * Initialize back-pointer to parent QLearner object
   * 
   * @param learner QLearner object associated with this CreditAssignmentType
   **/
  explicit CreditAssignmentType(QLearner * const learner) {
    learner_ = learner;
  }

 private:
  /*
   * Disable default constructor
   */
  CreditAssignmentType() {}

  /**
   * Reference back to the parent QLearner object
   */
  QLearner *learner_;
};


#endif  // _SHL_PRIMITIVES_LEARNER_CREDITASSIGNMENTTYPE_H_
