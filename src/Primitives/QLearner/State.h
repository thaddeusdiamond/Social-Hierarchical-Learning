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
   * @param unit_dist           Sum of 'minimum recognized change' over each
   *                            sensor that composes the state vector
   * @param reward              Real-valued reward value associated with state
   **/
  State(const std::vector<double> &state_descriptor,
        const double unit_dist = 0.,
        const double reward = 0.)
    : state_vector_(state_descriptor), unit_distance_(unit_dist),
      reward_(reward) {}

  /**
   * Copy constructor
   **/
  explicit State(State const &s) : state_vector_(s.get_state_vector()),
    unit_distance_(s.get_unit_distance()), reward_(s.get_base_reward()) {
    std::map<std::string, double> reward_layers = s.get_reward_layers();
    std::map<std::string, double>::const_iterator iter;
    for (iter = s.reward_layers_.begin(); iter != reward_layers_.end();
         ++iter) {
      reward_layers_[(*iter).first] = (*iter).second;
    }
  }

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

  /**
   * Finds the euclidean squared distance between two states
   * @param state State to measure distance to
   * @return Euclidean distance between states
   */
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

  /**
   * Sets a reward with key 'layer' to value 'val' on this state
   *
   * @param layer Keyword associated with value
   * @param val Reward value to assign
   **/
  virtual void set_reward(std::string layer, double val) {
    if (val != 0) {
      reward_layers_[layer] = val;
    } else {
      std::map<std::string, double>::iterator iter;
      iter = reward_layers_.find(layer);
      reward_layers_.erase(iter);
    }
  }

  /**
   * Sets all keys in reward_layers_ to values contained in layers
   *
   * @param layers Map with key=>reward to overwrite local values
   **/
  virtual void set_reward_layers(std::map<std::string, double> layers) {
    std::map<std::string, double>::iterator iter;
    for (iter = layers.begin(); iter != layers.end(); ++iter) {
      reward_layers_[iter->first] = iter->second;
    }
  }

  /**
   * Retrieves the total reward on this state object
   * @return Sum of base reward and all reward 'layers'
   */
  virtual double get_reward() const {
    double total_reward = reward_;
    std::map<std::string, double>::const_iterator iter;
    for (iter = reward_layers_.begin(); iter != reward_layers_.end(); ++iter) {
      total_reward += (*iter).second;
    }

    return total_reward;
  }

  virtual void set_base_reward(double const reward) { reward_ = reward; }

  /**
   * Retrieves non-layered, 'unmodified' reward value for this state
   * @return Base reward score
   */
  virtual double get_base_reward() const { return reward_; }

  virtual std::map<std::string, double> get_reward_layers() const {
    return reward_layers_;
  }

  virtual double get_unit_distance() const { return unit_distance_; }


  static const double NEARBY_STATE_THRESHOLD = 10.;

 private:
  std::vector<double> state_vector_;
  double unit_distance_;
  double reward_;
  std::map<std::string, double> reward_layers_;
  char state_hash_[160];  // @TODO: When state_vector_ is populated,
                          // store hash here to allow quick comparisons
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_STATE_H_
