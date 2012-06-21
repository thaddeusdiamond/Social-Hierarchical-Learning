/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a class for translating conditions into strings and vice-versa.
 * Condition types are made enumerated as they often deal with specific
 * object or environmental conditions that must be satisfied
 **/

#include <string>


#ifndef _SHL_PRIMITIVES_QLEARNER_CONDITION_H_
#define _SHL_PRIMITIVES_QLEARNER_CONDITION_H_

namespace Primitives {
class Condition {
 public:
  /**
   * Converts action to string
   * @return Serialized action descriptor
   **/
  std::string toString() {
    std::string default_val(NO_CONDITION);
    
    return default_val;
  };
  
  /**
    * Consts define condition types, such as "OBJECT_NEAR". When adding a
    * new condition type, the full encoding protocol should be explicitly
    * specified in a comment above it.
  */ 
  static const std::string NO_CONDITION;
  static const std::string OBJECT_NEAR;
  static const std::string NO_OBJECT_NEAR;
  
  
 private:
  std::string prefix;
  std::string content;
  std::string suffix;
};
}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_CONDITION_H_
