/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Implementation
 */

#ifndef _SHL_PRIMITIVES_LEARNER_QLEARNER_H_
#define _SHL_PRIMITIVES_LEARNER_QLEARNER_H_

#include <string>
#include <vector>

class Object;
class CreditAssignmentType;
class ExplorationType;
class Sensor;

class QLearner {
 public:
  /**
   * The QLearner constructor initializes a blank QLearner with
   * default initializations for exploration and credit assignment
   * functions
   */
  explicit QLearner() {}

  /**
   * Destructor for QLearner must free all memory it received from I/O and
   * had buffered
   */
  virtual ~QLearner() {}

  /**
   * Populates this object with the QTable contained in the target file
   *
   * @param filename Path to file containing saved QTable
   *
   * @return True on success, false on failure.
   */
  bool Load(std::string &filename);
  
  /**
   * Saves the entire contents of the QTable to the target file
   *
   * @param filename  Destination file. Created if doesn't exist. Overwritten if it does.
   *
   * @return True on success, false on failure
   */
  bool Save(std::string &filename);

  /**
   * Clears table, initializes everything in the object to pristine and
   * usable state.
   *
   * @return    True on success, false on failure
   */
  bool Init();

  /**
   * Takes a vector of inputs and applies the given reward to the state.
   * 
   * @param state Vector of pointers to state descriptors
   * @param reward Value to use with set credit assignment function
   *
   * @return True on successful modification of QTable, false on failure.
   */
  bool Learn(std::vector<Object*> &state, double reward);
  
  /**
   * Populates given variables with a list of nearby state descriptors
   * and reward values currently associated with them.
   *
   * @param cur_state Vector of state descriptors from the state to be searched near.
   * @param nearby_states Empty vector of state descriptors to be populated by the time the function returns
   * @param nearby_rewards Empty vector of state rewards, indices matching the corresponding states in nearby_states
   *
   * @return True on success, false on lookup error.
   */
  bool GetNearbyStates(std::vector<Object*> &cur_state, std::vector<Object*> &nearby_states, std::vector<double> &nearby_rewards);

  
  /**
   * Returns the chosen next step by the QLearner.
   *
   * @param cur_state Vector of state descriptors
   * @param next_state Empty vector to be populated with state descriptors
   * @param next_reward Double to be overwritten with reward value for next_state
   */
  bool GetNextState(std::vector<Object*> &cur_state, 
                    std::vector<Object*> &next_state, double &next_reward);
  
  /**
   * Sets the credit assignment type used by this QLearner. Provided object
   * will get a pointer back to this object to allow it to use all 
   * possible information available for its decisions
   *
   * @param credit_assigner Instantiated credit assignment implementation
   *
   * @return True if initialized properly, false if error.
   */
  bool SetCreditFunction(CreditAssignmentType &credit_assigner);

  /**
   * Sets the exploration function used by this QLearner. Provided object
   * will get a pointer back to this object to allow it to use all 
   * possible information available for its decisions
   *
   * @param explorer Instantiated exploration function implementation
   *
   * @return True if initialized properly, false if error.
   */
  bool SetExplorationFunction(ExplorationType &explorer);

  /**
   * Writes the complete status log of this learner to the given file
   *
   * @param filename File to have status log dumped to
   *
   * @return True on successful finish, false if error
   */
  bool DumpStatusLog(std::string &filename);

  /**
   * Reset the status log for this QLearner
   *
   * @return True on success, false on fail
   */
  bool ClearStatusLog();
  
  
  /**
   * Copies Sensor pointers from provided list to be polled
   * (in order given) when appending environmental data to 
   * the state information given to the Learn function.
   */
  bool SetEnvironment(std::vector<Sensor*> &sensor_list);
};

#endif  // _SHL_PRIMITIVES_LEARNER_QLEARNER_H_
