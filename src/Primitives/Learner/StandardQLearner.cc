/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a dummy student that can learn
 */

#include "Learner/StandardQLearner.h"

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
  return true;
}

bool StandardQLearner::GetNearbyStates(
  State const& cur_state, vector<State const* const>& nearby_states) {
  return true;
}

bool StandardQLearner::GetNextState(State const& cur_state,
                                    State const* const next_state) {
  return true;
}

bool StandardQLearner::SetCreditFunction(
  CreditAssignmentType* const credit_assigner) {
  return true;
}

bool StandardQLearner::SetExplorationFunction(ExplorationType* const explorer) {
  return true;
}

bool StandardQLearner::SetEnvironment(
  vector<Sensor* const> const& sensor_list) {
  return true;
}
