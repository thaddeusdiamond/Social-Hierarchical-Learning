/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 **/

#include <string>
#include <vector>
#include <map>
#include "Exploration/AStarExplorer.h"
#include "Exploration/ExplorationType.h"
#include "Student/Sensor.h"
#include "QLearner/QLearner.h"

namespace Primitives {

/**
 * Defines a heuristic function to be used when finding goal states
 **/
double AStarExplorer::Heuristic(State *cur_state) {
  return 0.0;
}
  
  
/**
 * Finds paths from the current state to the goal states specified for the
 * currently loaded skill.
 */
bool AStarExplorer::PopulatePaths(State *cur_state) {
  
  // Map from State hash to State pointer
  std::map<std::string, State*> closed_set;

  // Explored nodes
  std::map<State*, State*> came_from;

  // Nodes to be expanded
  std::map<std::string, State*> open_set;

  // Map from State pointer to associated goal-distance score
  std::map<State *, double> state_scores;  
  std::map<State *, double> best_state_scores;


  // Initialize with start state (cur_state)
  open_set[cur_state->get_state_hash()] = cur_state;
  best_state_scores[cur_state] = 0.;
  state_scores[cur_state] = best_state_scores[cur_state] + Heuristic(cur_state);
  
  while (open_set.size() > 0) {
    
    // Get state with the lowest composite distance score from open_set
    State *lowest_score_state = NULL;
    double lowest_score = 0.0;
    std::map<std::string, State*>::iterator iter;
    for (iter = open_set.begin(); iter != open_set.end(); ++iter) {
      double score = state_scores[iter->second];
      if (iter == open_set.begin() || score < lowest_score) {
        lowest_score = score;
        lowest_score_state = iter->second;
      }
    }
    
    // Error condition
    if (lowest_score_state == NULL) return false;
  
    // Check to see if it's a goal state
    if (active_skill_->IsGoalState(lowest_score_state)) {
      //TODO: Reconstruct path and store in paths_
      return true;
    }
    
    open_set.erase(open_set.find(lowest_score_state->get_state_hash()));
    closed_set[lowest_score_state->get_state_hash()] = lowest_score_state;
    
    std::vector<State *> neighbors;
    std::vector<State *>::iterator n_iter;
    for (n_iter = neighbors.begin(); n_iter != neighbors.end(); ++n_iter) {
      State *neighbor = *n_iter;
      
      // If already considered, try the next one
      if (closed_set.find(neighbor->get_state_hash()) != closed_set.end())
        continue;
      
      
      double temp_heuristic_score = 
              best_state_scores[lowest_score_state]
              + lowest_score_state->GetEuclideanDistance(neighbor);
      
      if (open_set.find(neighbor->get_state_hash()) == open_set.end()
          || temp_heuristic_score < best_state_scores[neighbor]) {
        open_set[neighbor->get_state_hash()] = neighbor;
        came_from[neighbor] = lowest_score_state;
        best_state_scores[neighbor] = temp_heuristic_score;
        state_scores[neighbor] = temp_heuristic_score + Heuristic(neighbor);
      }
    }
  }
  
  
  return false;
}

/**
 * Chooses an index into paths_ as the "active" path worth following
 * @return path_ index to follow, -1 if none found
 **/
int AStarExplorer::ChoosePath() {
  return -1;
}

/**
 * Performs a reward-transition update step along a given path,
 * setting a reward layer (starting from goal, diminishing toward the start),
 * to direct progress towards goal states.
 **/  
void AStarExplorer::UpdateStateActionGraph() {
  
}
 
bool AStarExplorer::GetNextState(State *cur_state,
                  State ** next_state,
                  double *reward) {
  // If paths_ is empty, populate it
  // If paths_ hasn't been updated recently enough, refresh it
  // If expected_state_ isn't NULL and cur_state doesn't equal it, re-plan.
  
  // Take the first state from the path vector being used, update paths_
  
 
                    
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



}  // namespace Primitives
