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
    std::vector<double>::iterator thresh_iter;
    std::vector<double>::iterator dist_iter;
    for (thresh_iter = nearby_thresholds_.begin(),
         dist_iter = dists.begin();
         thresh_iter != nearby_thresholds_.end()
         && dist_iter != dists.end(); ++thresh_iter, ++dist_iter) {
      double squared_thresh = (*thresh_iter);
      double squared_dist = (*dist_iter);
      if (squared_dist > squared_thresh) {
        fail = true;
        break;
      }
    }
    if (!fail) nearby_states.push_back(*iter);
  }

  return nearby_states;
}

State *QTable::GetState(State const &needle, bool add_estimated_state) {
  // Search through the huge states_ vector for the target state

  std::vector<double> nearby_state_dists = this->get_nearby_thresholds();
  std::vector<State *>::iterator iter;
  for (iter = states_.begin(); iter != states_.end(); iter++) {
    if (*iter == NULL) continue;  // Shouldn't have deleted states in the table

    if (needle.Equals(**iter))
      return (*iter);
  }




  if (add_estimated_state) {
    State s(needle.get_state_vector());
    State *new_state = this->AddState(s);

    std::vector<State *> nearby_states = this->GetNearbyStates(needle);
    std::map<std::string, double> reward_layers;

    std::vector<State *>::iterator nearby_iter;
    std::vector<double>::iterator nearby_dist_iter;
    std::vector<double>::iterator nearby_threshold_iter;

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

      for (nearby_threshold_iter = nearby_state_dists.begin(),
           nearby_dist_iter = squared_dists.begin();
           nearby_threshold_iter != nearby_state_dists.end(),
           nearby_dist_iter != squared_dists.end();
           ++nearby_threshold_iter, ++nearby_dist_iter) {
        weight += 1. - ((*nearby_dist_iter) / (*nearby_threshold_iter));
      }
      weight /= squared_dists.size();


      // Add the same reward transitions as the found state, reward value
      // weighted by the distance of the found state from the needle state
      // --Only transfer the 'base' layer--
      std::map<State*, std::map<std::string, double> > const &rewards
        = near_state->get_reward();

      std::map<State*, std::map<std::string, double> >::const_iterator
        state_reward_iter;

      std::string base_layer = std::string("base");

      // iterate through each state near_state links to, and copy the base
      // reward layer to the new state
      for (state_reward_iter = rewards.begin();
           state_reward_iter != rewards.end();
           ++state_reward_iter) {
          State *target_state = (*state_reward_iter).first;
          std::map<std::string, double> const &reward_layers =
            (*state_reward_iter).second;

          double base_reward = 0.;
          std::map<std::string, double>::const_iterator reward_layer =
            reward_layers.find(base_layer);
          if (reward_layer != reward_layers.end()) {
            base_reward = (*reward_layer).second * weight;
          }

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

}  // namespace Primitives
