/**
 * @file
 * @author Brad Hayes <bradley.h.hayes@yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Implementation of a realtime primtive recognition observer
 * for the AAAI 2012 and IROS 2012 conference papers.
 **/

#ifndef _SHL_OBSERVATION_OBSERVER_REALTIMEOBSERVER_H_
#define _SHL_OBSERVATION_OBSERVER_REALTIMEOBSERVER_H_


#include <vector>
#include <string>
#include <utility>
#include "Observer/Observer.h"
#include "Observer/Task.h"
#include "Primitives/QLearner/QLearner.h"
#include "Primitives/Student/Sensor.h"

namespace Observation {

using std::vector;
using std::string;
using std::pair;

class RealtimeObserver : public Observer {
 public:
  RealtimeObserver() : is_observing_(false) {}

  bool Observe(Task* task);
  bool StopObserving(void);
  void reset(void);

  /**
   * Returns a vector of strings consisting of the highest scoring labels
   * from the internal timeline
   * @return Timeline with most confident labels at time of calling
   **/
  vector<string> GetTimeline(void);

 private:
  /**
   * Internal timeline that is reset each time "Observe" is called
   * Describes what is occurring during each frame of animation
   **/
  vector<vector<pair<double, string> > > timeline_;

  bool is_observing_;
};


}  // namespace Observation

#endif  // _SHL_OBSERVATION_OBSERVER_REALTIMEOBSERVER_H_
