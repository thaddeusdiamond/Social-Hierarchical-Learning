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

namespace Primitives {

bool StandardQLearner::Load(string const& filename) {
  return true;
}

bool StandardQLearner::Save(string const& filename) {
  return true;
}

bool StandardQLearner::Init(std::vector<Sensor const *> const &sensors) {
  this->sensors_ = sensors;
  QTable qt;
  this->q_table_ = qt;
  return true;
}

bool StandardQLearner::Learn(State const& state) {
  State *existing_state = this->q_table_.GetState(state);
  if (existing_state) {
    existing_state->set_base_reward(state.get_base_reward());
    return true;
  } else {
    State *s = this->q_table_.AddState(state);
    return (s != NULL);
  }
}

bool StandardQLearner::AddNearbyEmptyStates(State const &state) {
  std::vector<double> base_descriptor = state.get_state_vector();
  std::vector<Sensor const *>::const_iterator sensor_iter;
  int i = 0;
  for (i = 0, sensor_iter = sensors_.begin(); sensor_iter != sensors_.end();
       ++i, ++sensor_iter) {
    double min_increment = (*sensor_iter)->get_min_increment();
    std::vector<double> incr_descriptor = base_descriptor;
    std::vector<double> decr_descriptor = base_descriptor;

    incr_descriptor[i] += min_increment;
    decr_descriptor[i] -= min_increment;

    State increment_state(incr_descriptor);
    State decrement_state(decr_descriptor);

    if (this->q_table_.HasState(increment_state)) {
      if (!this->q_table_.GetState(increment_state)) {
        this->q_table_.AddState(increment_state);
      }
    }

    if (this->q_table_.HasState(decrement_state)) {
      if (!this->q_table_.GetState(decrement_state)) {
        this->q_table_.AddState(decrement_state);
      }
    }
  }

  return true;
}


bool StandardQLearner::GetNearbyStates(
  State const& cur_state, std::vector<State const *>& nearby_states) {
  // Retrieve all states within search_distances of the cur_state that are
  // in the qtable.

  this->AddNearbyEmptyStates(cur_state);

  std::vector<State *> all_states = this->q_table_.get_states();

  std::vector<State *>::const_iterator iter;
  bool state_distance_fail = false;
  for (iter = all_states.begin(); iter != all_states.end(); ++iter) {
    State *cmp_state = (*iter);
    state_distance_fail = false;
    // iterate through each value in the state and check if it's within required
    // distance from the current state (according to the internal sensor vector)
    std::vector<Sensor const *>::const_iterator sensor_iter;
    int i;
    for (i = 0, sensor_iter = sensors_.begin(); sensor_iter != sensors_.end();
         ++sensor_iter, ++i) {
      double dist = cmp_state->get_state_vector()[i]
                    - cur_state.get_state_vector()[i];
      dist = (dist < 0) ? -dist : dist;
      if (dist > (*sensor_iter)->get_nearby_threshold()) {
        state_distance_fail = true;
        break;
      }
    }

    if (state_distance_fail) continue;
    nearby_states.push_back(*iter);
  }

  return (nearby_states.size() > 0);
}

bool StandardQLearner::GetNextState(State const& cur_state,
                                    State const** next_state) {
  if (exploration_type_ == NULL) {
    std::vector<State const *> nearby_states;
    this->GetNearbyStates(cur_state, nearby_states);

    std::vector<State const *>::const_iterator iter;
    double best_score = 0.;
    std::vector<State const *> best_states;
    for (iter = nearby_states.begin(); iter != nearby_states.end(); ++iter) {
      if ((*iter)->get_reward() > best_score) {
        best_states.clear();
        best_score = (*iter)->get_reward();
        best_states.push_back(*iter);
      } else if ((*iter)->get_reward() == best_score) {
        best_states.push_back(*iter);
      }
    }

    // Randomly choose from the best available states
    int idx = rand() % best_states.size();

    // Set return value
    (*next_state) = best_states[idx];

    // Couldn't find any next state -- we failed!
    if (best_states.size() == 0) return false;

    return true;
  } else {
    return exploration_type_->GetNextStep(cur_state, next_state);
  }
}

bool StandardQLearner::AssignCredit(double signal) {
  return credit_assignment_type_->ApplyCredit(signal);
}

}  // namespace Primitives
