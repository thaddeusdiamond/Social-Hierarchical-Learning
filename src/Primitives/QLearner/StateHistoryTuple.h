/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Implementation
 */

#ifndef _SHL_PRIMITIVES_QLEARNER_STATEHISTORYTUPLE_H_
#define _SHL_PRIMITIVES_QLEARNER_STATEHISTORYTUPLE_H_

#include "QLearner/State.h"

namespace Primitives {

class StateHistoryTuple {
 public:
 /**
  * Default Constructor
  * 
  * @param state System State
  * @param timestamp System time in milliseconds when state was entered
  */
  explicit StateHistoryTuple(State * const state, double timestamp) {
    state_ = state;
    timestamp_ = timestamp;
  }

 /**
  * Copy Constructor
  */
  explicit StateHistoryTuple(StateHistoryTuple const &sht) {
    state_ = sht.state_;
    timestamp_ = sht.timestamp_;
  }

  /**
   * Pointer to state that was entered at timestamp_
   */
  State *state_;

  /**
   * System time in milliseconds when state was entered
   **/
  double timestamp_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_STATEHISTORYTUPLE_H_
