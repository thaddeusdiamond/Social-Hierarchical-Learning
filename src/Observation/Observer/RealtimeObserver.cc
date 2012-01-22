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
#include <stdlib.h>
#include <vector>
#include <deque>
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
  
  timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  double cur_time_ms = (time.tv_sec * 1000.) + 
                       (static_cast<double>(time.tv_nsec) / 1000000.);
  
  int cur_frame = 0;
  double start_time = cur_time_ms;
  
  while (is_observing_) {
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
    
    frames_.push_back(unified_frame); // Store incoming frame data
    vector<pair<double, string> > timeline_entry;
    timeline_entry.push_back(pair<double,string>(0.5,"Unknown"));
    timeline_.push_back(timeline_entry); // Add default guess
    
    vector<ObservablePrimitive *>::iterator p_iter;
    for (p_iter = primitives.begin(); p_iter != primitives.end();
         ++p_iter) {
      ObservablePrimitive *p = *p_iter;
      // QTable *qtable = p->q_learner->get_q_table();
      
      
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
      if (prev_state != NULL) {
        transition_reward = prev_state->GetRewardValue(current_state);
      } else {
        p->current_state = current_state;
        continue;
      }
      
      if (transition_reward > 0.) {
        p->hit_states.push_back(pair<double,State*>(
          cur_time_ms,current_state));
      } else {
        prev_state->set_reward(current_state,"base",-0.1);
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
      
      
      // If hit_state_duration is greater than acceptable duration, trim the start
      while (hit_state_duration > p->duration_max_millis) {
        p->hit_states.pop_front();        
        
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
      if (p->q_learner->IsNearTrainedGoalState(*current_state, 1.)) {
      
      
        // Create a vector 'waypoints' of State* from within p's qlearner
        // sampling from p->hit_states. 
        //    For each entry, if rand() < .75, add State* to 'waypoints'.
        vector<State*> waypoints;
        vector<State *>::iterator waypoint_iter;
        deque<pair<double, State*> >::iterator hit_iter;
        waypoints.push_back(p->hit_states[0].second);
        for (hit_iter = p->hit_states.begin()+1; 
             hit_iter != p->hit_states.end();
             ++hit_iter) {
          int r_val = rand() % 100;
          
          if (r_val < 75)
            waypoints.push_back(hit_iter->second);
        }                
        
        
        // Set p->QLearner's current_state to the first hit_state in the window
        
        // Add layer "waypoint" onto all transitions into States in 'waypoints' 
        // to artificially increase the reward for paths through that state
        for (waypoint_iter = waypoints.begin(); 
             waypoint_iter != waypoints.end();
             ++waypoint_iter) {
          State *s = *waypoint_iter;
          vector<State *> &incoming_states = s->get_incoming_states();
          vector<State *>::iterator inc_iter;
          for (inc_iter = incoming_states.begin();
               inc_iter != incoming_states.end();
               ++inc_iter) {
            State *inc_state = *inc_iter;
            if (inc_state != s)
              inc_state->set_reward(s,"waypoint",1000.);
          }
          
        }
        
        // Calculate the max number of states to pass through before 
        // overshooting the max possible time for that primitive
        double max_state_transitions = sampling_rate_ 
          * p->q_learner->get_anticipated_duration() * 1.25;
        
        double target_state_transitions = sampling_rate_ * hit_state_duration;
        
        // Calculate A:
        // Over the time window from first used hit_state until 'now'
        // Calculate #hit frames / how many frames have elapsed, store in 'A'
        double match_score_a = p->hit_states.size() 
          / ((cur_time_ms - first_hit_timestamp) * sampling_rate_);
        
        // Calculate B:
        // Follow a greedy path through the QLearner, summing the base reward
        // values as you go (ignoring waypoint layer)
        // Store summed base_reward values in 'B'
        double match_score_b = 0.;
        vector<State *> waypointed_path;
        vector<State *>::iterator waypointed_path_iter;
        State *wp_path_state = p->hit_states[0].second; // start at first hit
        State *wp_path_next_state = NULL;
        double temp_reward = 0.;
        int states_traversed = 0;

        waypointed_path.push_back(wp_path_state);
        while (states_traversed < max_state_transitions
               && states_traversed < target_state_transitions) {
          bool success = p->q_learner->GetNextState(*wp_path_state,
                                                    &wp_path_next_state,
                                                    temp_reward);
          if (!success) {
            // Shouldn't run into this case... maybe errorlog message here
            break;
          }
          
          
          wp_path_state = wp_path_next_state;
          waypointed_path.push_back(wp_path_state);
          ++states_traversed;
        }
        
        double wp_path_score = 0.;
        wp_path_state = *(waypointed_path.begin());
        for (waypointed_path_iter = waypointed_path.begin()+1;
             waypointed_path_iter != waypointed_path.end();
             ++waypointed_path_iter) {
          State *next = (*waypointed_path_iter);
          
          wp_path_score += wp_path_state->GetRewardValue(next,false,"base");
        }
        
        match_score_b = wp_path_score 
                        / static_cast<double>(waypointed_path.size());
        

        // Remove layer "waypoint" from transitions into States in 'waypoints' 
        for (waypoint_iter = waypoints.begin(); 
             waypoint_iter != waypoints.end();
             ++waypoint_iter) {
          State *s = *waypoint_iter;
          vector<State *> &incoming_states = s->get_incoming_states();
          vector<State *>::iterator inc_iter;
          for (inc_iter = incoming_states.begin();
               inc_iter != incoming_states.end();
               ++inc_iter) {
            State *inc_state = *inc_iter;
            inc_state->set_reward(s,"waypoint",0.);
          }
          
        }
        
        
        
        // Calculate C:
        // Follow a greedy path through the training data, beginning from the
        // initial state with closest distance to first_hit_state
        // Store summed base_reward values in 'C'
        double match_score_c = 0.;
        vector<State *> optimal_path;
        State *optimal_path_state;
        State *optimal_path_next_state;
        states_traversed = 0;
        double optimal_path_score = 0.;
        
        optimal_path_state = p->hit_states[0].second;
        optimal_path.push_back(optimal_path_state);
        while (states_traversed < max_state_transitions) {
          bool success = p->q_learner->GetNextState(*optimal_path_state,
                                                    &optimal_path_next_state,
                                                    temp_reward);
          if (!success) {
            optimal_path_score = -1.; 
            break;
          }
          optimal_path_state = optimal_path_next_state;
          optimal_path.push_back(optimal_path_state);
          

          optimal_path_score += optimal_path_state->GetRewardValue(
                                  optimal_path_next_state,false,"base");
          ++states_traversed;
        }
        
        match_score_c = optimal_path_score 
                        / static_cast<double>(optimal_path.size());
        
        
        // Calculate D:
        // Calculate duration elapsed by WP'd calculated trajectory, assuming
        //   one state per frame of input
        double match_score_d = sampling_rate_ 
          * static_cast<double>(optimal_path.size());
        
        // Calculate E:
        // Calculate duration of expected time to completion
        double match_score_e = p->q_learner->get_anticipated_duration();
        
        
        // Calculate F:
        // Calculate best possible per-state score for gesture
        double match_score_f = 100.;
        
        
        // Calculate confidence score for label:
        // score = A * 0.2 + [B/C] * .7 + [D/E] * 0.1
        double final_score = match_score_a * 0.25
                             + (match_score_b/match_score_c) * 0.4
                             + (match_score_d / match_score_e) * 0.1
                             + (match_score_b / match_score_f) * 0.25;
        
        
        // For all frames covered from first_hit_timestamp to now, apply label
        // pair: < score, p->name >
        
        // Figure out which frame to start with: offset from start * frames/sec
        int frame_start = static_cast<int>((first_hit_timestamp - start_time)
                                            * sampling_rate_);
        int frame_end = cur_frame;
        pair<double, string> label(final_score, p->name);
        for (int i=frame_start; i <= frame_end; ++i) {
          timeline_[i].push_back(label);
        }
        
      }
    }
    ++cur_frame;
  } 

  
  //TODO: Free up memory
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
vector<string> RealtimeObserver::GetFinalTimeline() {
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
