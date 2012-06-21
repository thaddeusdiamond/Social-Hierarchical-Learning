/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for action generators. An action generator is capable
 * of taking environmental inputs and a sample action (pre/post condition set)
 * and then generating similar actions.
 * 
 * Example case:
 *  Template action is picking up a particular red block and placing it 
 *  in the red container. User specifies the invariants (object class, 
 *  post-condition). Generator creates actions for each instance of 'red block'
 *  on the table.
 **/


#ifndef _SHL_MANAGER_PLAN_GENERATOR_H_
#define _SHL_MANAGER_PLAN_GENERATOR_H_

namespace Primitives {
class Generator {
};
}
  
#endif  // _SHL_MANAGER_PLAN_GENERATOR_H_
