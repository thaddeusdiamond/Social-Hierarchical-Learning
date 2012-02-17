/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * This is an abstract interface for a changepoint detector.
 **/

#ifndef _SHL_OBSERVATION_CHANGEPOINT_CHANGEPOINTDETECTOR_H_
#define _SHL_OBSERVATION_CHANGEPOINT_CHANGEPOINTDETECTOR_H_

#include <vector>

namespace Observation {
  
using std::vector;

class ChangepointDetector {
 public:
  virtual ~ChangepointDetector() {}
  
  /**
   * Given a timeseries of n-vectors, detect changepoints and return
   * a vector containing their indices.
   * 
   * @param signal Outer vector indicates frame number, inner vector is
   *        n-dimensional frame data
   * @returns Vector of frame indices where changepoints occur
   **/
  virtual vector<int> GetChangepoints(vector<vector<double> > &signal) = 0;
  
  
  /**
   * Increases the sensitivity of this changepoint detector to declare
   * changepoints more frequently.
   **/
  virtual void IncreaseSensitivity() = 0;

  /**
   * Decreases the sensitivity of this changepoint detector to declare
   * changepoints less frequently.
   **/
  virtual void DecreaseSensitivity() = 0;  
  
  /**
   * Hard-coded value decided by implementor indicating minimum amount of
   * datapoints are required to get valid output from this detector.
   * 
   * @returns Minimum amount of data points required for changepoints to
   *          be detected reliably
   **/
   virtual unsigned int GetMinimumInputLength() = 0;

 protected:
  double sensitivity_;
};

}  // namespace Observation

#endif  // _SHL_OBSERVATION_CHANGEPOINT_CHANGEPOINTDETECTOR_H_
