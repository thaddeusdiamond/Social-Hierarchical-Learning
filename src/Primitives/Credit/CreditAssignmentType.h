/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Credit Assignment Function 
 * Implementation
 */

#ifndef _SHL_PRIMITIVES_CREDIT_CREDITASSIGNMENTTYPE_H_
#define _SHL_PRIMITIVES_CREDIT_CREDITASSIGNMENTTYPE_H_

#include <string>
#include <vector>

/** @todo Namespaces should have descriptions **/
namespace Primitives {

using std::string;
using std::vector;

class QLearner;
class Sensor;

class CreditAssignmentType {
 public:
  /**
   * Destructor for CreditAssignmentType must free all memory it allocated
   * interally, but is not responsible for freeing anything passed into it.
   */
  virtual ~CreditAssignmentType() {}

  /**
   * Applies the given signal to the parent QLearner object
   *
   * @param     signal Reinforcement signal to interpret/assign
   *
   * @return    True on success, false on failure.
   */
  virtual bool ApplyCredit(double signal) = 0;

 protected:
  /**
   * Initialize back-pointer to parent QLearner object
   * 
   * @param learner QLearner object associated with this CreditAssignmentType
   **/
  explicit CreditAssignmentType(QLearner * const learner) {
    learner_ = learner;
  }

 private:
  /*
   * Disable default constructor
   */
  CreditAssignmentType() {}

  /**
   * Reference back to the parent QLearner object
   */
  QLearner *learner_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_CREDIT_CREDITASSIGNMENTTYPE_H_
