/**
 * @author Brad Hayes (bradley.h.hayes@yale.edu)
 * AAAI'12 Test Framework
 * 
 **/


#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include "Primitives/Student/LBDStudent.h"
#include "Observation/Playback/PlaybackSensor.h"
#include "Observation/Observer/RealtimeObserver.h"
#include "Common/Utils.h"

using namespace std;
using namespace Primitives;
using namespace Observation;
using namespace Utils;

static string primitive_list[] = {
    "coat", "shoes", "school", "store", "drive", "in", "out"
};
static unsigned int primitives_count = 7;

int main(int argc, char* argv[]) {
  
  if (argc < 5) {
    cout << "aaai12 <training dir> <test file> <observation duration> <waypointing: 1 or 0>" << endl;
  }
  
  double duration = atof(argv[3]);
  
  
  cout << "Starting test:" << endl;
  string base_dir(argv[1]);
  
  LBDStudent student;
  
  
  vector<double> nearby_thresholds;
  double xy_min = 0.01;
  double z_min = 20.;
  double nearby_multiplier = 5.;
  //xy_min *= xy_min;
  //z_min *= z_min;
  //nearby_multiplier *= nearby_multiplier;

  nearby_thresholds.push_back(xy_min*nearby_multiplier);
  nearby_thresholds.push_back(xy_min*nearby_multiplier);
  nearby_thresholds.push_back(z_min*nearby_multiplier);
  nearby_thresholds.push_back(xy_min*nearby_multiplier);
  nearby_thresholds.push_back(xy_min*nearby_multiplier);
  nearby_thresholds.push_back(z_min*nearby_multiplier);

  double sampling_frequency = 1000./29.;
  
  
  char filename[100];
  //Give multiple training examples per skill
  for (int reps = 0; reps < 5; ++reps) {
    for (unsigned int i = 0; i < primitives_count; ++i) {
      snprintf(filename,100,"%s%s.csv", base_dir.c_str(), 
               primitive_list[i].c_str());
      QLearner *skill = student.LearnSkillFromFile(string(filename), 
                                                   primitive_list[i]);
      if (skill == NULL) Log(stderr,ERROR,"Null skill loaded.");
      skill->get_q_table()->set_nearby_thresholds(nearby_thresholds);
      skill->set_anticipated_duration(sampling_frequency 
                                      * skill->get_q_table()->
                                      get_states().size());
      student.AddSkill(skill);
      char buf[1024];
      snprintf(buf,1024,"Loaded Skill %s with %ld states and duration %g", 
               skill->get_name().c_str(),
               skill->get_q_table()->get_states().size(),
               (skill->get_anticipated_duration()/1000.));
      Log(stderr, DEBUG, buf);  
    }
  }
  
  RealtimeObserver observer(sampling_frequency);
  
  // Transfer student skills to observer
  vector<QLearner *> * student_skills = student.get_primitives();
  vector<QLearner *>::iterator s_iter;
  for (s_iter = student_skills->begin(); s_iter != student_skills->end();
       ++s_iter) {
    observer.AddSkill(*s_iter);
  }

  if (argv[4][0] == '0')
    observer.use_waypointing_ = false;
 
  char test_file[1024];
  snprintf(test_file,1024,"%s%s.csv", base_dir.c_str(), argv[2]);

  PlaybackSensor *test_sensor = new PlaybackSensor(test_file,6);
  test_sensor->set_nearby_threshold(xy_min*nearby_multiplier);
  observer.AddSensor(test_sensor);
  observer.Observe(NULL, duration);

  // vector<vector<pair<double, string> > timeline = observer.get_timeline();
  vector<string> final_timeline = observer.GetFinalTimeline();
  
  for (unsigned int i = 0;i < final_timeline.size(); ++i) {
    cout << i << ": " << final_timeline[i] << endl;
  }
  
  map<string, vector<double> > p_confidences = observer.GetPrimitiveCentricPerformanceTimeline();
  
  map<string, vector<double> >::iterator c_iter;
  for (c_iter = p_confidences.begin(); c_iter != p_confidences.end(); ++c_iter) {
    vector<double> &vals = c_iter->second;
    cout << c_iter->first << ": ";
    if (vals.size()) cout << vals[0];
    for (unsigned int i = 1; i < vals.size(); ++i) {
      cout << ", " << vals[i];
    }
    cout << endl;
  }
  
  
  delete test_sensor;
  return 0; 
}
