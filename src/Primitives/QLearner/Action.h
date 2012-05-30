/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a class for translating actions into strings and vice-versa.
 * Several default action types are made available through this file as well,
 * including non-specific actions such as "interpolate motor positions"
 **/

#include <string>


#ifndef _SHL_PRIMITIVES_QLEARNER_ACTION_H_
#define _SHL_PRIMITIVES_QLEARNER_ACTION_H_

namespace Primitives {
  class Action {
   public:
    /**
     * Converts action to string
     * @return Serialized action descriptor
     **/
    std::string toString() {
      std::string default_val(NO_ACTION);
      
      return default_val;
    };
    
    // Consts define hardcoded action types, such as 
    // "interpolate motor positions"
    static const std::string NO_ACTION;
    static const std::string INTERPOLATE;
  };
}

#endif // _SHL_PRIMITIVES_QLEARNER_ACTION_H_
