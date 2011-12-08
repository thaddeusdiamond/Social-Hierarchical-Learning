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
#include "Primitives/QLearner/QTable.h"
#include "Primitives/Student/Sensor.h"

class Task;

using std::vector;

class Observer {
 public:
  /**
   * The destructor for an Observer is made virtual for polymorphism
   **/
  virtual ~Observer() {}

  /**
   * Every Observer must, at some point, begin the observation process
   *
   * @param     task            The task to be observed
   * @param     tables          The learned QTables to observe the task against
   * @param     tables_len      How many tables have been passed in as an array
   * @param     sensors         A vector of sensors to collect scene data from
   * 
   * @returns   True unless an error occurs during observation
   **/
  virtual bool Observe(Task* task, QTable* tables, int tables_len,
                       vector<Sensor*> const &sensors) = 0;

  /**
   * During the observation an observer will necessarily diverge the QTable
   * at some point to test if the current input matches a desired QTable
   *
   * @param     table           The table to be tested against
   * @param     sensors         A set of sensors to be polled over time & tested
   * @param     min_threshold   Minimum probability that should be allowed
   *                            before calling it quits
   * @param     death_time      The amount of time (in s) that the tester should
   *                            let the probability linger below min_threshold
   *                            before returning
   *
   * @returns   The confidence threshold of the tested QTable
   **/
  virtual double AttemptMatch(QTable const &table,
                              vector<Sensor*> const &sensors,
                              double min_threshold,
                              double death_time) = 0;

  /**
   * The Observer must be notified to stop the observation process (end of
   * tape or time)
   *
   * @returns   True unless an error stopping
   **/
  virtual bool StopObserving(void) = 0;

  /**
   * @todo There should probably be a set of sensors here but I wasn't sure
   *       if they should be environment, motors, sensors, etc...
   **/
};

#endif  // _SHL_OBSERVATION_OBSERVER_OBSERVER_H_
