/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a simple dummy implementation
 **/

#include "Observer/Dummy.h"

namespace Observation {

bool Dummy::Observe(Task* task) {
  return true;
}

bool Dummy::StopObserving(void) {
  return true;
}

}  // namespace Observation
