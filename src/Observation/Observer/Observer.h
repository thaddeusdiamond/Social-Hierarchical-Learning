/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for any observation mechanism (performing
 * task recognition or LfD)
 **/

#ifndef _SHL_OBSERVATION_OBSERVER_OBSERVER_H_
#define _SHL_OBSERVATION_OBSERVER_OBSERVER_H_

#include <vector>
#include "Primitives/QLearner/QLearner.h"
#include "Primitives/Student/Sensor.h"

namespace Observation {

class Task;

using std::vector;
using Primitives::QLearner;
using Primitives::Sensor;

class Observer {
 public:
  /**
   * The destructor for an Observer is made virtual for polymorphism
   **/
  virtual ~Observer() {}

  /**
   * Begins the observation process, monitoring the internally kept sensors
   * and matching primitives as they appear in the input sequence.
   *
   * @param     task (optional) The task to be observed and monitored
   *
   * @returns   True if observation mode is engaged, false on error
   **/
  virtual bool Observe(Task* task) = 0;

  /**
   * The Observer must be notified to stop the observation process (end of
   * tape or time)
   *
   * @returns   True unless an error stopping
   **/
  virtual bool StopObserving(void) = 0;

  /**
   * Initialize the observer with the set of sensors it will be using
   * @param sensors Vector of sensor pointers the be polled by observer
   */
  void Init(std::vector<Sensor *> sensors) {
    sensors_ = sensors;
  }

  /**
   * Add a skill into the library of primitives to identify
   * @param skill
   */
  void AddSkill(QLearner * skill) { primitives_.push_back(skill); }

  std::vector<Sensor *> const & get_sensors() { return sensors_; }
  std::vector<QLearner *> const & get_primitives() { return primitives_; }

 private:
  std::vector<Sensor *> sensors_;
  std::vector<QLearner *> primitives_;
};

}  // namespace Observation

#endif  // _SHL_OBSERVATION_OBSERVER_OBSERVER_H_
