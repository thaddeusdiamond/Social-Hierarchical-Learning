/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is the implementation for the TAMER framework for reinforcement learning
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

bool Dummy::LoadComparators(Primitive *primitive, Key* tables, size_t tables_len) {
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

bool Dummy::DumpLog(std::string &filename) {
  return true;
}

bool Dummy::Log(std::string &str) {
  return true;
}

std::vector<Sensor*> *get_sensors() {
  return NULL;
}


