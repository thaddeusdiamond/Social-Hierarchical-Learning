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

namespace Primitives {

QLearner* LBDStudent::LearnSkillFromFile(std::string filename) {
  const int BUF_SIZE = 4096;
  char line_buf[BUF_SIZE];
  char *saveptr_min_incr, *saveptr_nearby_thresh, *saveptr_state_val;

  // double unit_distance = 0.;
  // double nearby_distance = 0.;
  std::ifstream training_file(filename.c_str());
  if (!training_file.is_open()) return NULL;

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

  // Every subsequent line: CSV of sensor values
  int frame_num = 1;
  unsigned int frame_buffer = 5;
  QTable *qt = skill->get_q_table();
  std::deque<State*> seen_states;
  // Create array of "loaded states"
  // At each iteration through, add to array of loaded states
  // Once array size is 5 or greater, create state transitions for
  // state at index 0 to each subsequent state with reward 1/n
  // where n is the distance between the states in the array
  // pop index 0 off of array
  while (!training_file.eof()) {
    training_file.getline(line_buf, BUF_SIZE);
    tokenizer = strtok_r(line_buf, ", ", &saveptr_state_val);
    std::vector<double> state_vector;
    while (tokenizer != NULL) {
      double sensor_val = atof(tokenizer);
      state_vector.push_back(sensor_val);

      tokenizer = strtok_r(NULL, ", ", &saveptr_state_val);
    }
    State s(state_vector);
    State *new_state = qt->GetState(s, false);
    if (!new_state) new_state = qt->AddState(s);
    seen_states.push_back(new_state);

    if (seen_states.size() >= frame_buffer) {
      // Start making connections from state at index 0
      State *root = seen_states[0];
      std::deque<State*>::iterator s_iter;
      double weight = 1.;
      unsigned int connect_count;
      for (s_iter = seen_states.begin() + 1, connect_count = 1;
           s_iter != seen_states.end() && connect_count < frame_buffer;
           ++s_iter, ++connect_count) {
        State *connect_state = qt->GetState((**s_iter), false);
        if (!connect_state) connect_state = qt->AddState((**s_iter));
        weight = 1. / static_cast<double>(connect_count);
        root->set_reward(connect_state, "base", weight);
      }
      seen_states.pop_front();
    }
    ++frame_num;
  }

  while (seen_states.size() > 1) {
      State *root = seen_states[0];
      std::deque<State*>::iterator s_iter;
      double weight = 1.;
      unsigned int connect_count;
      for (s_iter = seen_states.begin() + 1, connect_count = 1;
           s_iter != seen_states.end() && connect_count < frame_buffer;
           ++s_iter, ++connect_count) {
        State *connect_state = qt->GetState((**s_iter), false);
        if (!connect_state) connect_state = qt->AddState((**s_iter));
        weight = 1. / static_cast<double>(connect_count);
        root->set_reward(connect_state, "base", weight);
      }
      seen_states.pop_front();
  }

  training_file.close();

  if (seen_states.size() != 0) {
    // Should always hit this condition.. otherwise I screwed up above
    qt->AddGoalState(seen_states[0]);
  } else {
    // @TODO: Log some error message
    return NULL;
  }

  return skill;
}

}  // namespace Primitives
