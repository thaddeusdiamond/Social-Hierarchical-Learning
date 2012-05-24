/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Implementation
 */

#ifndef _SHL_PRIMITIVES_QLEARNER_STANDARDQLEARNER_H_
#define _SHL_PRIMITIVES_QLEARNER_STANDARDQLEARNER_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include "QLearner/QLearner.h"
#include "QLearner/QTable.h"
#include "QLearner/StateHistoryTuple.h"

namespace Primitives {

class QLearner;
class QTable;
class State;

class StandardQLearner : public QLearner {
 public:
  explicit StandardQLearner(std::string name);
  StandardQLearner(std::string name, QTable qt);
  ~StandardQLearner() {
    if (exploration_type_)
      delete exploration_type_;
    if (credit_assignment_type_)
      delete credit_assignment_type_;
  }

/**
   * Populates this object with the QTable contained in the target file
   *
   * @param     filename        Path to file containing saved QTable
   *
   * @return    True on success, false on failure.
   */
  virtual bool Load(string const& filename);

  /**
   * Saves the entire contents of the QTable to the target file
   *
   * @param     filename        Destination file. Created if doesn't exist,
   *                            overwritten if it does.
   *
   * @return    True on success, false on failure
   */
  virtual bool Save(string const& filename);

  /**
   * Clears table, initializes everything in the object to pristine and
   * usable state.
   *
   * @return    True on success, false on failure
   */
  virtual bool Init(std::vector<Sensor *> const &sensors);

  /**
   * Populates nearby_states with a list of neighboring state descriptors
   * and the reward values currently associated with them.
   *
   * @param     cur_state        Vector of state descriptors
   * @param     nearby_states    Empty vector of state descriptors to be
   *                             populated with const pointers from within this
   *                             object by the time the function returns
   *
   * @return True on success, false on lookup error.
   */
  virtual bool GetNearbyStates(State const& cur_state,
                       vector<State const *>& nearby_states);


  /**
   * Returns the chosen next step by the QLearner.
   *
   * @param     cur_state       State descriptor
   * @param     next_state      Overwritten with pointer to next State object
   *
   * @return True on success, false on lookup error
   */
  virtual bool GetNextState(State *cur_state,
                            State **next_state,
                            double &reward);


  /**
   * Applies a reinforcement signal through this QLearner's CreditAssignmentType
   * Should be applied after setting the current state of the system.
   *
   * @param signal Double describing the feedback being received
   * @return true on successful application, false on error
   **/
  virtual bool AssignCredit(double signal);

  /**
   * Gets a fixed-action-path execution of a skill.
   * @param current_state Pointer to an initiation state (or NULL to select
   *                      the first known initiation state in the skill)
   */
  virtual vector<State *> GetNearestFixedExecutionPath(State *current_state);

 private:
  StandardQLearner() {}
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_STANDARDQLEARNER_H_
