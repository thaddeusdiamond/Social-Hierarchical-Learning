/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Sensor used by the QLearner
 */

#ifndef _SHL_PRIMITIVES_STUDENT_SENSOR_H_
#define _SHL_PRIMITIVES_STUDENT_SENSOR_H_

#include <vector>
#include <string>

namespace Primitives {

class Sensor {
 public:
  explicit Sensor(std::string sensor_name) {
    name_ = sensor_name;
  }

  /**
   * Default destructor, does nothing
   **/
  virtual ~Sensor() {}

  /**
   * Sets the values of this sensor if it supports it.
   * @param values Array of doubles to set this sensor to
   * @param num_values Size of passed array
   * 
   * @return True on success, false if setting is not permitted
   **/
  virtual bool SetValues(double const * const values, int num_values) = 0;

  /**
   * Returns the values held by this sensor
   * 
   * @return const array of doubles indicating values of this sensor
   **/
  virtual double const * const GetValues() = 0;

  /**
   * Accessor for number of values this sensor stores
   * 
   * @return Size of values_ array
   **/
  virtual int const get_num_values() const { return num_values_; }

  /**
   * Accessor for max # decimal places recorded by sensor
   * 
   * @return Size of values_ array
   **/
  virtual int const get_min_increment() const {
    return min_increment_;
  }

  /**
   * Provides a constant that signifies the max distance to consider
   * something 'nearby' for this sensor.
   * 
   * @return Const char* string with the name of this sensor
   **/
  virtual double get_nearby_threshold() const { return nearby_threshold_; }
  virtual void set_nearby_threshold(double t) { nearby_threshold_ = t; }

  /**
   * Provides an english-readable name for this sensor
   * 
   * @return Const char* string with the name of this sensor
   **/
  virtual std::string const &get_name() const { return name_; }

 protected:
  /**
   * Polls the associated sensor to get its new value
   * 
   * @return True on success, false on failure
   **/
  virtual bool Poll() = 0;

  /** @todo I think these should really be removed since Sensor defines
   *        an interface, though we can leave it to show some of the things
   *        that might belong in a Sensor object **/

  /**
   * Sensor plaintext name
   **/
  std::string name_;

  /**
   * Current values
   **/
  double * values_;

  /**
   * Number of values encapsulated by sensor
   **/
  int num_values_;

  /**
   * Max allowed value for sensor
   **/
  double max_value_;

  /**
   * Min allowed value for sensor
   **/
  double min_value_;

  /**
   * Describes the minimum possible increment for this value
   **/
  double min_increment_;

  /**
   * Describes the max variation to search when looking for "nearby" values
   **/
  double nearby_threshold_;
};

}  // namespace Primitives

#endif  // _SHL_PRIMITIVES_STUDENT_SENSOR_H_
