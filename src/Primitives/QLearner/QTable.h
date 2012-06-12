/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Table (Database)
 */

#ifndef _SHL_PRIMITIVES_QLEARNER_QTABLE_H_
#define _SHL_PRIMITIVES_QLEARNER_QTABLE_H_

#include <string>
#include <vector>
#include "QLearner/State.h"
#include "Common/Utils.h"

namespace Primitives {

class State;
using Utils::Log;

class QTable {
 public:
  /**
   * Default Constructor
   **/
  explicit QTable() { }

  /**
   * Copy Constructor
   **/
  explicit QTable(QTable *q_table) {
    QTable &q = (*q_table);
    std::vector<State *> qstates = q.get_states();
    std::vector<State *> goal_states = q.get_goal_states();
    std::vector<State *> trained_goal_states =
        q.get_trained_goal_states();
    std::vector<State *>::iterator iter;
    for (iter = qstates.begin(); iter != qstates.end(); ++iter) {
      State *added_state = this->AddState(**iter);
      std::vector<State *>::iterator iter;
      for (iter = goal_states.begin(); iter != goal_states.end(); ++iter) {
        if ((*iter)->Equals(added_state)) {
          this->AddGoalState(added_state, false);
          break;
        }
      }

      for (iter = trained_goal_states.begin();
           iter != trained_goal_states.end(); ++iter) {
        if ((*iter)->Equals(added_state)) {
          this->AddGoalState(added_state, true);
          break;
        }
      }
    }
  }

  /**
   * Destructor for QTable: Deletes all states internally created/held
   **/
  virtual ~QTable() {
    std::vector<State *>::iterator iter;
    for (iter = states_.begin(); iter != states_.end(); iter++) {
      if (*iter)
        delete (*iter);
    }
    states_.clear();
  }

  /**
   * @return direct access to states vector
   **/
  std::vector<State *> & get_states() {
    return states_;
  }

  /**
   * @return direct access to 'intuited' goal states vector
   **/
  std::vector<State *> & get_goal_states() {
    return goal_states_;
  }

  /**
   * @return direct access to trained goal states vector
   **/
  std::vector<State *> & get_trained_goal_states() {
    return trained_goal_states_;
  }

  /**
   * @return "Nearby" threshold distances for each sensor dimension
   **/
  std::vector<double> & get_nearby_thresholds() {
    return nearby_thresholds_;
  }

  /**
   * @todo Must make sure this is set before allowing most QTable
   * operations to occur.
   *
   * Sets the 'nearby' distance thresholds for each state dimension.
   * Squares all of the distance values coming in, to optimize distance
   * checks later on (eliminating need for constant sqrt'ing).
   * @param thresh vector of distance thresholds
   **/
  void set_nearby_thresholds(std::vector<double> const &thresh) {
    nearby_thresholds_ = thresh;
    std::vector<double>::iterator iter;
    for (iter = nearby_thresholds_.begin(); iter != nearby_thresholds_.end();
         ++iter) {
      double val = (*iter);
      val *= val;
      (*iter) = val;
    }
  }

  /**
   * Checks if the QTable has a state described by needle, and if so returns
   * the internally stored version.
   *
   * @param needle State to find within the QTable
   * @param add_estimated_state Adds state to table with estimated rewards based
                                on reward values of nearby states if true.
   * @return NULL if needle not found, else: state pointer to internal version
   **/
  State *GetState(State const &needle, bool add_estimated_state);


  /**
   * Checks if the QTable has a state described by needle, and if so returns
   * the internally stored version.
   *
   * @param needle_hash Hash of State to find within the QTable
   * @return NULL if needle not found, else: state pointer to internal version
   **/
  State *GetState(std::string needle_hash);

  /**
   * Checks if the QTable has a state described by needle via Bloom Filter.
   * Faster than GetState, but capable of false positives. Never false negative.
   *
   * @param needle State to find within QTable
   * @return true if state (probably) contained inside, false if not
   **/
  bool HasState(State const &needle);

  /**
   * Returns a vector of existing states determined to be 'nearby'
   * to the needle state
   * @param needle State to look near for existing states
   * @return vector of nearby states
   **/
  std::vector<State*> GetNearbyStates(State const &needle);

  /**
   * Returns a vector of existing states that have an outbound link to the
   * state provided. This is guaranteed to only return states
   */
  std::vector<State*> GetIncomingStates(State const &s);

  /**
   * Copy the state into a piece of memory that the QTable owns/manages. Doesn't
   * check for a duplicate existing: assumes that you did your homework and you
   * aren't re-adding something that already exists.
   *
   * @param state State to copy and insert into QTable
   * @return Pointer to internal copy of state param
   **/
  State *AddState(State const &state);

