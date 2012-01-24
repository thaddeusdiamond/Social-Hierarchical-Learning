/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Exploration Function Implementation
 **/

#ifndef _SHL_PRIMITIVES_EXPLORATION_GREEDYEXPLORER_H_
#define _SHL_PRIMITIVES_EXPLORATION_GREEDYEXPLORER_H_

#include <string>
#include <vector>
#include "Exploration/ExplorationType.h"
#include "Student/Sensor.h"
#include "QLearner/QLearner.h"

namespace Primitives {

class GreedyExplorer : public ExplorationType {
 public:
  GreedyExplorer() {}
  ~GreedyExplorer() {}
  
  bool GetNextState(State *cur_state,
                    State ** next_state,
                    double *reward) {
    std::map<State*, std::map<std::string, double> > const &rewards =
      cur_state->get_reward();
    std::map<State*, std::map<std::string, double> >::const_iterator iter;
    
    State *best_candidate = NULL;
    double best_reward = -10000.;

    for (iter = rewards.begin(); iter != rewards.end(); ++iter) {
      State *prospect = iter->first;
      double prospect_reward = cur_state->GetRewardValue(prospect);
      if (best_candidate == NULL || prospect_reward > best_reward) {
        best_candidate = prospect;
        best_reward = prospect_reward;
      }                 
    }
    
    if (best_candidate == NULL) return false;
    
    *reward = best_reward;    
    *next_state = best_candidate;
    return true;
  }
};
  
} // namespace Primitives



#endif // _SHL_PRIMITIVES_EXPLORATION_GREEDYEXPLORER_H_
