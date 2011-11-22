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

  explicit State(State &s) : state_vector_(s.get_state_vector()),
    reward_(s.get_reward()) {}

  /**
   * Shouldn't have to free anything here
   */
  virtual ~State() {}

  /**
   * Returns an immutable state descriptor vector of doubles.
   *
   * @return    Const reference to state descriptor
   */
  virtual std::vector<double> const &get_state_vector() {
    return state_vector_;
  };

  virtual void set_reward(const double reward) { reward_ = reward; }

  virtual double get_reward() { return reward_; }

 private:
  std::vector<double> state_vector_;
  double reward_;
};


#endif  // _SHL_PRIMITIVES_LEARNER_STATE_H_
