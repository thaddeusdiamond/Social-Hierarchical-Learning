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
    serialized_states.append("BEGIN state\n");
    serialized_states.append((*state_iter)->serialize());
    serialized_states.append("END state\n");
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
       
  serialized_table.append(serialized_states);
  serialized_table.append("BEGIN initiate_states");
  serialized_table.append(initiate_state_hashes);
  serialized_table.append("END initiate_states");
  serialized_table.append("BEGIN goal_states");
  serialized_table.append(goal_state_hashes);
  serialized_table.append("END goal_states");
  serialized_table.append("BEGIN trained_goal_states");
  serialized_table.append(trained_goal_state_hashes);
  serialized_table.append("END trained_goal_states");
  serialized_table.append("BEGIN nearby_thresholds");
  serialized_table.append(nearby_thresholds);
  serialized_table.append("END nearby_thresholds");
  
  return serialized_table;
}


}  // namespace Primitives
