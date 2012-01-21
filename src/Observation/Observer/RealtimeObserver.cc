/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a standard Table-based QLearner
 */

#include "Observer/RealtimeObserver.h"
#include <vector>
#include <utility>
#include "Primitives/QLearner/QTable.h"
#include "Primitives/QLearner/State.h"

namespace Observation {

using std::pair;
using std::vector;
using std::string;
using Primitives::State;
using Primitives::QTable;

bool RealtimeObserver::Observe(Task* task) {
  if (sensors_.size() == 0) return false;
  if (primitives_.size() == 0) return false;

  is_observing_ =  true;

  vector<double> unified_frame;

  vector<ObservablePrimitive *> primitives;
  // Pair of timestamps and primitive pointers
  vector<pair<double,ObservablePrimitive *> > timed_out_primitives;
  
  // Setup observable primitives based on skills list
  vector<QLearner *>::iterator p_iter;
  for (p_iter = primitives_.begin(); p_iter != primitives_.end(); ++p_iter) {
    ObservablePrimitive *op = 
      new ObservablePrimitive((*p_iter)->get_name(), (*p_iter));
    primitives.push_back(op);
  }
  
  
  while (is_observing_) {
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    double cur_time_ms = (time.tv_sec * 1000.) +
                         (static_cast<double>(time.tv_nsec) / 1000000.);
      
    // Retrieve latest sensor data
    vector<Sensor*>::iterator sens_iter;
    for (sens_iter = sensors_.begin(); sens_iter != sensors_.end();
         ++sens_iter) {
      const double *values = (*sens_iter)->GetValues();
      for (int i = 0; i < (*sens_iter)->get_num_values(); ++i)
        unified_frame.push_back(values[i]);
    }
    
    vector<ObservablePrimitive *>::iterator p_iter;
    for (p_iter = primitives.begin(); p_iter != primitives.end();
         ++p_iter) {
      ObservablePrimitive *p = *p_iter;
      //QTable *qtable = p->q_learner->get_q_table();
      
      
      // @TODO Check for timeout status- if yes, then continue
      //                                 if no, check if should be released
      
      // @TODO Make sure the beginning of p's hit_states starts within the
      //       window of eligibility for it occurring.
      //       (cut out states beginning earlier than (now - p->duration)
      
      // Get current state from QTable with descriptor unified_frame
      State input_frame(unified_frame);
      State *current_state = p->q_learner->get_q_table()->GetState(
        input_frame, true);
        
      // If p has current_state set, get reward going from old to new state
      // if non-zero reward, then add state to hit_states for p
      State *prev_state = p->current_state;
      double transition_reward = 0.;
      if (prev_state != NULL)
        transition_reward = prev_state->GetRewardValue(current_state);
        
      if (transition_reward > 0.) {
        p->hit_states.push_back(pair<double,vector<double> >(
          cur_time_ms,unified_frame));
      }
      
      // If duration represented by hit_states is greater than 50% of the
      // anticipated duration of the primitive and number of frames in
      // hit_states between hit_states' start and now is less than 
      // 75% * number of frames elapsed between hit_states' start and now
      //  --> add p to the timeout pile
      double hit_state_duration = 0.;
      double first_hit_timestamp = 0.; 
      if (p->hit_states.size() > 1) {
        first_hit_timestamp = p->hit_states[0].first;
        hit_state_duration = p->hit_states[p->hit_states.size()-1].first
                            - first_hit_timestamp;
      }
      
      // @TODO: Magic numbers need documenting/turned into const variables
      if (hit_state_duration > .5 * p->duration_max_millis) {
          if (first_hit_timestamp > 0. 
              && p->hit_states.size() 
                 / (sampling_rate_ * (cur_time_ms - first_hit_timestamp))
                 < 0.75) {
            timed_out_primitives.push_back(
              pair<double,ObservablePrimitive*>(
                (cur_time_ms+p->duration_max_millis/3.),p));
            continue;
          }
      }
      
      // If current state is a trained goal state of p or near a trained goal
      // state of p:
      if (p->q_learner->isNearTrainedGoalState(*current_state, 1.)) {
      
      
        // Create a vector 'waypoints' of State* from within p's qlearner
        // sampling from p->hit_states. 
        //    For each entry, if rand() < .75, add State* to 'waypoints'.
        
        // Set p->QLearner's current_state to the first hit_state in the window
        // Add layer "waypoint" onto all transitions into States in 'waypoints' 
        // to artificially increase the reward for paths through that state
        
      
        // Calculate the max number of states to pass through before 
        // overshooting the max possible time for that primitive
        
        // Calculate A:
        // Over the time window from first used hit_state until 'now'
        // Calculate #hit frames / how many frames have elapsed, store in 'A'
        
        // Calculate B:
        // Follow a greedy path through the QLearner, summing the base_reward
        // values as you go (ignoring waypoint layer)
        // Store summed base_reward values in 'B'
        
        // Calculate C:
        // Follow a greedy path through the training data, beginning from the
        // initial state with closest distance to first_hit_state
        // Store summed base_reward values in 'C'
        
        // Calculate D:
        // Calculate duration elapsed by WP'd calculated trajectory, assuming
        //   one state per frame of input
    
        // Calculate E:
        // Calculate duration of expected time to completion
        
        // Calculate confidence score for label:
        // score = A * 0.2 + [B/C] * .7 + [D/E] * 0.1
        // For all frames covered from first_hit_state to now, apply label
        // pair: < score, p->name >
      }
    }
  } 

  return true;
}

bool RealtimeObserver::StopObserving() {
  is_observing_ = false;
  return true;
}


void RealtimeObserver::reset() {
  timeline_.clear();
}

/**
 * Returns a vector of strings consisting of the highest scoring labels
 * from the internal timeline
 * @return Timeline with most confident labels at time of calling
 **/
vector<string> RealtimeObserver::GetTimeline() {
  vector<string> best_timeline;
  vector<vector<pair<double, string> > >::iterator frame_iter;
  vector<pair<double, string> >::iterator label_iter;

  for (frame_iter = timeline_.begin(); frame_iter != timeline_.end();
       ++frame_iter) {
    vector<pair<double, string> > &labels = (*frame_iter);
    pair<double, string> *best_guess = NULL;
    for (label_iter = labels.begin(); label_iter != labels.end();
         ++label_iter) {
      if (best_guess == NULL) {
        best_guess = &(*label_iter);
      } else {
        if ((*label_iter).first > best_guess->first)
          best_guess = &(*label_iter);
      }
    }
    best_timeline.push_back(best_guess->second);
  }

  return best_timeline;
}


}  // namespace Observation
