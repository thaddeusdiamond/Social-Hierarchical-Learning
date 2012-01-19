/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @author Thaddeus Diamond <diamond@cs.yale.edu>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for any skill acquired/to be acquired
 * in the primitives learning phase of SHL
 **/

#ifndef _SHL_PRIMITIVES_QLEARNER_PRIMITIVE_H_
#define _SHL_PRIMITIVES_QLEARNER_PRIMITIVE_H_

#include <string>
#include <vector>
#include <cstdlib>

using std::string;
using std::vector;

namespace Primitives {

class Primitive {
 public:
  Primitive(std::string name, std::vector<Sensor*> sensors)
    : name_(name), sensors_(sensors) {}

  virtual ~Primitive() {}
  virtual void save();
  virtual void load(std::string primitive_name);

  QTable & get_q_table() { return q_table_; }
  std::string get_name() { return name_; }
  std::vector<Sensor*> get_sensors() { return sensors_; }

 private:
  QTable q_table_;
  std::string name_;
  std::vector<Sensor*> sensors_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_QLEARNER_PRIMITIVE_H_
