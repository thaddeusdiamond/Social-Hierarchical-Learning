/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Runs A* search from current state to each defined goal state. Chooses
 * the highest-reward path of all found paths.
 **/

#ifndef _SHL_PRIMITIVES_EXPLORATION_ASTAREXPLORER_H_
#define _SHL_PRIMITIVES_EXPLORATION_ASTAREXPLORER_H_

#include <string>
#include <vector>
#include <map>
#include <list>
#include "Student/Student.h"
#include "Exploration/ExplorationType.h"
#include "Student/Sensor.h"
#include "QLearner/QLearner.h"

namespace Primitives {

class AStarExplorer : public ExplorationType {
 public:
  AStarExplorer(Student *s, QLearner *skill, double max_search_time) 
    : owner_student_(s), 
      active_skill_(skill),
      max_search_time_(max_search_time) {}
  ~AStarExplorer() { ClearPaths(); }
  
  
  bool PopulatePaths(State *cur_state);
  std::list<State *> * FindPath(State * cur_state,
                                               State * goal_state);
  
  bool GetNextState(State *cur_state,
                    State ** next_state,
                    double *reward);

 private:
  std::list<State *> ReconstructPath(
                                     std::map<State *, State *> came_from,
                                     State *cur_state);
  int ChoosePath();
  void UpdateStateActionGraph();
  void ClearPaths() {
    std::vector<std::list<State *> *>::iterator iter;
    for (iter = paths_.begin(); iter != paths_.end(); ++iter) {
      std::list<State *> * path = *iter;
      delete path;
    }
  }
  double Heuristic(State *cur_state, State *goal_state);

  Student *owner_student_;  
  QLearner *active_skill_;
  double max_search_time_;
  std::vector<std::list<State *> * > paths_;
  State *expected_state_;
  QTable *q_table_;
};

}  // namespace Primitives



#endif  // _SHL_PRIMITIVES_EXPLORATION_ASTAREXPLORER_H_
