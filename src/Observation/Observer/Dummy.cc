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

bool Dummy::Observe(Task* task, QTable* table, int tables_len,
                    vector<Sensor*> const &sensors) {
  return true;
}

bool Dummy::StopObserving(void) {
  return true;
}

double Dummy::AttemptMatch(QTable const &table, vector<Sensor*> const &sensors,
                           double min_threshold, double death_time) {
  return 0.0;
}
