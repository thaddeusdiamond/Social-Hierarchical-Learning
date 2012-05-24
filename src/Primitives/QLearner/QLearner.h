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
#include "QLearner/Object.h"
#include "Common/Utils.h"


namespace Primitives {

using std::string;
using std::vector;
using Utils::Log;
class ExplorationType;

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
  virtual bool Init(std::vector<Sensor *> const &sensors) = 0;


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
   * @param     reward          Reward value for transitioning into next state
   *
   * @return True on success, false on lookup error
   **/
  virtual bool GetNextState(State *cur_state,
                            State **next_state,
                            double &reward) = 0;

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
    CreditAssignmentType* credit_assigner) {
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
  virtual bool SetExplorationFunction(ExplorationType* explorer) {
    exploration_type_ = explorer;
    return (exploration_type_ != NULL);
  }

  /**
   * Copies Sensor pointers from provided list to be polled
   * (in order given) when appending environmental data to
   * the state information given to the Learn function.
   **/
  virtual bool SetEnvironment(vector<Sensor*> const& sensor_list) {
    sensors_ = sensor_list;
    /*
    std::vector<Sensor*>::const_iterator iter;
    for (iter = sensor_list.begin(); iter != sensor_list.end(); ++iter) {
      Sensor *s = *iter;
      sensors_.push_back(s);
    }*/
    return true;
  }

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
   * Returns a vector of states resembling an optimal traversal
   * through the QTable for executing the action, 
   * resulting in a fixed action pattern. 
   * 
   * For actions that respond to stimuli actively,
   * you want to do the traversal step by step, 
   * instead of getting full playback in advance.
   * 
   * @param current_state Current system state, used to find the nearest
   *                      initiation state to start from.
   * 
   * @return Vector of states that must be traversed to execute the fixed
   *         action pattern
   **/
  virtual vector<State *> GetNearestFixedExecutionPath(
                                              State *current_state) = 0;

  /**
   * Checks if the state provided is near a goal state
   *
   * @param state Any state object
   * @param sensitivity Measure of how close to goal state is 'close enough'
   *                    This is currently used as a multiple of state unit
   *                    distance (sum over min. sensor change values in
   *                    state vector).
   * @param distance Distance from nearest goal expressed as percentage of
   *                 nearby thresholds, summed for each sensor value
   * @return true if found in list, false if not a goal state
   */
  virtual bool IsNearTrainedGoalState(State const &state, double sensitivity,
                                      double &best_distance) {
    std::vector<State *>::iterator state_iter;
    std::vector<State *> &goal_states = q_table_.get_trained_goal_states();

    best_distance = 1E10;

    // Iterate through all candidate goal states...
    for (state_iter = goal_states.begin(); state_iter != goal_states.end();
      ++state_iter) {
      bool state_fail = false;
      std::vector<double> dists = state.GetSquaredDistances(*state_iter);

      vector<double> const &nearby_thresholds =
        q_table_.get_nearby_thresholds();

      double tmp_distance = 0.;

      // Check each sensor value distance for violation of near-ness
      for (unsigned int idx = 0;
           idx < dists.size() && idx < nearby_thresholds.size();
           ++idx) {
        double sensor_unit_dist = nearby_thresholds[idx];
        double distance = dists[idx];

        tmp_distance += distance / sensor_unit_dist;
        if (distance > sensor_unit_dist * sensitivity) {
          // Uncomment to get debugging info about distance threshold checks
          // double dist_val = distance - (sensor_unit_dist * sensitivity);
          // char buf[1024];
          // snprintf(buf,1024,"State too far from goal by %g.",dist_val);
          // Log(stderr,DEBUG,buf);
          state_fail = true;
        }
      }

      tmp_distance /= dists.size();
      if (tmp_distance < best_distance) best_distance = tmp_distance;
      if (!state_fail) return true;
    }
    return false;
  }

  /**
   * Returns a stack of recently visited states
   *
   * @return Stack of StateHistoryTuple
   **/
  virtual std::stack<StateHistoryTuple> &get_state_history() {
    return state_history_;
  }

  virtual State *get_current_state() {
    if (state_history_.size())
      return state_history_.top().state_;
    return NULL;
  }

  virtual QTable *get_q_table() { return &q_table_; }

  virtual std::string get_name() { return name_; }
  virtual void set_name(std::string name) { name_ = name; }

  virtual int get_trials() { return trials_; }
  virtual void set_trials(int trials) { trials_ = trials; }

  virtual double get_anticipated_duration() { return anticipated_duration_; }
  virtual void set_anticipated_duration(int anticipated_duration) {
    anticipated_duration_ = anticipated_duration;
  }

 protected:
  std::stack<StateHistoryTuple> state_history_;
  QTable q_table_;
  int trials_;
  double anticipated_duration_;

  std::string name_;
  CreditAssignmentType *credit_assignment_type_;
  ExplorationType *exploration_type_;
  std::vector<Sensor *> sensors_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_QLEARNER_H_
