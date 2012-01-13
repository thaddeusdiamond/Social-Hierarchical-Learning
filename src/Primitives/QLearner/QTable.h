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

namespace Primitives {

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
    std::vector<State const *> goal_states = q.get_goal_states();
    std::vector<State *>::const_iterator iter;
    for (iter = qstates.begin(); iter != qstates.end(); ++iter) {
      State *added_state = this->AddState(**iter);
      std::vector<State const *>::const_iterator iter;
      for (iter = goal_states.begin(); iter != goal_states.end(); ++iter) {
        if ((*iter)->Equals(*added_state)) {
          this->AddGoalState(added_state);
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
      delete (*iter);
      iter = states_.erase(iter);
    }
  }

  /**
   * @return direct access to states vector
   **/
  virtual std::vector<State *> & get_states() {
    return states_;
  }

  /**
   * @return direct access to goal states vector
   **/
  virtual std::vector<State const *> & get_goal_states() {
    return goal_states_;
  }

  /**
   * Checks if the QTable has a state described by needle, and if so returns
   * the internally stored version.
   *
   * @param needle State to find within the QTable
   * @param add_if_not_found Adds state to table with estimated rewards based on 
   *                         reward values of nearby states if true. 
   * @return NULL if needle not found, else: state pointer to internal version
   **/
  virtual State *GetState(State const &needle, bool add_if_not_found = false);

  /**
   * Checks if the QTable has a state described by needle via Bloom Filter.
   * Faster than GetState, but capable of false positives. Never false negative.
   *
   * @param needle State to find within QTable
   * @return true if state (probably) contained inside, false if not
   **/
  virtual bool HasState(State const &needle);

  /**
   * Copy the state into a piece of memory that the QTable owns/manages. Doesn't
   * check for a duplicate existing: assumes that you did your homework and you 
   * aren't re-adding something that already exists.
   *
   * @param state State to copy and insert into QTable
   * @return Pointer to internal copy of state param
   **/
  virtual State *AddState(State const &state);

  /**
   * Add the state pointed at by state to the list of goal states of this table
   * 
   * @param state Pointer to internally kept goal state
   */
  virtual void AddGoalState(State const * state) {
    if (isGoalState(*state)) return;
    goal_states_.push_back(state);
  }

  /**
   * Checks if the state provided is directly a goal state
   * 
   * @param state Any state object
   * @return true if found in list, false if not a goal state
   */
  virtual bool isGoalState(State const &state) {
    std::vector<State const *>::const_iterator iter;
    for (iter = goal_states_.begin(); iter != goal_states_.end(); ++iter) {
      if (state.Equals(**iter)) return true;
    }
    return false;
  }

  /**
   * Checks if the state provided is near a goal state
   * 
   * @param state Any state object
   * @param sensitivity Measure of how close to goal state is 'close enough'
   *                    This is currently used as a multiple of state unit
   *                    distance (sum over min. sensor change values in 
   *                    state vector).
   * @return true if found in list, false if not a goal state
   */
  virtual bool isNearGoalState(State const &state, double sensitivity) {
    std::vector<State const *>::iterator iter;
    for (iter = goal_states_.begin(); iter != goal_states_.end(); ++iter) {
      if (state.GetSquaredDistance(*iter) <
          sensitivity * state.get_unit_distance())
        return true;
    }
    return false;
  }

 private:
  /**
   * Huge array of all states seen thus far 
   **/
  std::vector<State *> states_;
  std::vector<State const *> goal_states_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_QTABLE_H_
