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
#include "Common/Utils.h"
namespace Observation {

using std::pair;
using std::vector;
using std::string;
using namespace std;
using Primitives::State;
using Primitives::QTable;
using Utils::Log;

bool RealtimeObserver::Observe(Task* task, double duration) {
    duration_ = duration;
    return Observe(task);
}
  
bool RealtimeObserver::Observe(Task* task) {
  FILE *log_stream = stderr;
  double LEARNING_RATE = 0.2;
  double DISCOUNT_FACTOR = 0.9;
  
  if (sensors_.size() == 0) {
    Log(log_stream, DEBUG, (string("No sensors defined on observer.")).c_str());
    return false; 
  }
  if (primitives_.size() == 0) {
    Log(log_stream, DEBUG, (string("No primitives defined on observer.").c_str())); 
    return false; 
  }

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
  double end_time = start_time + duration_;
  double last_frame_time_ms = 0.;
  
  timeline_.clear();
  frames_.clear();
  
  while (is_observing_ && cur_time_ms < end_time) {
    unified_frame.clear();
    bool clear_hit_states = false;
    
    while (sampling_rate_ > cur_time_ms - last_frame_time_ms) {
      clock_gettime(CLOCK_REALTIME, &time);
      cur_time_ms = (time.tv_sec * 1000.) +
                           (static_cast<double>(time.tv_nsec) / 1000000.);
    }
    last_frame_time_ms = cur_time_ms;
  
    // Retrieve latest sensor data
    //char buf[1024];
    //snprintf(buf,1024,"Capturing Frame %d with %ld sensors...",cur_frame,
    //          sensors_.size());
    //Log(log_stream, DEBUG, buf); 
    
    unsigned int sens_iter;
    for (sens_iter = 0; sens_iter < sensors_.size();
         ++sens_iter) {
      Sensor *sensor = sensors_[sens_iter];

      const double *values = sensor->GetValues();
      // snprintf(buf,1024,"... contains %d values ...",
      //    sensor->get_num_values());
      // Log(log_stream,DEBUG,buf);
      for (int i = 0; i < sensor->get_num_values(); ++i) {
        double rand_factor = double(rand() % 6) / 100. + 0.97;
        unified_frame.push_back(values[i] * rand_factor);
      }
    }

    //snprintf(buf,1024,"...inc frame has size %ld", unified_frame.size());
    //Log(log_stream,DEBUG,buf);
    
    frames_.push_back(unified_frame); // Store incoming frame data
    vector<pair<double, string> > timeline_entry;
    timeline_entry.push_back(pair<double,string>(0.3,"Unknown"));
    timeline_.push_back(timeline_entry); // Add default guess

    //Log(log_stream, DEBUG, 
    //  string("Done capturing frame. Beginning primitive loop").c_str()); 

    
    vector<ObservablePrimitive *>::iterator p_iter;
    for (p_iter = primitives.begin(); p_iter != primitives.end();
         ++p_iter) {
      
      ObservablePrimitive *p = *p_iter;
      
      /*char buf[1024];
       snprintf(buf,1024,"Analyzing primitive %s with state count %ld",
        p->name.c_str(), p->q_learner->get_q_table()->get_states().size());
      Log(log_stream, DEBUG, buf); */
      
      
      // QTable *qtable = p->q_learner->get_q_table();
      
      
      // @TODO Check for timeout status- if yes, then continue
      //                                 if no, check if should be released
      
      // @TODO Make sure the beginning of p's hit_states starts within the
      //       window of eligibility for it occurring.
      //       (cut out states beginning earlier than (now - p->duration)
      
      // Get current state from QTable with descriptor unified_frame
      State input_frame(unified_frame);
      // Log(log_stream,DEBUG, input_frame.to_string().c_str());
      
      State *current_state = p->q_learner->get_q_table()->GetState(
        input_frame, true);
      
      /*char buf[1024];
      snprintf(buf,1024,"Current_state has descriptorsize %ld",
        current_state->get_state_vector().size());
      Log(log_stream,DEBUG,buf);*/
      
      if (!current_state) {
        Log(log_stream,ERROR,"QTable failed to add state");
      }
        
      // If p has current_state set, get reward going from old to new state
      // if non-zero reward, then add state to hit_states for p
      State *prev_state = p->current_state;
      
      if (prev_state == current_state) {
  //      Log(stderr,DEBUG,"Duplicate frame.");
//        continue; 
      }

      p->current_state = current_state;

      
      double transition_reward = 0.;
      if (prev_state != NULL) {
        transition_reward = prev_state->GetRewardValue(current_state,true,"");
        char buf[1024];
        snprintf(buf,1024,"...State transition %d has reward %g. Prev_state has %ld"
                " outbound connections. Descriptor size %ld", cur_frame, transition_reward, 
                prev_state->get_reward().size(),
                prev_state->get_state_vector().size());
        //Log(log_stream,DEBUG,buf); 
        /*Log(log_stream,DEBUG,string(
          prev_state->to_string() + " to " + current_state->to_string()
          ).c_str());*/
        
      } else {
        continue;
      }
      
      if (transition_reward > 0.) {
        p->hit_states.push_back(pair<double,State*>(
          cur_time_ms,current_state));
      } else {
        // Log(log_stream,ERROR,"...Zero reward transition on training data..?");
        prev_state->set_reward(current_state,"base",-1);
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
      while (hit_state_duration > p->duration_max_millis * 1.5) {
        p->hit_states.pop_front();        
        
        first_hit_timestamp = p->hit_states[0].first;
        hit_state_duration = p->hit_states[p->hit_states.size()-1].first
                             - first_hit_timestamp;
      }
      
      
      // @TODO: Magic numbers need documenting/turned into const variables
      if (hit_state_duration > .5 * p->duration_max_millis) {
          if (first_hit_timestamp > 0. 
              && p->hit_states.size() 
                 / ((cur_time_ms - first_hit_timestamp) / sampling_rate_)
                 < 0.5) {
            //timed_out_primitives.push_back(
            //  pair<double,ObservablePrimitive*>(
            //    (cur_time_ms+p->duration_max_millis/3.),p));
            char buf[1024];
            snprintf(buf,1024,"...Hit window too inaccurate (%g%%)",
              (p->hit_states.size() 
                 / ((cur_time_ms - first_hit_timestamp) / sampling_rate_)));
            //Log(log_stream,ERROR,buf);
            continue;
          }
      } else { 
        // Not close enough to the required duration to actually consider this skill yet
        
        char buf[1024];
        snprintf(buf,1024,"...Hit window too small %g (%g%%) to check for goal",
          hit_state_duration, (hit_state_duration / p->duration_max_millis));
        //Log(log_stream,ERROR,buf);
        continue;
      }
      
      // If current state is a trained goal state of p or near a trained goal
      // state of p:
      //Log(log_stream,DEBUG,"...Checking for goal!");
      double distance_to_goal = 0.;
      if (p->q_learner->IsNearTrainedGoalState(*current_state, 5.,
                                               distance_to_goal)) {
      //Log(log_stream, DEBUG, 
      //  string("ZOMG Near goal state for " + p->name).c_str()); 

        double anticipated_frames_elapsed = (
          (cur_time_ms - first_hit_timestamp) / sampling_rate_);

        // Calculate the max number of states to pass through before 
        // overshooting the max possible time for that primitive
        double max_state_transitions = sampling_rate_ 
          * p->q_learner->get_anticipated_duration() * 1.5;
        
        double target_state_transitions = hit_state_duration / sampling_rate_;

        double temp_reward = 0.;
        int states_traversed = 0;
  
      
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
          
          if (r_val < 60)
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
          if (incoming_states.size() == 0) continue;
          vector<State *>::iterator inc_iter;
          for (inc_iter = incoming_states.begin();
               inc_iter != incoming_states.end();
               ++inc_iter) {
            State *inc_state = *inc_iter;
            if (!inc_state)
              Log(stderr,ERROR,"NULL State to be WP'd?");
            if (inc_state != s)
              inc_state->set_reward(s,string("waypoint"),45.);
          }
          
        }

        // Calculate C:
        // Follow a greedy path through the training data,
        // only loosely following the received data. This can be thought
        // of as an "optimized" path, given the data seen.
        // Store summed base_reward values in 'C'
        double match_score_c = 0.;
        vector<State *> optimal_path;
        State *optimal_path_state;
        State *optimal_path_next_state = NULL;
        states_traversed = 0;
        double optimal_path_score = 0.;
        
        optimal_path_state = p->hit_states[0].second;
        optimal_path.push_back(optimal_path_state);
        while (states_traversed < anticipated_frames_elapsed) {
          temp_reward = 0.;
          bool success = p->q_learner->GetNextState(optimal_path_state,
                                                    &optimal_path_next_state,
                                                    temp_reward);                                                    
                                                    
          if (!success) {
            char buf[1024];
            snprintf(buf,1024,"Failed optimized state traversal on primitive %s"
              " after %ld steps",
              p->name.c_str(), optimal_path.size());
            Log(stderr,ERROR,buf);
            break;
          }
          
          double transition_reward = optimal_path_state->GetRewardValue(
                                  optimal_path_next_state,false,"base");
          
          double best_transition_from_next_state = 0.;
          map<State*, map<string, double> > const &next_state_rewards = optimal_path_next_state->get_reward();
          map<State*, std::map<std::string, double> >::const_iterator future_reward;
          for (future_reward = next_state_rewards.begin();
               future_reward != next_state_rewards.end();
               ++future_reward) {
                double reward = optimal_path_next_state->GetRewardValue(future_reward->first, false, "base");
                if (reward > best_transition_from_next_state)
                  best_transition_from_next_state = reward;
          }
          // Transition update rule
          vector<State *> &inc_states = optimal_path_state->get_incoming_states(); 
          for (unsigned int i=0;i<inc_states.size();++i) {
              State *inc_state = inc_states[i];
              double reward_to_cur_state = inc_state->GetRewardValue(optimal_path_state,false,"base");
              reward_to_cur_state = (1.-LEARNING_RATE)*reward_to_cur_state 
                + LEARNING_RATE * ( transition_reward  + DISCOUNT_FACTOR*best_transition_from_next_state - reward_to_cur_state);
              inc_state->set_reward(optimal_path_state,string("base"),reward_to_cur_state);
          }
                                  
          optimal_path_score += transition_reward;

          optimal_path_state = optimal_path_next_state;
          optimal_path.push_back(optimal_path_state);
                                  
          optimal_path_next_state = NULL;
          ++states_traversed;

          char buf[1024];
          snprintf(buf,1024,"Chose transition with value %g, real value %g",
            temp_reward, transition_reward);
          //Log(stderr,DEBUG,buf);

          
          double temp_dbl = 0.;
          if (states_traversed > anticipated_frames_elapsed/4. &&
              p->q_learner->IsNearTrainedGoalState(*optimal_path_state, .25,
                                               temp_dbl)) {
            break;
          } 
          
        }
        
        if (optimal_path.size() == 0) continue;
        match_score_c = (optimal_path_score)
                        / static_cast<double>(optimal_path.size()-1);


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

        // Create a vector 'waypoints' of State* from within p's qlearner
        // sampling from p->hit_states. 
        //    For each entry, if rand() < .75, add State* to 'waypoints'.
        waypoints.clear();
        for (hit_iter = p->hit_states.begin(); 
             hit_iter != p->hit_states.end();
             ++hit_iter) {
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
          if (incoming_states.size() == 0) continue;
          vector<State *>::iterator inc_iter;
          for (inc_iter = incoming_states.begin();
               inc_iter != incoming_states.end();
               ++inc_iter) {
            State *inc_state = *inc_iter;
            if (!inc_state)
              Log(stderr,ERROR,"NULL State to be WP'd?");
            if (inc_state != s)
              inc_state->set_reward(s,string("waypoint"),100.);
          }
          
        }

        // Calculate B:
        // Follow a greedy path through the QLearner, summing the base reward
        // values as you go (ignoring waypoint layer). This is thought of
        // as the "actual" path through the state space that was observed
        // Store summed base_reward values in 'B'
        double match_score_b = 0.;
        vector<State *> waypointed_path;
        vector<State *>::iterator waypointed_path_iter;
        State *wp_path_state = p->hit_states[0].second; // start at first hit
        State *wp_path_next_state = NULL;
        states_traversed = 0.;
        temp_reward = 0.;
        waypointed_path.push_back(wp_path_state);
        while (states_traversed < max_state_transitions
               && states_traversed < target_state_transitions) {
          bool success = p->q_learner->GetNextState(wp_path_state,
                                                    &wp_path_next_state,
                                                    temp_reward);
          if (!success) {
            // Shouldn't run into this case... maybe errorlog message here
            char buf[1024];
            snprintf(buf,1024,"Failed state traversal on primitive %s"
                " after %ld steps.", p->name.c_str(), waypointed_path.size());
            Log(stderr,ERROR,buf);
            break;
          }
          
          
          wp_path_state = wp_path_next_state;
          waypointed_path.push_back(wp_path_state);
          wp_path_next_state = NULL;
          ++states_traversed;
          
          /*double temp_dbl = 0.;
          if (p->q_learner->IsNearTrainedGoalState(*wp_path_state, 1.,
                                               temp_dbl)) {
            break;
          }*/
        }
        
        double wp_path_score = 0.;
        wp_path_state = *(waypointed_path.begin());
        for (waypointed_path_iter = waypointed_path.begin()+1;
             waypointed_path_iter != waypointed_path.end();
             ++waypointed_path_iter) {
          State *next = (*waypointed_path_iter);
          
          wp_path_score += wp_path_state->GetRewardValue(next,false,"base");
          wp_path_state = next;
        }
        
        if (waypointed_path.size() == 0) continue;
        match_score_b = (wp_path_score)
                        / static_cast<double>(waypointed_path.size()-1);
        

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
        

        

/*        
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
        while (states_traversed < anticipated_frames_elapsed) {
          bool success = p->q_learner->GetNextState(optimal_path_state,
                                                    &optimal_path_next_state,
                                                    temp_reward);
          if (!success) {
            // Shouldn't run into this case... maybe errorlog message here
            char buf[1024];
            snprintf(buf,1024,"Failed optimist state traversal on primitive %s"
              " after %ld steps",
              p->name.c_str(), optimal_path.size());
            Log(stderr,ERROR,buf);
            break;
          }
          
          optimal_path_score += optimal_path_state->GetRewardValue(
                                  optimal_path_next_state,false,"base");

          optimal_path_state = optimal_path_next_state;
          optimal_path.push_back(optimal_path_state);
          

                                  
          optimal_path_next_state = NULL;
          ++states_traversed;
          
          double temp_dbl = 0.;
          if (states_traversed > target_state_transitions*0.25
            && p->q_learner->IsNearTrainedGoalState(*optimal_path_state, .25,
                                               temp_dbl)) {
            break;
          } 
          
        }
        
        if (optimal_path.size() == 0) continue;
        match_score_c = (optimal_path_score / 100.)
                        / static_cast<double>(optimal_path.size());
*/
        
        // Calculate A:
        // Over the time window covered by the waypointed path
        // Calculate # good states / how many frames have elapsed, store in 'A'
        double match_score_a = 1 - 
          (fabs(waypointed_path.size() - anticipated_frames_elapsed))
          / anticipated_frames_elapsed;

          
        
        // Calculate D:
        // Calculate duration elapsed by WP'd calculated trajectory, assuming
        //   one state per frame of input
        double match_score_d = sampling_rate_ 
          * static_cast<double>(optimal_path.size());
        
        // Calculate E:
        // Calculate duration of expected time to completion
        double match_score_e = p->q_learner->get_anticipated_duration();
        
        double match_distance_d_e = 1 
            - (fabs(match_score_d - match_score_e) / match_score_e);
        
        // Calculate F:
        // Calculate best possible per-state score for gesture
        double match_score_f = 100.;
        
        // Calculate confidence score for label:
        // score = A * 0.2 + [B/C] * .7 + [D/E] * 0.1
        double final_score = 0.;

        if (use_waypointing_) {
          final_score =  match_score_a * 0.25;
          final_score += (match_score_b/100.) * 0.40;
          if (match_score_b/match_score_c > 1.)
            final_score += 0.2;
          else
            final_score += (match_score_b/match_score_c) * 0.20;
            final_score += match_distance_d_e  * 0.15;
        
          if (match_score_a < 0.75) final_score *= 0.8;
          if (match_score_b < 75.) final_score *= 0.8;
          if (match_score_c < 75.) final_score *= 0.8;
          if (match_score_b - 10. > match_score_c) final_score *= 0.8;
          if (match_distance_d_e < .80) final_score *= 0.8;
        } else {
          final_score =  match_score_a * 0.25
                           + (match_score_b/100.) * 0.60
                           + match_distance_d_e  * 0.15;
        
          if (match_score_a < .75) final_score *= 0.8;
          if (match_score_b < 75.) final_score *= 0.8;
          if (match_distance_d_e < .80) final_score *= 0.8;          
        }
        
        // For all frames covered from first_hit_timestamp to now, apply label
        // pair: < score, p->name >
        
        // Figure out which frame to start with: offset from start / (frames/sec)
        int frame_start = static_cast<int>((first_hit_timestamp - start_time)
                                            / sampling_rate_);
        int frame_end = cur_frame;
        
        char scorebuf[4096];
        snprintf(scorebuf,4096,"Assigning Label: (%s, %g) from frame %d to %d"
        " scores: %g, %g, %g, %g, %g, %g, %g",
          p->name.c_str(), final_score, frame_start, frame_end,
          match_score_a, match_score_b, match_score_c, match_score_d,
          match_score_e,
          match_distance_d_e,
          match_score_f);
        Log(stderr,DEBUG,scorebuf);
        
        pair<double, string> label(final_score, p->name);
        for (int i=frame_start; i <= frame_end; ++i) {
          timeline_[i].push_back(label);
        }
        
        if (final_score > 90) { 
          clear_hit_states = false; 
          for (unsigned int hidx = 0; hidx < p->hit_states.size();++hidx) {
            State *s  = p->hit_states[hidx].second;
            // Transition update rule
            vector<State *> &inc_states = s->get_incoming_states(); 
            for (unsigned int i=0;i<inc_states.size();++i) {
                State *inc_state = inc_states[i];
                double reward_to_cur_state = inc_state->GetRewardValue(optimal_path_state,false,"base");
                reward_to_cur_state *= (1.+LEARNING_RATE);
                inc_state->set_reward(s,string("base"),reward_to_cur_state);
            }
          }
          
          
          unsigned int h_sz = p->hit_states.size() / 2;
          while (--h_sz > 0) 
            p->hit_states.pop_front();  
        }
      }
    }
    
    if (clear_hit_states) {
      for (p_iter = primitives.begin(); p_iter != primitives.end();
         ++p_iter) {
        ObservablePrimitive *p = *p_iter;
        p->hit_states.clear();
      }
    } 
    
    ++cur_frame;
  } 

  vector<ObservablePrimitive *>::iterator op_iter;
  for (op_iter = primitives.begin(); op_iter != primitives.end();
     ++op_iter) {
    ObservablePrimitive *p = *op_iter;
    delete p;
    p = NULL;
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


/**
 * Returns a frame-by-frame mapping of primitive name -> confidence score
 * 
 **/
vector<map<string,double> > RealtimeObserver::GetPrimitivePerformanceTimeline(void) {
//  vector<vector<pair<double, string> > > timeline_;
  vector<map<string, double> > result;
  
  for (unsigned int i = 0; i < timeline_.size(); ++i) {
    map<string, double> frame_result;
    for (unsigned int j = 0; j < primitives_.size(); ++j) {
      frame_result[primitives_[j]->get_name()] = 0.;
    }
    
    vector<pair<double,string> > &labels = timeline_[i];
    for (unsigned int j = 0; j < labels.size(); ++j) {
      if (labels[j].first > frame_result[labels[j].second])
        frame_result[labels[j].second] = labels[j].first;
    }
    
    result.push_back(frame_result);
  }
  return result;  
}


/**
 * Returns a primitive-centric mapping of each frame's confidence score
 * 
 **/
map<string, vector<double> > RealtimeObserver::GetPrimitiveCentricPerformanceTimeline(void) {
//  vector<vector<pair<double, string> > > timeline_;
  map<string, vector<double> > result;
  
  for (unsigned int j = 0; j < primitives_.size(); ++j) {
    vector<double> confidences;
    
    for (unsigned int i = 0; i < timeline_.size(); ++i) {
      vector<pair<double,string> > &labels = timeline_[i];
      double best_score = 0.;

      for (unsigned int k = 0; k < labels.size(); ++k) {
        if (labels[k].first > best_score && !strcmp(labels[k].second.c_str(), primitives_[j]->get_name().c_str()))
          best_score = labels[k].first;
      }
      
      confidences.push_back(best_score);
    }
    
    result[primitives_[j]->get_name()] = confidences;
  }
  
  return result;  
}












}  // namespace Observation
