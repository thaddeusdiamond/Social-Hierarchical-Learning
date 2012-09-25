/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 **/

#include <string>
#include <vector>
#include <list>
#include <map>
#include "Exploration/AStarExplorer.h"
#include "Exploration/ExplorationType.h"
#include "Student/Sensor.h"
#include "QLearner/QLearner.h"

namespace Primitives {

/**
 * Defines a heuristic function to be used when finding goal states
 **/
double AStarExplorer::Heuristic(State *cur_state, State *goal_state) {
  return cur_state->GetEuclideanDistance(goal_state);
}
  


std::list<State *> AStarExplorer::ReconstructPath(
                                  std::map<State *, State *> came_from,
                                  State *cur_state) {
  std::list<State *> path;
  if (came_from.find(cur_state) != came_from.end()) {
    path = this->ReconstructPath(came_from, came_from[cur_state]);
  }
  path.push_back(cur_state);
  return path;
}

/**
 * Finds paths from the current state to the goal states specified for the
 * currently loaded skill.
 */
bool AStarExplorer::PopulatePaths(State *cur_state) {
  this->ClearPaths();
  
  // Go through all of the trained and discovered goal states known,
  // add paths for each one to the paths_ vector
  std::vector<State *>::iterator iter;
  std::vector<State *> &goal_states = q_table_->get_goal_states();
  for (iter = goal_states.begin(); iter != goal_states.end(); ++iter) {
    State *goal = *iter;
    std::list<State *> * path = FindPath(cur_state, goal);
    if (path != NULL)
      paths_.push_back(path);    
  }

  goal_states = q_table_->get_trained_goal_states();
  for (iter = goal_states.begin(); iter != goal_states.end(); ++iter) {
    State *goal = *iter;
    std::list<State *> * path = FindPath(cur_state, goal);
    if (path != NULL)
      paths_.push_back(path);    
  }

  
  // If there are postconditions
  // Run with a NULL goal_state to explore until any goal states are found.
  if (active_skill_->get_postconditions().size() > 0) {
    std::list<State *> * path = FindPath(cur_state, NULL);
    if (path != NULL)
      paths_.push_back(path);        
  }
  
  return (paths_.size() > 0);
}

std::list<State *> * AStarExplorer::FindPath(
                                      State *cur_state, State *goal_state) {
  
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
  state_scores[cur_state] = best_state_scores[cur_state] 
                            + Heuristic(cur_state, goal_state);
  
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
    if (lowest_score_state == NULL) {
      Log(stdout, ERROR, "AStarExplorer::FindPath - Empty open_set.");
      return NULL; 
    }

    State *cur_state = lowest_score_state;

  
    // Check to see if it's a goal state
    bool is_goal_state = false;
    if (goal_state == NULL)
      is_goal_state = active_skill_->IsGoalState(lowest_score_state);
    else
      is_goal_state = goal_state->Equals(lowest_score_state);
      
    if (is_goal_state) {
      //Reconstruct path and return it
      std::list<State *> found_path = this->ReconstructPath(came_from, 
                                                            cur_state);
      std::list<State *> * path = new std::list<State *>(found_path);
      return path;
    }
        
    // Remove current state from the open set, add to closed set
    open_set.erase(open_set.find(cur_state->get_state_hash()));
    closed_set[cur_state->get_state_hash()] = cur_state;
    
    
    // Look at all neighboring states, add them to the open set if we haven't
    // seen them before
    std::vector<State *> neighbors = q_table_->GetNearbyStates(*cur_state);
    std::vector<State *>::iterator n_iter;
    for (n_iter = neighbors.begin(); n_iter != neighbors.end(); ++n_iter) {
      State *neighbor = *n_iter;
      
      // If already evaluated, try the next one
      if (closed_set.find(neighbor->get_state_hash()) != closed_set.end())
        continue;
      
      
      double temp_heuristic_score = 
              best_state_scores[cur_state]
              + cur_state->GetEuclideanDistance(neighbor);
      
      if (open_set.find(neighbor->get_state_hash()) == open_set.end()
          || temp_heuristic_score < best_state_scores[neighbor]) {
        open_set[neighbor->get_state_hash()] = neighbor;
        came_from[neighbor] = cur_state;
        best_state_scores[neighbor] = temp_heuristic_score;
        state_scores[neighbor] = temp_heuristic_score 
                                 + Heuristic(neighbor, goal_state);
      }
    }
  }
  
  
  return NULL;
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
