/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Definitions of certain fixed-type actions are placed here
 * 
 **/

#include <string>
#include "QLearner/Condition.h"

const std::string Primitives::Condition::NO_CONDITION = "NO_CONDITION";
const std::string Primitives::Condition::OBJECT_NEAR = "OBJECT_NEAR";
const std::string Primitives::Condition::NO_OBJECT_NEAR = "NO_OBJECT_NEAR";
const std::string Primitives::Condition::OBJECT_DIST_FROM_GRIPPER =
  "OBJECT_DIST_FROM_GRIPPER";
