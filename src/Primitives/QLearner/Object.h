/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is a placeholder class for an object involved in a learning task
 **/

#ifndef _SHL_PRIMITIVES_QLEARNER_OBJECT_H_
#define _SHL_PRIMITIVES_QLEARNER_OBJECT_H_

namespace Primitives {

class Object {
 public:
  /**
   * The default constructor initializes a null learner object
   **/
  explicit Object() {}

  /**
   * Destructor for dummy student must free all memory it received from I/O and
   * had buffered
   **/
  virtual ~Object() {}
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_OBJECT_H_
