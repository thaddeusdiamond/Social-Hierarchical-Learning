/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a simple dummy for Observer class
 **/

#ifndef _SHL_OBSERVATION_OBSERVER_DUMMY_H_
#define _SHL_OBSERVATION_OBSERVER_DUMMY_H_

#include <vector>
#include "Observer/Observer.h"

namespace Observation {

using std::vector;

class Dummy : public Observer {
 public:
  /**
   * The constructor for a Dummy is null
   **/
  Dummy() {}

  /**
   * The destructor for a Dummy is made virtual for polymorphism
   **/
  virtual ~Dummy() {}

  virtual bool Observe(Task* task, QTable* tables, int tables_len,
                       vector<Sensor*> const &sensors);
  virtual bool StopObserving(void);
  virtual double AttemptMatch(QTable const &table,
                              vector<Sensor*> const &sensors,
                              double min_threshold,
                              double death_time);
};

}  // namespace Observation

#endif  // _SHL_OBSERVATION_OBSERVER_DUMMY_H_
