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

#include <stdio.h>
#include <hashlibpp.h>
#include <cstdlib>
#include <utility>
#include <vector>
#include <stack>
#include <map>
#include <cmath>
#include <string>
#include "Common/Utils.h"
#include "Primitives/QLearner/Action.h"

namespace Primitives {
using std::vector;
using std::string;
using std::map;
using Utils::Log;

class State {
 public:
  /**
   * Constructs a State variable given a vector of doubles and a reward
   *
   * @param state_descriptor    Description of state being represented
   **/
  explicit State(const std::vector<double> &state_descriptor)
    : state_vector_(state_descriptor), out_transitions_sample_count_(0) { 
    generateHash();
  }

  /**
   * Copy constructor. Disregards all state transitions from s.
   **/
  explicit State(State const &s) : state_vector_(s.get_state_vector()),
      out_transitions_sample_count_(0) {
    generateHash();        
  }

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
  virtual bool Equals(State *state) const;

  /**
   * Serializes the state so that it can be saved to disk.
   *
   * 
   * State Serialization Format:
   * 
   * Line 1: CSV State Vector (doubles)
   * 
   * BEGIN {BLOCK}
   * 
   * END {BLOCK}
   * 
   * Block "rewards"
   * Line 1: "Target " + MD5 Target Hash
   * Line 2: Layer Name
   * Line 3: Reward Value
   * Line 4: Layer Name
   * Line 5: Reward Value
   * 
   * Block "incoming"
   * Line 1: Incoming state hash
   * 
  std::vector<double> state_vector_;
  std::map<State*, std::map<std::string, double> > reward_;
  std::vector<State*> incoming_states_;
  
  // Maps action IDs (strings) to a list of states/probabilities
  std::map<std::string, std::vector<std::pair<State *, int> > > 
      out_transitions_;
  unsigned int out_transitions_sample_count_;

   */
  virtual std::string serialize();
  
  virtual bool unserialize(std::vector<std::string> const &contents,
                           std::map<std::string, State*> &hash_map);

  /**
   * Finds the euclidean squared distance between two states
   * @param state State to measure distance to
   * @return Euclidean distance between states
   */
  virtual std::vector<double> GetSquaredDistances(State const * const state)
    const;

  /**
   * Finds the euclidean distance between two states
   * @param state State to measure distance to
   * @return Euclidean distance between states
   */
  virtual double GetEuclideanDistance(State const * const state)
    const;

  /**
   * Retrieves the reward (transition function) on this state object
   * for a particular State
   * @return Summed reward value
   */
  virtual double GetRewardValue(State *target, bool all_layers = true,
                                std::string layer = "");


  /**
   * Returns an immutable state descriptor vector of doubles.
   *
   * @return    Const reference to state descriptor
   **/
  virtual std::vector<double> get_state_vector() const {
    return state_vector_;
  };
  
  
  
  /**
   * Returns a serialized action string that has the highest probability of
   * reaching the target state. If no known action is found, an empty string
   * is returned
   * 
   * @param target_state QTable-Internal pointer to target State
   * @return Serialized action, or empty string if no suitable actions found.
   * 
   **/
  virtual std::string GetActionForTransition(State *target_state);

  /**
   * Associates an transition count with a target state/action pair, which
   * can be used to determine transition probabilities for an action.
   * 
   * @param target State pointer to state **internal** to the skill's QTable
   * @param action Serialized action to be associated with the transition
   **/
  virtual bool ConnectState(State *target, std::string action);
  virtual bool ConnectState(State *target, std::string action,
                            int default_frequency);
  /**
   * Sets a reward with key 'layer' to value 'val' on this state
   *
   * @param layer Keyword associated with value
   * @param val Reward value to assign
   **/
  virtual void set_reward(State *target, std::string layer, double val);


  /**
   * Retrieves the reward (transition function) on this state object
   * @return Function mapping State* to reward layers
   */
  virtual std::map<State*, std::map<std::string, double> > get_reward() const {
    return reward_;
  }

  virtual std::vector<State *> &get_incoming_states() {
    return incoming_states_;
  }

  virtual std::string to_string() {
    char buf[4096];
    unsigned int state_count = state_vector_.size();
    for (unsigned int i = 0; i < state_count; ++i) {
      if (i)
        snprintf(buf, sizeof(buf), "%s, %g", buf, state_vector_[i]);
      else
        snprintf(buf, sizeof(buf), "%g", state_vector_[i]);
    }
    return std::string(buf);
  }

  std::string get_state_hash() const { return state_hash_; }

 private:
  explicit State() {}
  
  /**
   * Populates the state_hash_ with an MD5 hash of the state vector values
   */
  void generateHash() {
    hashwrapper *hash_gen = new md5wrapper();
    char buf[4096];
    std::string buf_str;
    if (state_vector_.size() == 0) return;
    
    snprintf(buf, sizeof(buf), "%g", state_vector_[0]);
    buf_str = std::string(buf);
    for (unsigned int i = 1; i < state_vector_.size(); ++i) {
     memset(buf, 0, sizeof(buf));
     snprintf(buf, sizeof(buf), "%g", state_vector_[i]);
     buf_str.append(buf);
    }

    state_hash_ = hash_gen->getHashFromString(std::string(buf));
  }
  

  std::vector<double> state_vector_;
  std::map<State*, std::map<std::string, double> > reward_;
  std::vector<State*> incoming_states_;
  
  // Maps action IDs (strings) to a list of states/probabilities
  std::map<std::string, std::vector<std::pair<State *, int> > > 
      out_transitions_;
  unsigned int out_transitions_sample_count_;
  
  std::string state_hash_;  // MD5 Hash of State Vector
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_STATE_H_
