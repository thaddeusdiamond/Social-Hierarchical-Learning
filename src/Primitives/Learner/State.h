/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Exploration Function Implementation
 */

#ifndef _SHL_PRIMITIVES_LEARNER_STATE_H_
#define _SHL_PRIMITIVES_LEARNER_STATE_H_

#include <vector>

class State {
 public:
  /**
   * Constructs a State variable given a vector of doubles and a reward
   * 
   * @param state_descriptor    Description of state being represented
   * @param reward              Real-valued reward value associated with state
   */
  State(const std::vector<double> &state_descriptor, const double reward = 0.)
    : state_vector_(state_descriptor), reward_(reward) {}

  /**
   * Shouldn't have to free anything here
   */
  virtual ~State() {}

  /**
   * Returns an immutable state descriptor vector of doubles.
   *
   * @return    Const reference to state descriptor
   */
  virtual const std::vector<double>& GetStateVector() {
    return state_vector_;
  };

  virtual void SetReward(const double reward) { reward_ = reward; }

  virtual double GetReward() { return reward_; }

 private:
  std::vector<double> state_vector_;
  double reward_;
};


#endif  // _SHL_PRIMITIVES_LEARNER_STATE_H_
