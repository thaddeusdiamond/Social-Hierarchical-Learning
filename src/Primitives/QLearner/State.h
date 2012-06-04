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
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <hashlibpp.h>
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
  virtual bool Equals(State *state) const {
    if (state->get_state_vector().size() != state_vector_.size())
      return false;


    vector<double> const &cmp_vector = state->get_state_vector();
    int vector_size = state_vector_.size();
    for (int i = 0; i < vector_size; ++i) {
      if (fabs((state_vector_[i]) - (cmp_vector[i])) > 0.00001) {
        return false;
      }
    }

    return true;
  }

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
  virtual std::string serialize() {     
    using std::pair;
    using std::vector;
    using std::string;
    const unsigned int BUFFER_SIZE = 4096;
    char state_vector[BUFFER_SIZE];
    char reward_transitions[BUFFER_SIZE];
    char incoming_states[BUFFER_SIZE];
    char action_transitions[BUFFER_SIZE];
    
    string serialized_state;
     
    for (unsigned int i = 0; i < state_vector_.size(); ++i)
      snprintf(state_vector, BUFFER_SIZE, "%s,%g",state_vector, state_vector_[i]);
    snprintf(state_vector, BUFFER_SIZE, "%s\n",state_vector);
    
    map<State*, map<string, double> >::iterator reward_iter;
    for (reward_iter = reward_.begin(); reward_iter != reward_.end();
         ++reward_iter) {
      State *to_state = reward_iter->first;
      snprintf(reward_transitions, BUFFER_SIZE, "%sTarget %s\n", 
               reward_transitions, to_state->get_state_hash().c_str());
    
      map<string, double>::iterator layer_iter;
      for (layer_iter = reward_iter->second.begin(); 
           layer_iter != reward_iter->second.end();
           ++layer_iter) {
         string label = layer_iter->first;
         double val = layer_iter->second;
         snprintf(reward_transitions, BUFFER_SIZE, "%s%s\n%g\n",
                  reward_transitions, label.c_str(), val);
      }
    }

    vector<State*>::iterator  incoming_iter;
    for (incoming_iter = incoming_states_.begin();
         incoming_iter != incoming_states_.end();
         incoming_iter++) {
      snprintf(incoming_states, BUFFER_SIZE, "%s%s\n", incoming_states, 
               (*incoming_iter)->get_state_hash().c_str());
    }
    
    map<string, vector<pair<State *, int> > >::iterator action_iter;
    
    for (action_iter = out_transitions_.begin();
         action_iter != out_transitions_.end();
         ++action_iter) {
      snprintf(action_transitions, BUFFER_SIZE, "%sAction %s\n",
               action_transitions, action_iter->first.c_str());
      vector<pair<State *, int> >::iterator target_iter;
      for (target_iter = action_iter->second.begin();
           target_iter != action_iter->second.end();
           ++target_iter) {
        snprintf(action_transitions, BUFFER_SIZE, "%s%s\n%d\n", action_transitions,
                 target_iter->first->get_state_hash().c_str(),
                 target_iter->second);
      }
    }
    
    serialized_state.append(state_vector);
    serialized_state.append("BEGIN rewards\n");
    serialized_state.append(reward_transitions);
    serialized_state.append("END rewards\n");
    serialized_state.append("BEGIN incoming\n");
    serialized_state.append(incoming_states);
    serialized_state.append("END incoming\n");
    serialized_state.append("BEGIN actions\n");
    serialized_state.append(action_transitions);
    serialized_state.append("END actions\n");
    
    return serialized_state;
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

    vector<double> const &cmp_vector = state->get_state_vector();

    double distance = 0.;
    unsigned int idx;
    for (idx = 0; idx < state_vector_.size(); ++idx) {
      distance = (state_vector_[idx] - cmp_vector[idx])
      * (state_vector_[idx] - cmp_vector[idx]);
      distances.push_back(distance);
    }

    return distances;
  }

  /**
   * Retrieves the reward (transition function) on this state object
   * for a particular State
   * @return Summed reward value
   */
  virtual double GetRewardValue(State *target, bool all_layers = true,
                                std::string layer = "") {
    if (reward_.find(target) == reward_.end()) return 0.;
    std::map<std::string, double> &reward_layers = reward_[target];
    std::map<std::string, double>::const_iterator iter;

    double total = 0.;
    if (all_layers) {
      for (iter = reward_layers.begin(); iter != reward_layers.end();
          ++iter) {
        total += (*iter).second;
      }
    } else {
      iter = reward_layers.find(layer);
      if (iter != reward_layers.end())
        total = (*iter).second;
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
   * Returns a serialized action string that has the highest probability of
   * reaching the target state. If no known action is found, an empty string
   * is returned
   * 
   * @param target_state QTable-Internal pointer to target State
   * @return Serialized action, or empty string if no suitable actions found.
   * 
   **/
  virtual std::string getActionForTransition(State *target_state) {
    using std::pair;
    using std::string;
    using std::map;
    using std::vector;
    
    map<string, vector<pair<State *, int> > >::iterator action_iter;
    double best_probability = 0.;
    string best_action = "";
    
    // Iterate over all known action transitions from this state
    for (action_iter = this->out_transitions_.begin();
         action_iter != this->out_transitions_.end();
         ++action_iter) {
        vector<pair<State *, int> > &destinations = action_iter->second;
        vector<pair<State *, int> >::iterator iter;      
        
        // Count how many transition examples we have from a given action
        double total_entries = 0.;
        double action_prb = 0.; // Probability of going to target_state with
                                // this action
                                
        // Iterate over all states previously reached with this action
        for (iter = destinations.begin(); iter != destinations.end(); ++iter) {
          total_entries += (*iter).second;
          if ((*iter).first == target_state) {
            double prb = double((*iter).second);
            action_prb = prb;
          }
        }
        
        action_prb /= total_entries; // Normalize the probability
        
        // Keep track of the best possible action to reach the desired state
        if (action_prb > best_probability) {
          best_probability = action_prb;
          best_action = action_iter->first;
        }
    }
    
    return best_action;
  }

  /**
   * Associates an transition count with a target state/action pair, which
   * can be used to determine transition probabilities for an action.
   * 
   * @param target State pointer to state **internal** to the skill's QTable
   * @param action Serialized action to be associated with the transition
   **/
  virtual bool connectState(State *target, std::string action) {
    using std::pair;
    using std::string;
    using std::map;
    using std::vector;
    
    map<string, vector<pair<State *, int> > >::iterator
        iter;

    // See if this action has already been transitioned from in this state
    iter = this->out_transitions_.find(action);

    // If action hasn't been done yet, add it to the action/transition map
    if (iter == this->out_transitions_.end()) {
      vector<pair<State *, int> > transition_probabilities;
      pair<State *, int> transition (target, 1);
      transition_probabilities.push_back(transition);
      this->out_transitions_[action] = transition_probabilities;
    } else {
      // Have done this action before, so just add the new transition info
      vector<pair<State *, int> > &transition_probabilities = iter->second;
      vector<pair<State *, int> >::iterator state_iter;
      
      bool found_state = false;
      for (state_iter = transition_probabilities.begin();
           state_iter != transition_probabilities.end();
           ++state_iter) {
        if (state_iter->first == target) {
          ++(state_iter->second);
          found_state = true;
          break;
        }
      }
      
      // If we've never transitioned to this state with this action, add it
      if (!found_state) {
        pair<State *, int> transition (target, 1);
        transition_probabilities.push_back(transition);
      }
    }
    return true;
  };

  /**
   * Sets a reward with key 'layer' to value 'val' on this state
   *
   * @param layer Keyword associated with value
   * @param val Reward value to assign
   **/
  virtual void set_reward(State *target, std::string layer, double val) {
    if (target == NULL) return;
    std::vector<State *> &inc_states = target->get_incoming_states();

    // If setting the reward layer to something
    if (val != 0) {
      std::map<State*, std::map<std::string, double> >::iterator
        layer_map = (reward_.find(target));

      // If there is no existing link to target, make one
      if (layer_map == reward_.end()) {
        reward_[target] = std::map<std::string, double>();
      }
      (reward_[target])[layer] = val;


      vector<State *>::iterator inc_iter;
      bool found = false;
      for (inc_iter = inc_states.begin(); inc_iter != inc_states.end();
           ++inc_iter) {
        if (*inc_iter == this) {
          found = true;
          break;
        }
      }

      if (!found)
        inc_states.push_back(this);
    } else {
      if (reward_.find(target) == reward_.end()) return;

      // Clearing the reward layer away
      std::map<std::string, double>::iterator iter;
      iter = (reward_[target]).find(layer);
      if (iter != reward_[target].end())
        (reward_[target]).erase(iter);

      if (reward_[target].size() == 0) {
        vector<State *>::iterator inc_iter;
        bool found = false;
        for (inc_iter = inc_states.begin(); inc_iter != inc_states.end();
             ++inc_iter) {
          if (*inc_iter == this) {
            found = true;
            break;
          }
        }
        if (found)
          inc_states.erase(inc_iter);
      }
    }
  }


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

  std::string get_state_hash() { return state_hash_; }

 private:
  explicit State() {}
  
  /**
   * Populates the state_hash_ with an MD5 hash of the state vector values
   */
  void generateHash() {
    hashwrapper *hash_gen = new md5wrapper();
    char buf[4096];

    if (state_vector_.size() == 0) return;

    snprintf(buf, 4096, "%g", state_vector_[0]);
    for (unsigned int i = 1; i < state_vector_.size(); ++i)
     snprintf(buf, 4096, "%s,%g",buf, state_vector_[i]);
    
    state_hash_ = hash_gen->getHashFromString(std::string(buf));
  }
  

  std::vector<double> state_vector_;
  std::map<State*, std::map<std::string, double> > reward_;
  std::vector<State*> incoming_states_;
  
  // Maps action IDs (strings) to a list of states/probabilities
  std::map<std::string, std::vector<std::pair<State *, int> > > 
      out_transitions_;
  unsigned int out_transitions_sample_count_;
  
  std::string state_hash_; // MD5 Hash of State Vector

};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_STATE_H_
