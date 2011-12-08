/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a standard Table-based QLearner
 */

#include "QLearner/StandardQLearner.h"

bool StandardQLearner::Load(string const& filename) {
  return true;
}

bool StandardQLearner::Save(string const& filename) {
  return true;
}

bool StandardQLearner::Init() {
  return true;
}

bool StandardQLearner::Learn(State const& state) {
    State *s = this->q_table_.AddState(state);
    return (s != NULL);
}

bool StandardQLearner::GetNearbyStates(
  State const& cur_state, double search_radius,
  std::vector<State const *>& nearby_states) {
  // Retrieve all states within search_distances of the cur_state that are
  // in the qtable.

  double search_radius_squared = search_radius * search_radius;
  std::vector<State *> all_states = this->q_table_.get_states();

  std::vector<State *>::const_iterator iter;
  for (iter = all_states.begin(); iter != all_states.end(); ++iter) {
    double state_dist = cur_state.GetSquaredDistance(*iter);
    if (state_dist < 0.) continue;
    if (state_dist <= search_radius_squared) {}
      nearby_states.push_back(*iter);
  }

  return true;
}

bool StandardQLearner::GetNextState(State const& cur_state,
                                    State const** next_state) {
  if (exploration_type_ == NULL) {
    std::vector<State *> all_states = this->q_table_.get_states();

    std::vector<State *>::const_iterator iter;
    double best_score = 0.;
    State const *best_state = NULL;
    for (iter = all_states.begin(); iter != all_states.end(); ++iter) {
      if (best_state == NULL) {
        best_score = (*iter)->get_reward();
        best_state = (*iter);
      }

      if ((*iter)->get_reward() > best_score) {
        best_score = (*iter)->get_reward();
        best_state = (*iter);
      }
    }

    // Set return value
    (*next_state) = best_state;

    // Couldn't find any next state -- we failed!
    if (best_state == NULL) return false;

    return true;
  } else {
    return exploration_type_->GetNextStep(cur_state, next_state);
  }
}

bool StandardQLearner::AssignCredit(double signal) {
  return credit_assignment_type_->ApplyCredit(signal);
}
