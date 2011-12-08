/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Exploration Function Implementation
 **/

#ifndef _SHL_PRIMITIVES_QLEARNER_STATE_H_
#define _SHL_PRIMITIVES_QLEARNER_STATE_H_

#include <vector>

namespace Primitives {

class State {
 public:
  /**
   * Constructs a State variable given a vector of doubles and a reward
   * 
   * @param state_descriptor    Description of state being represented
   * @param reward              Real-valued reward value associated with state
   **/
  State(const std::vector<double> &state_descriptor, const double reward = 0.)
    : state_vector_(state_descriptor), reward_(reward) {}

  /**
   * Copy constructor
   **/
  explicit State(State const &s) : state_vector_(s.get_state_vector()),
    reward_(s.get_reward()) {}

  /**
   * Copy constructor
   **/
  explicit State() {}

  /**
   * Shouldn't have to free anything here
   **/
  virtual ~State() {}

  /**
   * Determines if this state is equal to the provided state parameter by
   * comparing the state_vector_ variables in each.
   * @param state State to compare to
   * @return true if state vectors are equal, false if not
   **/
  virtual bool Equals(State const &state) const  {
    if (state.get_state_vector().size() != state_vector_.size())
      return false;

    // Only check this if stateHash
    std::vector<double>::const_iterator iter;
    std::vector<double>::const_iterator needle_iter;
    for (iter = state_vector_.begin(),
         needle_iter = state.get_state_vector().begin();
         iter != state_vector_.end(),
         needle_iter != state.get_state_vector().end();
         ++iter, ++needle_iter) {
      if ((*iter) != (*needle_iter)) return false;
    }
    return true;
  }

  virtual double GetSquaredDistance(State const * const state) const {
    if (!state) return -1.;

    if (state->get_state_vector().size() != state_vector_.size())
      return -1.;

    double distance = 0.;

    std::vector<double>::const_iterator iter;
    std::vector<double>::const_iterator needle_iter;
    for (iter = state_vector_.begin(),
         needle_iter = state->get_state_vector().begin();
         iter != state_vector_.end(),
         needle_iter != state->get_state_vector().end();
         ++iter, ++needle_iter) {
      distance += ((*iter) - (*needle_iter)) * ((*iter) - (*needle_iter));
    }

    return distance;
  }

  /**
   * Returns an immutable state descriptor vector of doubles.
   *
   * @return    Const reference to state descriptor
   **/
  virtual std::vector<double> get_state_vector() const {
    return state_vector_;
  };

  virtual void set_reward(double const reward) { reward_ = reward; }

  virtual double get_reward() const { return reward_; }

 private:
  std::vector<double> state_vector_;
  double reward_;
  char state_hash_[160];  // @TODO: When state_vector_ is populated,
                          // store hash here to allow quick comparisons
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_STATE_H_
