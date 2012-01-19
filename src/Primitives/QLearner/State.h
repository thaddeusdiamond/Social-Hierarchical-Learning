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
#include <map>
#include <string>
namespace Primitives {

class State {
 public:
  /**
   * Constructs a State variable given a vector of doubles and a reward
   *
   * @param state_descriptor    Description of state being represented
   **/
  explicit State(const std::vector<double> &state_descriptor)
    : state_vector_(state_descriptor) {}

  /**
   * Copy constructor. Disregards all state transitions from s.
   **/
  explicit State(State const &s) : state_vector_(s.get_state_vector()) {}

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

  /**
   * Finds the euclidean squared distance between two states
   * @param state State to measure distance to
   * @return Euclidean distance between states
   */
  virtual std::vector<double> GetSquaredDistances(State const * const state)
    const {
    std::vector<double> distances;
    if (!state) return distances;

    if (state->get_state_vector().size() != state_vector_.size())
      return distances;

    double distance = 0.;

    std::vector<double>::const_iterator iter;
    std::vector<double>::const_iterator needle_iter;
    for (iter = state_vector_.begin(),
         needle_iter = state->get_state_vector().begin();
         iter != state_vector_.end(),
         needle_iter != state->get_state_vector().end();
         ++iter, ++needle_iter) {
      distance = ((*iter) - (*needle_iter)) * ((*iter) - (*needle_iter));
      distances.push_back(distance);
    }

    return distances;
  }

  /**
   * Retrieves the reward (transition function) on this state object
   * for a particular State
   * @return Summed reward value
   */
  virtual double GetRewardValue(State *target) {
    if (reward_.find(target) == reward_.end()) return 0.;
    std::map<std::string, double> &reward_layers = reward_[target];
    std::map<std::string, double>::const_iterator iter;
    double total = 0.;
    for (iter = reward_layers.begin(); iter != reward_layers.end();
         ++iter) {
      total += (*iter).second;
    }

    return total;
  }


  /**
   * Returns an immutable state descriptor vector of doubles.
   *
   * @return    Const reference to state descriptor
   **/
  virtual std::vector<double> get_state_vector() const {
    return state_vector_;
  };

  /**
   * Sets a reward with key 'layer' to value 'val' on this state
   *
   * @param layer Keyword associated with value
   * @param val Reward value to assign
   **/
  virtual void set_reward(State *target, std::string layer, double val) {
    if (val != 0) {
      std::map<State*, std::map<std::string, double> >::iterator
        layer_map = (reward_.find(target));
      if (layer_map == reward_.end()) {
        reward_[target] = std::map<std::string, double>();
      }
      (reward_[target])[layer] = val;
    } else {
      std::map<std::string, double>::iterator iter;
      iter = (reward_[target]).find(layer);
      (reward_[target]).erase(iter);
    }
  }


  /**
   * Retrieves the reward (transition function) on this state object
   * @return Function mapping State* to reward layers
   */
  virtual std::map<State*, std::map<std::string, double> > get_reward() const {
    return reward_;
  }

 private:
  /**
   * Copy constructor
   **/
  explicit State() {}

  std::vector<double> state_vector_;
  std::map<State*, std::map<std::string, double> > reward_;
  char state_hash_[160];  // @TODO: When state_vector_ is populated,
                          // store hash here to allow quick comparisons
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_STATE_H_
