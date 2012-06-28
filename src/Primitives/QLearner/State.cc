/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of the State Class
 */

#include "QLearner/State.h"
using Utils::Log;

namespace Primitives {


bool State::unserialize(std::vector<std::string> const &contents,
                        std::map<std::string, State*> &hash_map) {
  using std::string;
  using std::stack;
  using std::vector;

  stack<string> blocks;
  bool loaded_vector = false;  // Has the active_state's state_vector loaded?
  State *active_state = NULL;  // Internal state currently being worked on
  string last_action;  // Last Action descriptor string seen when loading within
                       // the "actions" block
  vector<string>::const_iterator iter;
/*
  Log(stdout, DEBUG, "Decoding State:\n");
  for (iter = contents.begin(); iter != contents.end(); ++iter) {
    Log(stdout, DEBUG, iter->c_str());
  }  
  Log(stdout, DEBUG, "----------\n");
*/
  
  for (iter = contents.begin(); iter != contents.end(); ++iter) {
    if (iter->substr(0, 6).compare("BEGIN ") == 0) {
      blocks.push(iter->substr(6));
      if (iter->substr(6).compare("state") == 0) {
        active_state = NULL;
        loaded_vector = false;
      }

      continue;
    } else if (iter->substr(0, 4).compare("END ") == 0) {
      if (blocks.top().compare(iter->substr(4)) == 0) {
        blocks.pop();
        
        // If closing out the 'actions' block, reset the last_action string
        if (iter->substr(4).compare("actions") == 0) {
          last_action = "";
        }
      } else {
        char buf[1024];
        snprintf(buf, sizeof(buf), 
                "State::Unserialize Mismatched END block: %s != %s",
                 blocks.top().c_str(), iter->substr(4).c_str());
        Utils::Log(stdout, ERROR, buf);
        blocks.pop();
      }
      continue;
    }

    if (blocks.empty()) continue;
    
    if (blocks.top().compare("state") == 0) {
      // If we just got inside a state block, load the state vector and set the
      // active_state so we know what we're loading/working with
      if (!loaded_vector) {
        vector<string> state_vector_values;
        vector<double> state_vector;
        Utils::split_string(state_vector_values, *iter, ",");
        vector<string>::iterator state_vector_iter;
        for (state_vector_iter = state_vector_values.begin();
             state_vector_iter != state_vector_values.end();
             ++state_vector_iter) {
          double val = atof((*state_vector_iter).c_str());
          state_vector.push_back(val);
        }          
        State s(state_vector);
        active_state = (hash_map[s.get_state_hash()]);
        if (this != active_state) {
          Log(stdout, ERROR, "Error loading state: Active state mismatch");
          return false;
        }
        loaded_vector = true;
      }
    } else if (blocks.top().compare("rewards") == 0) {
      // Load all the reward layers and transitions into the active_state
      if (iter->substr(0, 6).compare("Target") == 0) {
        // Loading a transition consists of three lines: 
        // Target <hash> (string)
        // Layer <Layer Name> (string)
        // Reward <Reward Value> (double)
        string target;
        string layer;
        string reward;
        double reward_val = 0.;
        
        target = iter->substr(7);
        
        for (int i = 0; i < 2; ++i) {
          ++iter;
          if (iter == contents.end()) {
            Log(stdout, ERROR, "Incomplete Transition-rewards Target group");
            return false;
          } else if (iter->substr(0, 5).compare("Layer") == 0) {
            layer = iter->substr(6);
          } else if (iter->substr(0, 6).compare("Reward") == 0) {
            reward = iter->substr(7);
            reward_val = atof(reward.c_str());
          }
        }
        
        State *target_state = (hash_map[target]);
        if (!target_state) {
          Log(stdout, ERROR, "Error loading state from qtable: "
              "Target missing in hashmap");
          char buf[1024];
          snprintf(buf, sizeof(buf), "Hashmap Size: %ld, missing hash '%s'",
                   hash_map.size(), target.c_str());
          Log(stdout, ERROR, buf);
          
          map<string, State*>::iterator iter;
          for (iter = hash_map.begin(); iter != hash_map.end(); ++iter) {
            snprintf(buf, sizeof(buf), "Hash: '%s'",
                     iter->first.c_str());
            Log(stdout, ERROR, buf);
          }
          
          State *test = (hash_map.find(target)->second);
          if (test) {
            snprintf(buf, sizeof(buf), "Found a state with hash %s.",
                     test->get_state_hash().c_str());
            Log(stdout, ERROR, buf);
          } else {
            Log(stdout, ERROR, 
                "No state found in hash_map matching that hash.");
          }

          return false;
        }
        active_state->set_reward(target_state, layer, reward_val);
      }
    } else if (blocks.top().compare("incoming") == 0) {
      // Don't need to really do anything here... this can be used as more of a 
      // sanity check to ensure everything loaded properly
    } else if (blocks.top().compare("actions") == 0) {
      if (iter->substr(0, 6).compare("Action") == 0) {
        last_action = iter->substr(7);
      } else if (last_action.length() > 0 && active_state) {
        State *target_state = NULL;
        int frequency = 0;
        for (int i = 0; i < 2; ++i) {
          if (iter == contents.end()) {
            Log(stdout, ERROR, "Incomplete Action transition Target group");
            return false;
          } else if (iter->substr(0, 5).compare("Target") == 0) {
            target_state = (hash_map[iter->substr(7)]);
            if (!target_state) {
              Log(stdout, ERROR, "Error loading state actions: Target missing"
                                 " in hashmap");
              return false;
            }
          } else if (iter->substr(0, 6).compare("Frequency") == 0) {
            frequency = atoi(iter->substr(10).c_str());
          }
          if (i == 0)  // Get a pair of Action Target/Frequency lines
            ++iter;
        }
        active_state->ConnectState(target_state, last_action, frequency);
      }
    }
  }

  return true;
}

bool State::Equals(State *state) const {
  if (state->get_state_vector().size() != state_vector_.size())
    return false;

  return (state->get_state_hash().compare(this->get_state_hash()) == 0);

  /*
  vector<double> const &cmp_vector = state->get_state_vector();
  int vector_size = state_vector_.size();
  for (int i = 0; i < vector_size; ++i) {
    if (fabs((state_vector_[i]) - (cmp_vector[i])) > 1E-10) {
      return false;
    }
  }
  */
  return true;
}

std::string State::serialize() {     
  using std::pair;
  using std::vector;
  using std::string;
  const unsigned int BUFFER_SIZE = 4096;
  char buf[BUFFER_SIZE];
  string state_vector;
  string reward_transitions;
  string incoming_states;
  string action_transitions;
  string serialized_state;
  
  memset(buf, 0, BUFFER_SIZE);
  for (unsigned int i = 0; i < state_vector_.size(); ++i) {
    snprintf(buf, BUFFER_SIZE, "%g", state_vector_[i]);
    state_vector.append(buf);
    if (i+1 < state_vector_.size()) state_vector.append(",");
  }
  state_vector.append("\n");
//  state_vector.append("Hash ");
//  state_vector.append(this->get_state_hash());
//  state_vector.append("\n");
  
  map<State*, map<string, double> >::iterator reward_iter;
  for (reward_iter = reward_.begin(); reward_iter != reward_.end();
       ++reward_iter) {
    State *to_state = reward_iter->first;
    snprintf(buf, BUFFER_SIZE, "Target %s\n",
             to_state->get_state_hash().c_str());
    reward_transitions.append(buf);
  
    map<string, double>::iterator layer_iter;
    for (layer_iter = reward_iter->second.begin(); 
         layer_iter != reward_iter->second.end();
         ++layer_iter) {
      string label = layer_iter->first;
      double val = layer_iter->second;
      snprintf(buf, BUFFER_SIZE, "Layer %s\nReward %g\n", label.c_str(), val);
      reward_transitions.append(buf);        
    }
  }

  vector<State*>::iterator  incoming_iter;
  for (incoming_iter = incoming_states_.begin();
       incoming_iter != incoming_states_.end();
       incoming_iter++) {
    snprintf(buf, BUFFER_SIZE, "%s\n",
             (*incoming_iter)->get_state_hash().c_str());
    incoming_states.append(buf);
  }
  
  map<string, vector<pair<State *, int> > >::iterator action_iter;
  
  for (action_iter = out_transitions_.begin();
       action_iter != out_transitions_.end();
       ++action_iter) {
    snprintf(buf, BUFFER_SIZE, "Action %s\n", action_iter->first.c_str());
    action_transitions.append(buf);
    
    vector<pair<State *, int> >::iterator target_iter;
    for (target_iter = action_iter->second.begin();
         target_iter != action_iter->second.end();
         ++target_iter) {
      snprintf(buf, BUFFER_SIZE, "Target %s\nFrequency %d\n",
               target_iter->first->get_state_hash().c_str(),
               target_iter->second);
      action_transitions.append(buf);
    }
  }
  
  serialized_state.append("BEGIN state\n");
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
  serialized_state.append("END state\n");
  
  return serialized_state;
}


std::vector<double> State::GetSquaredDistances(State const * const state) 
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


double State::GetEuclideanDistance(State const * const state) const {
  std::vector<double> squared_dists = this->GetSquaredDistances(state);
  
  double real_dist = 0.;
  
  std::vector<double>::iterator iter;
  for (iter = squared_dists.begin(); iter != squared_dists.end(); ++iter) {
    real_dist += *iter;
  }
  
  real_dist = sqrt(real_dist);
  return real_dist;
}


double State::GetRewardValue(State *target, bool all_layers,
                              std::string layer) {
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

std::string State::GetActionForTransition(State *target_state) {
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
      double action_prb = 0.;  // Probability of going to target_state with
                               // this action
                              
      // Iterate over all states previously reached with this action
      for (iter = destinations.begin(); iter != destinations.end(); ++iter) {
        total_entries += (*iter).second;
        if ((*iter).first == target_state) {
          double prb = static_cast<double>((*iter).second);
          action_prb = prb;
        }
      }
      
      action_prb /= total_entries;  // Normalize the probability
      
      // Keep track of the best possible action to reach the desired state
      if (action_prb > best_probability) {
        best_probability = action_prb;
        best_action = action_iter->first;
      }
  }
  
  return best_action;
}


bool State::ConnectState(State *target, std::string action) {
  return this->ConnectState(target, action, 1);
}

bool State::ConnectState(State *target, std::string action, 
                         int default_frequency) {
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
    pair<State *, int> transition(target, default_frequency);
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
      pair<State *, int> transition(target, default_frequency);
      transition_probabilities.push_back(transition);
    }
  }
  return true;
}

void State::set_reward(State *target, std::string layer, double val) {
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
      if ((*inc_iter)->Equals(this)) {
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
}  // namespace primitives
