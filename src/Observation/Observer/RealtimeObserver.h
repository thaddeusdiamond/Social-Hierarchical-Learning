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
#include <deque>
#include <string>
#include <utility>
#include "Observer/Observer.h"
#include "Observer/Task.h"
#include "Primitives/QLearner/QLearner.h"
#include "Primitives/Student/Sensor.h"
#include "Primitives/QLearner/State.h"

namespace Observation {

using std::vector;
using std::string;
using std::pair;
using std::deque;
using Primitives::QLearner;
using Primitives::Sensor;
using Primitives::State;

class RealtimeObserver : public Observer {
 public:
  RealtimeObserver(double sampling_rate_hz) : is_observing_(false),
      duration_(0.), sampling_rate_(sampling_rate_hz) {}

  bool Observe(Task* task, double duration);
  bool Observe(Task* task);
  bool StopObserving(void);
  void reset(void);

  /**
   * Returns a vector of strings consisting of the highest scoring labels
   * from the internal timeline
   * @return Timeline with most confident labels at time of calling
   **/
  vector<string> GetFinalTimeline(void);
  
  vector<vector<pair<double,string> > > &get_timeline() {
    return timeline_;
  }

  class ObservablePrimitive {
   public:
    ObservablePrimitive(string n, QLearner* qlearner)
      : name(n), q_learner(qlearner), current_state(NULL),
        goal_distance(1E10), strikes(0) { 
      hit_states.clear(); 
      duration_max_millis = qlearner->get_anticipated_duration();
    }

    // Each primitive gets a list of hit states: timestamp 
    // and the array index in frames_ containing the state vector    
    deque<pair<double, State*> > hit_states;
    string name;
    QLearner *q_learner;
    State *current_state;
    double goal_distance;
    int strikes;
    double duration_max_millis;
  };
  
 private:
  /**
   * Internal timeline that is reset each time "Observe" is called
   * Describes what is occurring during each frame of animation
   * 
   * Outer vector's elements represent each frame
   * Inner vector's elements represent <score, label> pairs
   **/
  vector<vector<pair<double, string> > > timeline_;

  /**
   * Internal representation of received frames
   **/
  vector<vector<double> > frames_;
  bool is_observing_;
  double duration_;
  double sampling_rate_;
};


}  // namespace Observation

#endif  // _SHL_OBSERVATION_OBSERVER_REALTIMEOBSERVER_H_
