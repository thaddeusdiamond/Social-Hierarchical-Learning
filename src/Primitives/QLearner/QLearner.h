/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Implementation
 **/

#ifndef _SHL_PRIMITIVES_QLEARNER_QLEARNER_H_
#define _SHL_PRIMITIVES_QLEARNER_QLEARNER_H_

#include <sys/time.h>
#include <string>
#include <vector>
#include <stack>
#include "QLearner/State.h"
#include "QLearner/StateHistoryTuple.h"
#include "Exploration/ExplorationType.h"
#include "Credit/CreditAssignmentType.h"
#include "QLearner/QTable.h"

namespace Primitives {

using std::string;
using std::vector;

class CreditAssignmentType;
class ExplorationType;
class Sensor;

class QLearner {
  public:
  /**
   * Destructor for QLearner must free all memory it received from I/O and
   * had buffered
   **/
  virtual ~QLearner() {}

  /**
   * Populates this object with the QTable contained in the target file
   *
   * @param     filename        Path to file containing saved QTable
   *
   * @return    True on success, false on failure.
   **/
  virtual bool Load(string const& filename) = 0;

  /**
   * Saves the entire contents of the QTable to the target file
   *
   * @param     filename        Destination file. Created if doesn't exist,
   *                            overwritten if it does.
   *
   * @return    True on success, false on failure
   **/
  virtual bool Save(string const& filename) = 0;

  /**
   * Clears table, initializes everything in the object to pristine and
   * usable state.
   *
   * @param sensors The sensors that are associated with the various state
   *                variables, consistent wiht the order the state vars
   *                will be passed in to the Learn function
   * @return    True on success, false on failure
   **/
  virtual bool Init(std::vector<Sensor const *> const &sensors) = 0;

  /**
   * Copies the state data provided to it and records it in the QTable
   *
   * @param     state           State description with reward
   *
   * @return    True on successful modification of QTable, false on failure.
   **/
  virtual bool Learn(State const& state) = 0;

  /**
   * Populates nearby_states with a list of neighboring state descriptors
   * and the reward values currently associated with them.
   *
   * @param     cur_state        Vector of state descriptors
   * @param     nearby_states    Empty vector of state descriptors to be
   *                             populated with const pointers from within this
   *                             object by the time the function returns
   *
   *
   * @return True on success, false on lookup error.
   **/
  virtual bool GetNearbyStates(State const& cur_state,
                               std::vector<State const *>& nearby_states) = 0;


  /**
   * Returns the chosen next step by the QLearner.
   *
   * @param     cur_state       State descriptor
   * @param     next_state      Overwritten with pointer to next State object
   *
   * @return True on success, false on lookup error
   **/
  virtual bool GetNextState(State const& cur_state,
                            State const** next_state) = 0;

  /**
   * Sets the credit assignment type used by this QLearner. Provided object
   * will get a pointer back to this object to allow it to use all
   * available information for its decisions
   *
   * @param     credit_assigner Instantiated credit assignment implementation
   *
   * @return    True if initialized properly, false if error.
   **/
  virtual bool SetCreditFunction(
    CreditAssignmentType* const credit_assigner) {
    credit_assignment_type_ = credit_assigner;
    return (credit_assignment_type_ != NULL);
  };

  /**
   * Sets the exploration function used by this QLearner. Provided object
   * will get a pointer back to this object to allow it to use all
   * possible information available for its decisions
   *
   * @param     explorer Instantiated exploration function implementation
   *
   * @return    True if initialized properly, false if error.
   **/
  virtual bool SetExplorationFunction(ExplorationType* const explorer) {
    exploration_type_ = explorer;
    return (exploration_type_ != NULL);
  }

  /**
   * Copies Sensor pointers from provided list to be polled
   * (in order given) when appending environmental data to
   * the state information given to the Learn function.
   **/
  virtual bool SetEnvironment(vector<Sensor* const> const& sensor_list) = 0;

  /**
   * Applies a reinforcement signal through this QLearner's CreditAssignmentType
   * Should be applied after setting the current state of the system.
   *
   * @param signal Double describing the feedback being received
   * @return true on successful application, false on error
   **/
  virtual bool AssignCredit(double signal) = 0;

  /**
   * Updates the state_history_ stack to reflect this system state
   *
   * @param state Current state of the system
   **/
  virtual void SetCurrentState(State * const state) {
    timeval now;
    gettimeofday(&now, NULL);
    double millis = (now.tv_sec*1000.) + (now.tv_usec/1000.);
    StateHistoryTuple sht(state, millis);
    state_history_.push(sht);
  }

  /**
   * Returns a stack of recently visited states
   *
   * @return Stack of StateHistoryTuple
   **/
  std::stack<StateHistoryTuple> &get_state_history() {
    return state_history_;
  }

  QTable &get_q_table() { return q_table_; }

  std::string get_name() { return name_; }
  void set_name(std::string name) { name_ = name; }

 protected:
  std::stack<StateHistoryTuple> state_history_;
  QTable q_table_;
  std::string name_;
  CreditAssignmentType *credit_assignment_type_;
  ExplorationType *exploration_type_;
  std::vector<Sensor const *> sensors_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_QLEARNER_H_
