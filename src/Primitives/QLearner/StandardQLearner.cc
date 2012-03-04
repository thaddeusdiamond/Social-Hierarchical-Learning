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
#include "QLearner/QTable.h"
#include "QLearner/State.h"

namespace Primitives {

StandardQLearner::StandardQLearner(std::string name) {
  name_ = name;
  trials_ = 0;
  anticipated_duration_ = 0;
  exploration_type_ = NULL;
  credit_assignment_type_ = NULL;
}

StandardQLearner::StandardQLearner(std::string name, QTable qt) {
  name_ = name;
  q_table_ = qt;
  trials_ = 0;
  anticipated_duration_ = 0;
  exploration_type_ = NULL;
  credit_assignment_type_ = NULL;
}

bool StandardQLearner::Load(string const& filename) {
  return true;
}

bool StandardQLearner::Save(string const& filename) {
  return true;
}

bool StandardQLearner::Init(std::vector<Sensor *> const &sensors) {
  this->sensors_ = sensors;
  QTable qt;
  this->q_table_ = qt;

  std::vector<double> thresh;
  std::vector<Sensor *>::const_iterator iter;

  for (iter = sensors_.begin(); iter != sensors_.end(); ++iter) {
    thresh.push_back((*iter)->get_nearby_threshold());
  }

  this->q_table_.set_nearby_thresholds(thresh);
  return true;
}

bool StandardQLearner::Learn(State const& state) {
  // @TODO: This should start the primitives recording process
  return true;
}

bool StandardQLearner::AddNearbyEmptyStates(State const &state) {
  std::vector<double> base_descriptor = state.get_state_vector();
  std::vector<Sensor *>::const_iterator sensor_iter;
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
      if (!this->q_table_.GetState(increment_state, false)) {
        this->q_table_.AddState(increment_state);
      }
    }

    if (this->q_table_.HasState(decrement_state)) {
      if (!this->q_table_.GetState(decrement_state, false)) {
        this->q_table_.AddState(decrement_state);
      }
    }
  }

  return true;
}

/**
 * @todo Requires rewrite to not duplicate functionality introduced in QTable
 *       class. This function is meant mostly for adding possible states into
 *       the QTable so the exploration function has more choices to pick from.
 *       Concept is half-baked at the moment.
 **/
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
    std::vector<Sensor *>::const_iterator sensor_iter;
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



vector<State *> StandardQLearner::GetNearestFixedExecutionPath(
                                                  State *current_state) {
  vector<State *> exec_path;
  State *initiation_state = NULL;

  if (this->q_table_.get_initiate_states().size() == 0) return exec_path;

  if (current_state == NULL) 
    initiation_state = this->q_table_.get_initiate_states()[0];
  else  
    initiation_state = this->q_table_.GetNearestState(*current_state,
                                          this->q_table_.get_initiate_states());

  // No initiation state was found, can't start the action.
  if (initiation_state == NULL) return exec_path;

  exec_path.push_back(initiation_state);
  State *prev_state = initiation_state;
  while (true) {
    State *next_state = NULL;
    double transition_reward = 0.;
    this->GetNextState(prev_state, &next_state, transition_reward);

    // If we hit a dead end or a bad transition, break out
    if (next_state == NULL || transition_reward <= 0.) {
      exec_path.clear();
      break;
    }

    // Add 'next states' until we hit something near a goal state
    // if we hit a goal state, add it and break the loop

    exec_path.push_back(next_state);

    if (this->q_table_.IsGoalState(*next_state)) break;

    prev_state = next_state;
  }

  return exec_path;
}


bool StandardQLearner::GetNextState(State *cur_state,
                                    State **next_state,
                                    double &reward) {
  if (exploration_type_ == NULL) return false;
  return exploration_type_->GetNextState(cur_state, next_state, &reward);
}

bool StandardQLearner::AssignCredit(double signal) {
  return credit_assignment_type_->ApplyCredit(signal);
}

};  // namespace Primitives
