/**
 * @author Brad Hayes (bradley.h.hayes@yale.edu)
 * IROS'12 Test Framework
 *
 **/


#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Primitives/Student/LBDStudent.h"
#include "Primitives/Student/FeedbackSensor.h"
#include "Observation/Playback/PlaybackSensor.h"
#include "Observation/Observer/RealtimeObserver.h"
#include "Common/Utils.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;


using Primitives::LBDStudent;
using Primitives::QLearner;
using Primitives::State;
using Primitives::FeedbackSensor;
using Observation::Observer;
using Observation::RealtimeObserver;
using Observation::PlaybackSensor;
using Utils::Log;

static string primitive_list[] = {
    "coat", "shoes", "school", "store", "drive", "in", "out"
};
static unsigned int primitives_count = 7;
const int NUM_SENSORS = 6;

const int REPETITIONS_PER_SKILL = 2;

const double XY_MIN = 0.01;
const double Z_MIN = 10.;
const double NEARBY_MULTIPLIER = 1.;


bool OutputStatePath(vector<State *> path, string filename) {
  std::ofstream outfile(filename.c_str());
  if (!outfile.is_open()) return false;
  
  for (unsigned int i = 0; i < path.size(); ++i) {
    State *s = path[i];
    char buf[4096];
    std::vector<double> const &state_vector = s->get_state_vector();
    for (unsigned int j = 0; j < state_vector.size(); ++j) {
      snprintf(buf,sizeof(buf),"%s, %g", buf, state_vector[j]);
    }
    snprintf(buf,sizeof(buf),"%s\n", buf);
    outfile.write(buf,sizeof(buf));
  }
  
  return true;
}


void EvaluateDetector(RealtimeObserver *obs, 
                      double obs_duration,
                      LBDStudent *student, 
                      string const &base_dir,
                      char *filename,
                      bool use_waypointing) {
    // Transfer student skills to observer
    vector<QLearner *> * student_skills = student->get_primitives();
    vector<QLearner *>::iterator s_iter;
    for (s_iter = student_skills->begin(); s_iter != student_skills->end();
         ++s_iter) {
      obs->AddSkill(*s_iter);
    }

    if (use_waypointing)
      obs->use_waypointing_ = false;

    char test_file[1024];
    snprintf(test_file, sizeof(test_file), "%s%s.csv", base_dir.c_str(),
             filename);

    PlaybackSensor *test_sensor = new PlaybackSensor(test_file, NUM_SENSORS);
    test_sensor->set_nearby_threshold(XY_MIN*NEARBY_MULTIPLIER);
    obs->AddSensor(test_sensor);
    obs->Observe(NULL, obs_duration);

    // vector<vector<pair<double, string> > timeline = obs->get_timeline();
    vector<string> final_timeline = obs->GetFinalTimeline();

    for (unsigned int i = 0; i < final_timeline.size(); ++i) {
      cout << i << ": " << final_timeline[i] << endl;
    }

    map<string, vector<double> > p_confidences =
      obs->GetPrimitiveCentricPerformanceTimeline();

    map<string, vector<double> >::iterator c_iter;
    for (c_iter = p_confidences.begin();
         c_iter != p_confidences.end();
         ++c_iter) {
      vector<double> &vals = c_iter->second;
      cout << c_iter->first << ": ";
      if (vals.size()) cout << vals[0];
      for (unsigned int i = 1; i < vals.size(); ++i) {
        cout << ", " << vals[i];
      }
      cout << endl;
    }


    delete test_sensor;  
}

vector<QLearner *> GetSeenSkills(LBDStudent *student, 
                        vector<string> const &observed_skill_names) {
  map<string, QLearner *> skills;
  vector<QLearner *> ret_skills;
  
  // Todo: Go through obs_skill_names and find their corresponding qlearner
  // in the student and add it to 'skills'
  vector<QLearner *>:: iterator skill_iter;
  vector<string>::const_iterator skill_name_iter;
  for (skill_name_iter = observed_skill_names.begin(); 
       skill_name_iter !=  observed_skill_names.end(); 
       ++skill_name_iter) {
    QLearner *skill = student->GetSkill(*skill_name_iter);
    if (skill != NULL) {
      if (skills.find(skill->get_name()) == skills.end())
        skills[skill->get_name()] = skill;
    }
  }
  
  map<string, QLearner *>::iterator skill_map_iter;
  for (skill_map_iter = skills.begin();
       skill_map_iter != skills.end();
       ++skill_map_iter) {
    ret_skills.push_back(skill_map_iter->second);
  }
  
  return ret_skills;
}


