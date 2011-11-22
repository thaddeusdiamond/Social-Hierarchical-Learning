/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Implementation
 **/

#ifndef _SHL_PRIMITIVES_LEARNER_QLEARNER_H_
#define _SHL_PRIMITIVES_LEARNER_QLEARNER_H_

#include <string>
#include <vector>
#include "Learner/State.h"

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
   * @return    True on success, false on failure
   **/
  virtual bool Init() = 0;

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
   * @param     cur_state       Vector of state descriptors 
   * @param     nearby_states   Empty vector of state descriptors to be
   *                            populated with const pointers from within this
   *                            object by the time the function returns
   *                            
   *
   * @return True on success, false on lookup error.
   **/
  virtual bool GetNearbyStates(State const& cur_state,
                               vector<State const* const>& nearby_states) = 0;


  /**
   * Returns the chosen next step by the QLearner.
   *
   * @param     cur_state       State descriptor
   * @param     next_state      Overwritten with pointer to next State object
   * 
   * @return True on success, false on lookup error
   **/
  virtual bool GetNextState(State const& cur_state,
                            State const* const next_state) = 0;

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
    CreditAssignmentType* const credit_assigner) = 0;

  /**
   * Sets the exploration function used by this QLearner. Provided object
   * will get a pointer back to this object to allow it to use all
   * possible information available for its decisions
   *
   * @param     explorer Instantiated exploration function implementation
   *
   * @return    True if initialized properly, false if error.
   **/
  virtual bool SetExplorationFunction(ExplorationType* const explorer) = 0;

  /**
   * Copies Sensor pointers from provided list to be polled
   * (in order given) when appending environmental data to
   * the state information given to the Learn function.
   **/
  virtual bool SetEnvironment(vector<Sensor* const> const& sensor_list) = 0;
};

#endif  // _SHL_PRIMITIVES_LEARNER_QLEARNER_H_