  /**
   * Add the state pointed at by state to the list of goal states of this table
   *
   * @param state Pointer to internally kept goal state
   * @param from_training Is this a goal state from training data
   *                      or was it just within a threshold of a real
   *                      goal state?
   */
  void AddGoalState(State *state, bool from_training) {
    if (IsGoalState(*state)) return;
    if (from_training)
      trained_goal_states_.push_back(state);
    else
      goal_states_.push_back(state);
  }


  /**
   * Add the state pointed at by state to the list of goal states of this table
   *
   * @param state Pointer to internally kept goal state
   * @param from_training Is this a goal state from training data
   *                      or was it just within a threshold of a real
   *                      goal state?
   */
  void AddInitiateState(State *state) {
    if (IsInitiateState(*state)) return;
    initiate_states_.push_back(state);
  }

  /**
   * Checks if the state provided is a known initiate state
   *
   * @param state Any state object
   * @return true if found in list, false if not a starting state
   */
  bool IsInitiateState(State const &state) {
    std::vector<State *>::const_iterator iter;
    for (iter = initiate_states_.begin();
         iter != initiate_states_.end(); ++iter) {
      if (state.Equals(*iter)) return true;
    }

    for (iter = initiate_states_.begin();
         iter != initiate_states_.end();
         ++iter) {
      if (state.Equals(*iter)) return true;
    }

    return false;
  }

  vector<State *> get_initiate_states() { return initiate_states_; }

  /**
   * Returns the nearest state in candidates to 'state'
   *
   * @param state Needle to search for
   * @param candidates possible states to find closest to
   * @return State pointer to closest found state
   **/
  State *GetNearestState(State const &state, vector<State*> const &candidates) {
      vector<State*>::const_iterator iter;
      double best_dist;
      State *best_guess = NULL;

      for (iter = candidates.begin(); iter != candidates.end(); ++iter) {
        State *cand_state = (*iter);
        vector<double> dists = state.GetSquaredDistances(cand_state);

        double dist = 0.;
        vector<double>::iterator dist_iter;
        for (dist_iter = dists.begin(); dist_iter != dists.end();
             ++dist_iter) {
          dist += *dist_iter;
        }

        if (best_guess == NULL || dist < best_dist) {
          best_guess = cand_state;
          best_dist = dist;
        }
      }

      return best_guess;
  }

  /**
   * Checks if the state provided is a known goal state
   *
   * @param state Any state object
   * @return true if found in list, false if not a goal state
   */
  bool IsGoalState(State const &state) {
    std::vector<State *>::const_iterator iter;
    for (iter = trained_goal_states_.begin();
         iter != trained_goal_states_.end(); ++iter) {
      if (state.Equals(*iter)) return true;
    }

    for (iter = goal_states_.begin(); iter != goal_states_.end(); ++iter) {
      if (state.Equals(*iter)) return true;
    }

    return false;
  }

  /**
   * Checks if the state provided is a trained goal state, as opposed
   * to a 'learned' or 'approximate' goal state.
   *
   * @param state Any state object
   * @return true if found in list, false if not a goal state
   */
  bool IsTrainedGoalState(State const &state) {
    std::vector<State *>::const_iterator iter;
    for (iter = trained_goal_states_.begin();
         iter != trained_goal_states_.end(); ++iter) {
      if (state.Equals(*iter)) return true;
    }

    return false;
  }


  /**
   * Checks if the states provided are nearby each other
   *
   * @param a Any state object
   * @param b Any state object
   * @return true if a and b are within nearby thresholds for all elements
   */
  bool IsNearState(State const &a, State const &b) {
    std::vector<double> const &a_sv = a.get_state_vector();
    std::vector<double> const &b_sv = b.get_state_vector();

    if (a.get_state_vector().size() != b.get_state_vector().size()
        || a.get_state_vector().size() != nearby_thresholds_.size())
      return false;

    unsigned int i;
    for (i = 0; i < nearby_thresholds_.size(); ++i) {
      double dist = a_sv[i] - b_sv[i];
      dist *= dist;
      if (dist > nearby_thresholds_[i]) return false;
    }

    return true;
  }


  /**
   * Serializes the QTable so it can be saved to file
   **/
  std::string serialize();

  /**
   * Restores the QTable from a file
   **/
  bool unserialize(std::vector<std::string> const &contents);

 private:
  /**
   * Huge array of all states seen thus far
   **/
  std::vector<State *> states_;

  /**
   * States that can signal the beginning of this skill
   **/
  std::vector<State *> initiate_states_;

  /**
   * States that can signal the completion of this skill
   **/
  std::vector<State *> goal_states_;

  /**
   * States that can signal the completion of this skill
   * as given by training data.
   **/
  std::vector<State *> trained_goal_states_;

  // Squared thresholds for a point to be "nearby" some other point
  std::vector<double> nearby_thresholds_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_QTABLE_H_
