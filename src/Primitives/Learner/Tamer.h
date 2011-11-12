/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for the TAMER framework for reinforcement learning
 */

#ifndef _SHL_PRIMITIVES_LEARNER_TAMER_H_
#define _SHL_PRIMITIVES_LEARNER_TAMER_H_

#include "Learner/Learner.h"

class Tamer : public Learner {
 public:
  /**
   * The TAMER constructor initializes a null learner object
   */
  explicit Tamer() {}

  /**
   * Destructor for TAMER must free all memory it received from I/O and
   * had buffered
   */
  virtual ~Tamer() {}

  virtual bool Setup(Object* objects, size_t objects_len);
  virtual bool LoadComparators(Key* tables, size_t tables_len);
  virtual bool Begin(Primitive* prim, Motor* motors, size_t motors_len);
  virtual bool UnloadData(PostProcessor* post_proc);
};

#endif  // _SHL_PRIMITIVES_LEARNER_TAMER_H_
