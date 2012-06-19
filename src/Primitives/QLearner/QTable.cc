/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of the QTable Storage Class
 */

#include <map>
#include "QLearner/QTable.h"
#include "QLearner/State.h"

namespace Primitives {


std::vector<State*> QTable::GetNearbyStates(State const &needle) {
  std::vector<State*> nearby_states;

  std::vector<State*>::iterator iter;

  for (iter = states_.begin(); iter != states_.end(); ++iter) {
    std::vector<double> dists = needle.GetSquaredDistances((*iter));

    bool fail = false;
    unsigned int idx;
    for (idx = 0; idx < nearby_thresholds_.size(); ++idx) {
      double squared_thresh = nearby_thresholds_[idx];
      double squared_dist = dists[idx];
      if (squared_dist > squared_thresh) {
        fail = true;
        break;
      }
    }

    if (!fail) nearby_states.push_back(*iter);
  }

  return nearby_states;
}

State *QTable::GetState(std::string needle_hash) {
  std::vector<State *>::iterator iter;
  for (iter = states_.begin(); iter != states_.end(); iter++) {
    if (*iter == NULL) continue;  // Shouldn't have deleted states in the table
    std::string state_hash = (*iter)->get_state_hash();
    if (needle_hash.compare(state_hash) == 0)
      return (*iter);
  }  
  return NULL;
}

State *QTable::GetState(State const &needle, bool add_estimated_state) {
  // Search through the huge states_ vector for the target state

  std::vector<double> nearby_state_dists = this->get_nearby_thresholds();
  std::vector<State *>::iterator iter;
  for (iter = states_.begin(); iter != states_.end(); iter++) {
    if (*iter == NULL) continue;  // Shouldn't have deleted states in the table

    if (needle.Equals(*iter))
      return (*iter);
  }


  // Log(stderr,DEBUG,"State not found in GetState.");

  if (add_estimated_state) {
    State s(needle.get_state_vector());
    State *new_state = this->AddState(s);

    if (new_state->get_state_vector().size() !=
        needle.get_state_vector().size())
      Log(stderr, ERROR,
          "AddState portion of GetState didn't copy the vector.");

    std::vector<State *> nearby_states = this->GetNearbyStates(needle);

    if (nearby_states.size() == 0)
      Log(stderr, ERROR, "No nearby states on GetState!");

    std::map<std::string, double> reward_layers;

    std::vector<State *>::iterator nearby_iter;

    // For each nearby state, take a fraction of its reward related to distance
    // and apply it to the new state to be created
    for (nearby_iter = nearby_states.begin();
         nearby_iter != nearby_states.end();
          ++nearby_iter) {
      State *near_state = *nearby_iter;

      // Calculate the weight of the transition rewards from the new state
      // based on distance to this nearby, pre-existing state
      std::vector<double> squared_dists =
        needle.GetSquaredDistances(*nearby_iter);

      double weight = 0.;
      unsigned int idx;
      for (idx = 0;
          idx < nearby_state_dists.size() && idx < squared_dists.size();
          ++idx) {
        weight += 1. - (squared_dists[idx] / nearby_state_dists[idx]);
      }
      weight /= squared_dists.size();
      
      // Add the same incoming reward transitions as the found state, reward
      // value weighted by the distance of the found state from the needle state
      // --Only transfer the 'base' layer--
      vector<State *> & inc_states = near_state->get_incoming_states();
      vector<State *>::iterator inc_iter;
      for (inc_iter = inc_states.begin(); inc_iter != inc_states.end();
           ++inc_iter) {
        State *inc_state = (*inc_iter);
        double orig_reward = inc_state->GetRewardValue(
                                near_state, false, "base");
        inc_state->set_reward(new_state, string("base"), orig_reward * weight);
      }

      // Add the same outgoing reward transitions as the found state, reward
      // weighted by the distance of the found state from the needle state
      // --Only transfer the 'base' layer--
      std::map<State*, std::map<std::string, double> > const &rewards
        = near_state->get_reward();

      std::map<State*, std::map<std::string, double> >::const_iterator
        state_reward_iter;

      std::string base_layer = string("base");

      // iterate through each state near_state links to, and copy the base
      // reward layer to the new state
      for (state_reward_iter = rewards.begin();
           state_reward_iter != rewards.end();
           ++state_reward_iter) {
          State *target_state = (*state_reward_iter).first;
          std::map<std::string, double> const &reward_layers =
            (*state_reward_iter).second;

          double base_reward = 1.;
          std::map<std::string, double>::const_iterator reward_layer =
            reward_layers.find(base_layer);
          if (reward_layer != reward_layers.end()) {
            base_reward = (*reward_layer).second * weight;
          }

          if (new_state->GetRewardValue(target_state, false, "base") <
              base_reward)
            new_state->set_reward(target_state, base_layer, base_reward);
      }
    }

    return new_state;
  } else {
    return NULL;
  }
}



/**
 * @todo
 * Use a bloom filter to find whether or not the QTable contains the needle.
 * Returning true here doesn't actually mean we have the state, it just means
 * that we probably have it, and it's worth the cycles to search for it in the
 * big mess that is the states_ vector.
 *
 * Returning a value of false means we absolutely 100% have not seen that state.
 **/
bool QTable::HasState(State const &needle) {
  return true;
}

State *QTable::AddState(State const &state) {
  State *s = new State(state);
  states_.push_back(s);
  return s;
}



std::string QTable::serialize() {
  using std::string;
  using std::vector;
  
  string serialized_table;
  string serialized_states;
  string initiate_state_hashes;
  string goal_state_hashes;
  string trained_goal_state_hashes;
  string nearby_thresholds;
  
  vector<State *>::iterator state_iter;
  for (state_iter = states_.begin();
       state_iter != states_.end();
       ++state_iter) {
    serialized_states.append((*state_iter)->serialize());
  }

  vector<State *>::iterator initiate_iter;
  for (initiate_iter = initiate_states_.begin();
       initiate_iter != initiate_states_.end();
       ++initiate_iter) {
    initiate_state_hashes.append((*initiate_iter)->get_state_hash());
    initiate_state_hashes.append("\n");
  }

  vector<State *>::iterator goal_iter;
  for (goal_iter = goal_states_.begin();
       goal_iter != goal_states_.end();
       ++goal_iter) {
    goal_state_hashes.append((*goal_iter)->get_state_hash());
    goal_state_hashes.append("\n");
  }

  vector<State *>::iterator trained_goal_iter;
  for (trained_goal_iter = trained_goal_states_.begin();
       trained_goal_iter != trained_goal_states_.end();
       ++trained_goal_iter) {
    trained_goal_state_hashes.append((*trained_goal_iter)->get_state_hash());
    trained_goal_state_hashes.append("\n");
  }
  
 vector<double>::iterator threshold_iter;
  for (threshold_iter = nearby_thresholds_.begin();
       threshold_iter != nearby_thresholds_.end();
       ++threshold_iter) {
         
    char buf[128];
    snprintf(buf, 128, "%g", (*threshold_iter));
    
    nearby_thresholds.append(buf);
    if ((threshold_iter+1) != nearby_thresholds_.end())
      nearby_thresholds.append(",");
  }
  nearby_thresholds.append("\n");
       
  serialized_table.append("BEGIN qtable\n");
  serialized_table.append("BEGIN internal_states\n");
  serialized_table.append(serialized_states);
  serialized_table.append("END internal_states\n");
  serialized_table.append("BEGIN initiate_states\n");
  serialized_table.append(initiate_state_hashes);
  serialized_table.append("END initiate_states\n");
  serialized_table.append("BEGIN goal_states\n");
  serialized_table.append(goal_state_hashes);
  serialized_table.append("END goal_states\n");
  serialized_table.append("BEGIN trained_goal_states\n");
  serialized_table.append(trained_goal_state_hashes);
  serialized_table.append("END trained_goal_states\n");
  serialized_table.append("BEGIN nearby_thresholds\n");
  serialized_table.append(nearby_thresholds);
  serialized_table.append("END nearby_thresholds\n");
  serialized_table.append("END qtable\n");
  
  return serialized_table;
}


bool QTable::unserialize(std::vector<std::string> const &contents) {
  using std::string;
  using std::vector;
  using std::stack;
  
  stack<string> blocks;
  vector<string>::const_iterator iter;
  map<string, State*> hash_map;
  
  // First Pass: Load all the individual states that were recorded without any
  //             of their connections (for hash-map lookup later)
  bool loaded_vector = false;
  for (iter = contents.begin(); iter != contents.end(); ++iter) {
    if (iter->substr(0,6).compare("BEGIN ") == 0) {
      blocks.push(iter->substr(6));
      if (iter->substr(6).compare("state") == 0)
        loaded_vector = false;
      continue;
    } else if (iter->substr(0,4).compare("END ") == 0) {
      if (blocks.top().compare(iter->substr(4)) == 0) {
        blocks.pop();
      } else {
        char buf[1024];
        snprintf(buf, 1024, "QTable::Unserialize 1: Mismatched END block: %s",
                 blocks.top().c_str());
        Utils::Log(stdout, ERROR, buf);
        blocks.pop();
      }
      continue;
    }

    // First line inside BEGIN state: load as state vector
    if (!loaded_vector && blocks.top().compare("state") == 0) {
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
      State *internal_state = this->AddState(s);

/*
      char buf[4096];
      snprintf(buf, 4096, "Loaded state with hash: %s",
               internal_state->get_state_hash().c_str());
      Log(stdout, DEBUG, buf);
*/
      
      hash_map[s.get_state_hash()] = internal_state;     
      loaded_vector = true;
    }
    //Don't do anything for all the other data contained in the state (yet)
  }
  
  /*
  char buf[1024];
  snprintf(buf, 1024, "Loaded %ld states from QTable First Pass",
           hash_map.size());
  Log(stdout, DEBUG, buf);  
  */

  // @todo Need to load initiate_states, goal_states, trained_goal_states
  
  // Second Pass: Load all of the details for each state, now that there is a
  //              mapping from hash => state in the Q-Table
  blocks.empty();
  loaded_vector = false; // Has the active_state's state vector been loaded?
  State *active_state = NULL; // Internal state currently being worked on
  string last_action; // Last Action descriptor string seen when loading within
                      // the "actions" block
  vector<string> state_contents;
  bool recording = false; // When true, put lines in the state_contents vector
  
  for (iter = contents.begin(); iter != contents.end(); ++iter) {
    if (iter->substr(0,6).compare("BEGIN ") == 0) {
      blocks.push(iter->substr(6));
      if (iter->substr(6).compare("state") == 0) {
        state_contents.clear();
        recording = true;
        active_state = NULL;
        loaded_vector = false;
      }
      if (recording) state_contents.push_back(*iter);
      continue;
    } else if (iter->substr(0,4).compare("END ") == 0) {
      if (blocks.top().compare(iter->substr(4)) == 0) {
        blocks.pop();
        
        // If closing out the 'actions' block, reset the last_action string
        if (iter->substr(4).compare("actions") == 0) {
          last_action = "";
        }
        
        state_contents.push_back(*iter);
        if (iter->substr(4).compare("state") == 0) {
          recording = false;
          bool success = active_state->unserialize(state_contents, hash_map);
          if (!success) {
            Log(stdout, ERROR, "Error unserializing State string!");
            return false;
          }
        }
      } else {
        char buf[1024];
        snprintf(buf, 1024, "QTable::Unserialize 2: Mismatched END block: %s",
                 blocks.top().c_str());
        Utils::Log(stdout, ERROR, buf);
        blocks.pop();
      }
      continue;
    }
   
    if (recording) state_contents.push_back(*iter);
    
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
        map<string, State*>::iterator found_state = 
          hash_map.find(s.get_state_hash());
        if (found_state == hash_map.end()) {
          char buf[4096];
          snprintf(buf, 4096, "Could not find state with hash %s in hash_map\n",
                   s.get_state_hash().c_str());
          Log(stdout, ERROR, buf);
        }  
        active_state = (found_state->second);
        if (!active_state) {
          char buf[1024];
          Log(stdout, ERROR, "Error loading qtable (2): State missing in hashmap");
          snprintf(buf, 1024, "Missing hash: '%s'\nHashmap size: %ld", 
                   s.get_state_hash().c_str(), hash_map.size());
          Log(stdout, ERROR, buf);
          return false;
        }
        loaded_vector = true;
      }
    } else if (blocks.top().compare("initiate_states") == 0) {
      AddInitiateState(hash_map[*iter]);
    } else if (blocks.top().compare("goal_states") == 0) {
      AddGoalState(hash_map[*iter], false);
    } else if (blocks.top().compare("trained_goal_states") == 0) {
      AddGoalState(hash_map[*iter], true);
    } else if (blocks.top().compare("nearby_thresholds") == 0) {
      vector<string> nt_values;
      vector<double> nt_vector;
      Utils::split_string(nt_values, *iter, ",");
      vector<string>::iterator nt_values_iter;
      for (nt_values_iter = nt_values.begin();
           nt_values_iter != nt_values.end();
           ++nt_values_iter) {
        double val = atof((*nt_values_iter).c_str());
        nt_vector.push_back(val);
      }
      set_nearby_thresholds(nt_vector);
    }
  }
  
  return true;
}


}  // namespace Primitives
