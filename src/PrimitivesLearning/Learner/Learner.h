/**
 * @file
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for any reinforcement learning mechanism
 * operating in the primitives learning phase of SHL
 */

#ifndef _SHL_PRIMITIVESLEARNING_LEARNER_LEARNER_H_
#define _SHL_PRIMITIVESLEARNING_LEARNER_LEARNER_H_

class Object;
class Primitive;
class Motor;
class PostProcessor;

class Learner {
 public:
  /**
   * Destructor for a learner must free all memory it received from I/O and
   * had buffered
   */
  virtual ~Learner() {}

  /**
   * Before the learner begins its training session, it must construct the
   * appropriate environment (i.e. object placement, human involvement, etc.)
   *
   * @param     objects       A pointer to a set of objects
   * @param     objects_len   How many objects have been passed to the setup
   *
   * @return    True unless an error occurred in setup
   */
  virtual bool Setup(Object* objects, size_t objects_len) = 0;

  /**
   * Sometimes we will try to perform AB testing with the learner, but in order
   * to do so we will need to explicitly go to the datastore and get that
   * learned Q-Table.
   *
   * @param     tables        A set of IDs for the Q-Tables we want to load
   * @param     tables_len    How many tables we are performing a comparison w/
   *
   * @return    True unless an error occurred during load time
   */
  virtual bool LoadComparators(Key* tables, size_t tables_len) = 0;

  /**
   * We explicitly alert the learner, which can be used on a one-off basis or
   * reused between trials, that a specific primitive is now being trained
   * against.
   *
   * @param     prim          A pointer to a primitive to train against
   * @param     motors        A pointer to a set of motors involved in trial
   * @param     motors_len    How many motors are expected to be used
   *
   * @return    True if the robot succeeds in task, fail otherwise
   */
  virtual bool Begin(Primitive* prim, Motor* motors, size_t motors_len) = 0;

  /**
   * At the end of any trial, the learner should dump the raw output to the
   * post-processor for further analysis and interpretation.
   *
   * @param     post_proc     A post-processor pointer that we will communicate
   *
   * @return    True unless there was an error dumping data
   */
  virtual bool UnloadData(PostProcessor* post_proc) = 0;
};

#endif  // _SHL_PRIMITIVESLEARNING_LEARNER_LEARNER_H_
