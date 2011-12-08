/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an interface for a Q-Learning Exploration Function Implementation
 */

#ifndef _SHL_PRIMITIVES_LEARNER_SENSOR_H_
#define _SHL_PRIMITIVES_LEARNER_SENSOR_H_

#include <vector>

class Sensor {
 public:
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
  virtual int const get_num_values() { return num_values_; }

  /**
   * Provides an english-readable name for this sensor
   * 
   * @return Const char* string with the name of this sensor
   **/
  virtual char const * const get_name() { return name_; }

 protected:
  /**
   * Polls the associated sensor to get its new value
   * 
   * @return True on success, false on failure
   **/
  virtual bool poll() = 0;

 private:
  Sensor() {}

  /**
   * Sensor plaintext name
   **/
  char *name_;

  /**
   * Current values
   **/
  double *values_;

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
   * Describes the number of decimal places to record (truncate after # digits)
   **/
  int significant_figures_;

  /**
   * Describes the max variation to search when looking for "nearby" values
   **/
  double nearby_threshold_;
};
#endif  // _SHL_PRIMITIVES_LEARNER_SENSOR_H_