int main(int argc, char* argv[]) {
  if (argc < 5) {
    cout << "iros12 <training dir> <test file> <observation duration>"
         << "<waypointing: 1 or 0>" << endl;
    return 0;
  }

  double duration = atof(argv[3]);
  bool use_waypointing = (argv[4][0] == '0');
  cout << "Starting test:" << endl;
  string base_dir(argv[1]);

  LBDStudent student;


  vector<double> nearby_thresholds;


  nearby_thresholds.push_back(XY_MIN*NEARBY_MULTIPLIER);
  nearby_thresholds.push_back(XY_MIN*NEARBY_MULTIPLIER);
  nearby_thresholds.push_back(Z_MIN*NEARBY_MULTIPLIER);
  nearby_thresholds.push_back(XY_MIN*NEARBY_MULTIPLIER);
  nearby_thresholds.push_back(XY_MIN*NEARBY_MULTIPLIER);
  nearby_thresholds.push_back(Z_MIN*NEARBY_MULTIPLIER);

  double sampling_frequency = 1000./29.;


  char filename[100];
  // Give multiple training examples per skill
  for (int reps = 0; reps < REPETITIONS_PER_SKILL; ++reps) {
    for (unsigned int i = 0; i < primitives_count; ++i) {
      snprintf(filename, sizeof(filename), "%s%s.csv", base_dir.c_str(),
               primitive_list[i].c_str());
      QLearner *skill = student.LearnSkillFromFile(string(filename),
                                                   primitive_list[i]);
      if (skill == NULL) Log(stderr, ERROR, "Null skill loaded.");
      skill->get_q_table()->set_nearby_thresholds(nearby_thresholds);
      skill->set_anticipated_duration(sampling_frequency
                                * skill->get_q_table()->get_states().size());
      student.AddSkill(skill);
      char buf[1024];
      snprintf(buf, sizeof(buf),
               "Loaded Skill %s with %ld states and duration %g",
               skill->get_name().c_str(),
               skill->get_q_table()->get_states().size(),
               (skill->get_anticipated_duration()/1000.));
      Log(stderr, DEBUG, buf);
    }
  }

  // Evaluation loop:
  // Observe scene, improve skills by demonstrating understanding and optionally
  // requesting follow-up data for particular relevant skills in the observation
  int playback = 0;
  while (playback < 10) {
    RealtimeObserver observer(sampling_frequency);
    EvaluateDetector(&observer, duration, 
                     &student, base_dir, argv[2], use_waypointing);

    vector<QLearner *> skills = GetSeenSkills(&student,
                                              observer.GetFinalTimeline());
    

    if (skills.size() == 0) break;

    // pick observed skill at random
    // (maybe later- pick the one with lowest confidence?)
    int chosen_skill_index = rand() % skills.size();
    
    cout << "Choose skill" << endl;
    // get playback path through MDP space
    QLearner *chosen_skill = skills[chosen_skill_index];
    
    cout << "Get playback path" << endl;
    // write playback path through MDP space to file
    vector<State *> playback_path = chosen_skill->GetNearestFixedExecutionPath(
                                      NULL);
                                      
    ++playback;
    cout << " Next Iter " << endl;
    continue; 
    bool request_demo = false;
    if (playback_path.size() == 0) {
      // Couldn't get a good path for some reason
      char buf[1024];
      snprintf(buf, sizeof(buf), "Could not generate a path for %s", 
               chosen_skill->get_name().c_str());
      Log(stderr, ERROR, buf);

      // TODO: Say "I can't figure out how to do xxxx. Please show me."
      request_demo = true;
    } else {
      // save state path to file
      OutputStatePath(playback_path, "playbackpath.csv");

      // call playback executable with file

      // ask for feedback

      // take keyboard feedback
      int feedback_type = 0;
      if (feedback_type == FeedbackSensor::BAD)
        request_demo = true;
    }

    if (request_demo) {
      // capture new feedback from kinect OR load from existing file
      // apply new feedback as training to QLearner
      student.LearnSkillFromFile("playbackresult.csv",chosen_skill->get_name());
    }

    ++playback;
  }
  
  return 0;
}
