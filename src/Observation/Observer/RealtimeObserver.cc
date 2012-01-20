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
#include <vector>
#include <utility>

namespace Observation {

using std::pair;
using std::vector;
using std::string;

bool RealtimeObserver::Observe(Task* task) {
  return true;
}

bool RealtimeObserver::StopObserving() {
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
vector<string> RealtimeObserver::GetTimeline() {
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
