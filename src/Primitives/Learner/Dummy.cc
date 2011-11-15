/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an implementation of a dummy student that can learn
 */

#include <vector>
#include <string>
#include "Learner/Dummy.h"
#include "Learner/QLearner.h"

bool Dummy::Setup(Object* objects, size_t objects_len) {
  return true;
}

bool Dummy::SetLearningMethod(QLearner &learner) {
  return true;
}

bool Dummy::LoadComparators(Primitive *prim, Key* tables, size_t tables_len) {
  return true;
}

bool Dummy::Learn(Primitive* prim, Motor* motors, size_t motors_len) {
  return true;
}

bool Dummy::StopLearning(void) {
  return true;
}

bool Dummy::ClearLog(void) {
  return true;
}

bool Dummy::DumpLog(string &filename) {
  return true;
}

bool Dummy::Log(string &str) {
  return true;
}

vector<Sensor*>* Dummy::GetSensors() {
  return NULL;
}


