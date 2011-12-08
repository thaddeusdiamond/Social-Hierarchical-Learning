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

#include "QLearner/QTable.h"

State *QTable::GetState(State const &needle) {
  // Search through the huge states_ vector for the target state

  std::vector<State *>::iterator iter;
  for (iter = states_.begin(); iter != states_.end(); iter++) {
    if (*iter == NULL) continue;  // Shouldn't have deleted states in the table

    if (needle.Equals(**iter))
      return (*iter);
  }

  return NULL;
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
