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

class State;

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
    std::vector<State const *> trained_goal_states = 
        q.get_trained_goal_states();
    std::vector<State *>::const_iterator iter;
    for (iter = qstates.begin(); iter != qstates.end(); ++iter) {
      State *added_state = this->AddState(**iter);
      std::vector<State const *>::const_iterator iter;
      for (iter = goal_states.begin(); iter != goal_states.end(); ++iter) {
        if ((*iter)->Equals(*added_state)) {
          this->AddGoalState(added_state,false);
          break;
        }
      }

      for (iter = trained_goal_states.begin(); 
           iter != trained_goal_states.end(); ++iter) {
        if ((*iter)->Equals(*added_state)) {
          this->AddGoalState(added_state,true);
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
  std::vector<State *> & get_states() {
    return states_;
  }

  /**
   * @return direct access to 'intuited' goal states vector
   **/
  std::vector<State const *> & get_goal_states() {
    return goal_states_;
  }

  /**
   * @return direct access to trained goal states vector
   **/
  std::vector<State const *> & get_trained_goal_states() {
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
  void AddGoalState(State const * state, bool from_training) {
    if (IsGoalState(*state)) return;
    if (from_training)
      trained_goal_states_.push_back(state);
    else
      goal_states_.push_back(state);
  }

  /**
   * Checks if the state provided is a known goal state
   * 
   * @param state Any state object
   * @return true if found in list, false if not a goal state
   */
  bool IsGoalState(State const &state) {
    std::vector<State const *>::const_iterator iter;
    for (iter = trained_goal_states_.begin(); 
         iter != trained_goal_states_.end(); ++iter) {
      if (state.Equals(**iter)) return true;
    }

    for (iter = goal_states_.begin(); iter != goal_states_.end(); ++iter) {
      if (state.Equals(**iter)) return true;
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
    std::vector<State const *>::const_iterator iter;
    for (iter = trained_goal_states_.begin(); 
         iter != trained_goal_states_.end(); ++iter) {
      if (state.Equals(**iter)) return true;
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

    std::vector<double>::const_iterator iter;
    unsigned int i;
    for (i=0, iter = nearby_thresholds_.begin(); 
         iter != nearby_thresholds_.end(); ++iter, ++i) {

      double dist = a_sv[i] - b_sv[i];
      dist *= dist;
      if (dist > *iter) return false;
    }
    
    return true;
  }


 private:
  /**
   * Huge array of all states seen thus far 
   **/
  std::vector<State *> states_;
  std::vector<State const *> goal_states_;
  std::vector<State const *> trained_goal_states_;
  
  // Squared thresholds for a point to be "nearby" some other point
  std::vector<double> nearby_thresholds_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_QTABLE_H_
