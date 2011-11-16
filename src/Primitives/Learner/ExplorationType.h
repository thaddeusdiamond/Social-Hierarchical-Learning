/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Exploration Function Implementation
 */

#ifndef _SHL_SRC_PRIMITIVES_LEARNER_EXPLORATIONTYPE_H_
#define _SHL_SRC_PRIMITIVES_LEARNER_EXPLORATIONTYPE_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class QLearner;
class Sensor;

class ExplorationType {
 public:
  /**
   * Initialize back-pointer to parent QLearner object
   * 
   * @param learner QLearner object associated with this ExplorationType
   */
  virtual ExplorationType(QLearner * const learner) {
    learner_ = learner;
  }

  /**
   * Destructor for ExplorationType must free all memory it allocated
   * interally, but is not responsible for freeing anything passed into it.
   */
  virtual ~ExplorationType() {}

  /**
   * Given the current state, return a list of possible next states
   *
   * @param     current_state   System state to explore around
   * @param     next_states     Plausible next states for the system
   *
   * @return    True on success, false on failure.
   */
  virtual bool getNextSteps(State const& current_state,
                            std::vector<State const* const> &next_states) = 0;

 private:
  /*
   * Disable default constructor
   */
  ExplorationType() {}

  /**
   * Reference back to the parent QLearner object
   */
  QLearner *learner_;
}


#endif  // _SHL_SRC_PRIMITIVES_LEARNER_EXPLORATIONTYPE_H_
