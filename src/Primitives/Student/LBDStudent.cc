/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a student that can learn from files akin to
 * Learning By Demonstration.
 **/

#include "Student/LBDStudent.h"
#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <deque>
#include "QLearner/StandardQLearner.h"
#include "Exploration/GreedyExplorer.h"

namespace Primitives {

using google::protobuf::int64;
using std::string;
using Utils::Log;


QLearner* LBDStudent::LearnSkillFromFile(string filename, string skill_name) {
  const double PERCENT_START_STATES = .05;
  double NOISE_RATE = 0.005; /* Noise data 0.05% */

  double MAX_REWARD = 100.;

  const int BUF_SIZE = 4096;
  unsigned int FRAME_BUFFER = 3;
  char line_buf[BUF_SIZE];
  // char *saveptr_min_incr, *saveptr_nearby_thresh;
  char *saveptr_state_val;

  FILE *log_stream = NULL;

  // double unit_distance = 0.;
  // double nearby_distance = 0.;
  std::ifstream training_file(filename.c_str());
  if (!training_file.is_open()) return NULL;

  QLearner *skill = this->GetSkill(skill_name);
  if (skill == NULL) {
    skill = reinterpret_cast<QLearner *>(new StandardQLearner(skill_name));
    this->AddSkill(skill);
  }

  GreedyExplorer *exploration_function = new GreedyExplorer();
  skill->SetExplorationFunction(exploration_function);

  char *tokenizer;
/*
  // Get first line: Skill Name
  training_file.getline(line_buf, BUF_SIZE);
  std::string skill_name(line_buf);
  QLearner *skill = this->GetSkill(skill_name);
  if (skill == NULL) {
    skill = reinterpret_cast<QLearner *>(new StandardQLearner(skill_name));
  }


  // Get second line: Min increment values per sensor (used for
  // calculating unit_distance within state variables)
  training_file.getline(line_buf, BUF_SIZE);
  double running_total_unit_distance = 0.;
  char *tokenizer = strtok_r(line_buf, ", ", &saveptr_min_incr);

  while (tokenizer != NULL) {
    double min_increment = atof(tokenizer);
    min_increment *= min_increment;
    running_total_unit_distance += min_increment;
    tokenizer = strtok_r(NULL, ", ", &saveptr_min_incr);
  }
  // unit_distance = sqrt(running_total_unit_distance);

  // Get third line: Min nearby distance values per sensor
  // (currently unused)
  training_file.getline(line_buf, BUF_SIZE);
  double running_total_nearby_distance = 0.;
  tokenizer = strtok_r(line_buf, ", ", &saveptr_nearby_thresh);

  while (tokenizer != NULL) {
    double min_increment = atof(tokenizer);
    min_increment *= min_increment;
    running_total_nearby_distance += min_increment;
    tokenizer = strtok_r(NULL, ", ", &saveptr_nearby_thresh);
  }
  // nearby_distance = sqrt(running_total_nearby_distance);
*/

  // Every subsequent line: CSV of sensor values
  int frame_num = 1;
  QTable *qt = skill->get_q_table();
  std::deque<State*> seen_states;
  // Create array of "loaded states"
  // At each iteration through, add to array of loaded states
  // Once array size is 5 or greater, create state transitions for
  // state at index 0 to each subsequent state with reward MAX_REWARD/n
  // where n is the distance between the states in the array
  // finally, pop index 0 off of array
  while (!training_file.eof()) {
    training_file.getline(line_buf, BUF_SIZE);
    tokenizer = strtok_r(line_buf, ", ", &saveptr_state_val);
    std::vector<double> state_vector;
    string state_str = "Inserted: ";
    char dbuf[100];

    while (tokenizer != NULL) {
      double sensor_val = atof(tokenizer);

      // Noise incoming data if parameter is set
      if (NOISE_RATE >= .001) { 
        double rand_factor = static_cast<double>(rand() % 
                             (static_cast<int>(NOISE_RATE*1000)))
                             / 1000. - (NOISE_RATE / 2.);
                        
  
        sensor_val *= 1.+rand_factor;
      }
      
      state_vector.push_back(sensor_val);
      snprintf(dbuf, sizeof(dbuf), "%g, ", sensor_val);
      state_str += dbuf;
      tokenizer = strtok_r(NULL, ", ", &saveptr_state_val);
    }

    // Handle newlines in the file...
    if (state_vector.size() == 0) continue;

    State s(state_vector);
    char buf[1024];
    snprintf(buf, sizeof(buf), "Loaded state vector of size %ld",
             static_cast<int64>(state_vector.size()));
    Log(log_stream, DEBUG, buf);
    State *new_state = qt->GetState(s, false);
    if (!new_state) {
      new_state = qt->AddState(s);
    }

    snprintf(buf, sizeof(buf), "...New state vector of size %ld",
             static_cast<int64>(state_vector.size()));
    Log(log_stream, DEBUG, buf);
    Log(log_stream, DEBUG, new_state->to_string().c_str());

    seen_states.push_back(new_state);

    ++frame_num;
  }

  /*
   * Define initiation set according to PERCENT_START_STATES
   * Connect states together, out to FRAME_BUFFER proceeding states.
   *    - These connections are weighted as 1/n * MAX_REWARD, where
   *      n is the distance from the trained connection.
   */
  int state_index = 0;
  while (seen_states.size() > 1) {
    State *root = seen_states[0];
    unsigned int s_iter;
    double weight;
    unsigned int connect_count;

    if (state_index == 0 || static_cast<double>(state_index)
        < PERCENT_START_STATES * static_cast<double>(seen_states.size())) {
      qt->AddInitiateState(root);
    }


    for (s_iter = 1, connect_count = 1;
         s_iter < seen_states.size() && connect_count <= FRAME_BUFFER;
         ++s_iter, ++connect_count) {
      State *connect_state = seen_states[s_iter];

      if (!connect_state)
        Log(log_stream, ERROR, "Connecting to un-added state!");

      weight = MAX_REWARD / (static_cast<double>(connect_count));
      if (root != connect_state) {
       root->set_reward(connect_state, "base", weight);
       root->ConnectState(connect_state, Primitives::Action::INTERPOLATE);
      }
    }
    
    seen_states.pop_front();
    ++state_index;
  }

  training_file.close();

  char buf[4096];
  snprintf(buf, sizeof(buf), "Finished loading %d frames into LBD Student "
    "for %s. It now has %ld states", (frame_num-1), skill_name.c_str(),
    static_cast<int64>(qt->get_states().size()));
  Log(log_stream, DEBUG, buf);


  if (seen_states.size() == 1) {
    // Should always hit this condition.. something went wrong above
    snprintf(buf, sizeof(buf), "Added Goal State: %s",
             seen_states[0]->to_string().c_str());
    Log(stderr, SUCCESS, buf);
    qt->AddGoalState(seen_states[0], true);
  } else {
    Log(stderr, ERROR, "No goal states on this primitive...?");
    return NULL;
  }


  return skill;
}

}  // namespace Primitives
