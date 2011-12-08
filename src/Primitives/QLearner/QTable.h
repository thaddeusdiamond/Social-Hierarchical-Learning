/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Table (Database)
 */

#ifndef _SHL_PRIMITIVES_LEARNER_QTABLE_H_
#define _SHL_PRIMITIVES_LEARNER_QTABLE_H_

#include <string>
#include <vector>
#include "QLearner/State.h"

class QTable {
 public:
  /**
   * Default Constructor
   **/
  explicit QTable() { }

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
  virtual std::vector<State *> get_states() const {
    return states_;
  }

  /**
   * Checks if the QTable has a state described by needle, and if so returns
   * the internally stored version.
   *
   * @param needle State to find within the QTable
   * @return NULL if needle not found, else: state pointer to internal version
   **/
  virtual State *GetState(State const &needle);

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

 private:
  /**
   * Huge array of all states seen thus far 
   **/
  std::vector<State *> states_;
};


#endif  // _SHL_PRIMITIVES_LEARNER_QTABLE_H_
