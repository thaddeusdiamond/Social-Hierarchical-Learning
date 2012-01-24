/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Exploration Function Implementation
 **/

#ifndef _SHL_PRIMITIVES_EXPLORATION_EXPLORATIONTYPE_H_
#define _SHL_PRIMITIVES_EXPLORATION_EXPLORATIONTYPE_H_

#include <string>
#include <vector>
#include "Student/Sensor.h"
#include "QLearner/QLearner.h"

namespace Primitives {

using std::string;
using std::vector;

class QLearner;
class Sensor;

class ExplorationType {
 public:
  /**
   * Destructor for ExplorationType must free all memory it allocated
   * interally, but is not responsible for freeing anything passed into it.
   **/
  virtual ~ExplorationType() {}

  /**
   * Given the current state, return a list of possible next states
   *
   * @param     cur_state       System state to explore around
   * @param     next_state      Populated with suggested next state
   * @reward    reward          Populated with reward for suggested 
   *                            transition
   *
   * @return    True on success, false on failure.
   **/
  virtual bool GetNextState(State *cur_state,
                            State **next_state,
                            double *reward) = 0;
 private:
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_EXPLORATION_EXPLORATIONTYPE_H_
