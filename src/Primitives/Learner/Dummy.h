/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a dummy student while we finish enumerating functions/features.
 */

#ifndef _SHL_PRIMITIVES_LEARNER_DUMMY_H_
#define _SHL_PRIMITIVES_LEARNER_DUMMY_H_

#include "Learner/Student.h"

class Dummy : public Student {
 public:
  /**
   * The default constructor initializes a null learner object
   */
  explicit Dummy() {}

  /**
   * Destructor for TAMER must free all memory it received from I/O and
   * had buffered
   */
  virtual ~Dummy() {}

  virtual bool Setup(Object* objects, size_t objects_len);
  virtual bool SetLearningMethod(QLearner &learner);
  virtual bool LoadComparators(Primitive *primitive, Key* tables, size_t tables_len);
  virtual bool Learn(Primitive* prim, Motor* motors, size_t motors_len);
  virtual bool StopLearning(void);
  virtual bool ClearLog(void);
  virtual bool DumpLog(std::string &filename);
  virtual bool Log(std::string &str);
  virtual std::vector<Sensor*> *get_sensors();
};

#endif  // _SHL_PRIMITIVES_LEARNER_DUMMY_H_
