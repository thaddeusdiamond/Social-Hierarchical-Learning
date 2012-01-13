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

namespace Primitives {

State *QTable::GetState(State const &needle, bool add_if_not_found) {
  // Search through the huge states_ vector for the target state

  std::vector<State *> nearby_states;
  std::vector<double> nearby_state_dists;
  std::vector<State *>::iterator iter;
  for (iter = states_.begin(); iter != states_.end(); iter++) {
    if (*iter == NULL) continue;  // Shouldn't have deleted states in the table

    if (needle.Equals(**iter))
      return (*iter);
    else if (add_if_not_found) {
      double square_dist = needle.GetSquaredDistance(*iter);
      double thresh_dist = State::NEARBY_STATE_THRESHOLD
                           * needle.get_unit_distance();
      thresh_dist *= thresh_dist;
      if (square_dist < thresh_dist) {
        nearby_states.push_back(*iter);
        nearby_state_dists.push_back(square_dist);
      }
    }
  }

  if (add_if_not_found) {
    State s(needle.get_state_vector(), needle.get_unit_distance(), 0.);
    double base_reward = 0.;
    std::map<std::string, double> reward_layers;
    double max_dist = State::NEARBY_STATE_THRESHOLD
                      * needle.get_unit_distance();

    std::vector<State *>::iterator nearby_iter;
    std::vector<double>::iterator nearby_dist_iter;

    // For each nearby state, take a fraction of its reward related to distance
    // and apply it to the new state to be created
    for (nearby_iter = nearby_states.begin(),
         nearby_dist_iter = nearby_state_dists.begin();
         nearby_iter != nearby_states.end(),
         nearby_dist_iter != nearby_state_dists.end();
         ++nearby_iter, ++nearby_dist_iter) {
      State *near_state = *nearby_iter;
      std::map<std::string, double> layers = near_state->get_reward_layers();
      double weighted_percent_multiple = (max_dist - *nearby_dist_iter)
                                         /  (max_dist);

      // Add a proportion of the base reward from neighboring states
      base_reward += near_state->get_base_reward()
                     * weighted_percent_multiple;

      // Handle the layered cases
      std::map<std::string, double>::iterator reward_layer_iter;
      for (reward_layer_iter = layers.begin();
           reward_layer_iter != layers.end();
           ++reward_layer_iter) {
        double partial_layer_reward = (*reward_layer_iter).second
                                      * weighted_percent_multiple;

        if (reward_layers.find((*reward_layer_iter).first)
            == reward_layers.end())
          reward_layers[(*reward_layer_iter).first] = partial_layer_reward;
        else
          reward_layers[(*reward_layer_iter).first] += partial_layer_reward;
      }
    }

    s.set_base_reward(base_reward);
    s.set_reward_layers(reward_layers);
    return this->AddState(s);
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
