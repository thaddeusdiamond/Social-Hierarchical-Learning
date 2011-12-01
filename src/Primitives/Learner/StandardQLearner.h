/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Implementation
 */

#ifndef _SHL_PRIMITIVES_LEARNER_STANDARDQLEARNER_H_
#define _SHL_PRIMITIVES_LEARNER_STANDARDQLEARNER_H_

#include <string>
#include <vector>
#include "Learner/QLearner.h"
#include "Learner/QTable.h"
#include "Learner/StateHistoryTuple.h"


class StandardQLearner : public QLearner {
 public:
  explicit StandardQLearner() {}

/**
   * Populates this object with the QTable contained in the target file
   *
   * @param     filename        Path to file containing saved QTable
   *
   * @return    True on success, false on failure.
   */
  bool Load(string const& filename);

  /**
   * Saves the entire contents of the QTable to the target file
   *
   * @param     filename        Destination file. Created if doesn't exist,
   *                            overwritten if it does.
   *
   * @return    True on success, false on failure
   */
  bool Save(string const& filename);

  /**
   * Clears table, initializes everything in the object to pristine and
   * usable state.
   *
   * @return    True on success, false on failure
   */
  bool Init();

  /**
   * Copies the state data provided to it and records it in the QTable
   *
   * @param     state           State description with reward
   *
   * @return    True on successful modification of QTable, false on failure.
   */
  bool Learn(State const& state);

  /**
   * Populates nearby_states with a list of neighboring state descriptors
   * and the reward values currently associated with them.
   *
   * @param     cur_state        Vector of state descriptors 
   * @param     search_radius    Double indicating euclidean distance to search
   * @param     nearby_states    Empty vector of state descriptors to be
   *                             populated with const pointers from within this
   *                             object by the time the function returns
   *                            
   * @return True on success, false on lookup error.
   */
  bool GetNearbyStates(State const& cur_state,
                       double search_radius,
                       vector<State const *>& nearby_states);


  /**
   * Returns the chosen next step by the QLearner.
   *
   * @param     cur_state       State descriptor
   * @param     next_state      Overwritten with pointer to next State object
   * 
   * @return True on success, false on lookup error
   */
  bool GetNextState(State const& cur_state,
                    State const** next_state);

  /**
   * Sets the credit assignment type used by this QLearner. Provided object
   * will get a pointer back to this object to allow it to use all
   * available information for its decisions
   *
   * @param     credit_assigner Instantiated credit assignment implementation
   *
   * @return    True if initialized properly, false if error.
   */
  bool SetCreditFunction(
    CreditAssignmentType* const credit_assigner);

  /**
   * Sets the exploration function used by this QLearner. Provided object
   * will get a pointer back to this object to allow it to use all
   * possible information available for its decisions
   *
   * @param     explorer Instantiated exploration function implementation
   *
   * @return    True if initialized properly, false if error.
   */
  bool SetExplorationFunction(ExplorationType* const explorer);

  /**
   * Copies Sensor pointers from provided list to be polled
   * (in order given) when appending environmental data to
   * the state information given to the Learn function.
   */
  bool SetEnvironment(vector<Sensor* const> const& sensor_list);
};

#endif  // _SHL_PRIMITIVES_LEARNER_STANDARDQLEARNER_H_
