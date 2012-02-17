/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Tracks the moving average variance over a timespan
 **/
 
#include <vector>
#include "ChangepointDetector.h"
 
namespace Observation {

using std::vector;
  
class VarianceDetector : public ChangepointDetector {
 public:
  VarianceDetector(int dimension, int min_len = 10) : dimension_(dimension),
                                                      min_length_(min_len) {
    sensitivity_ = 1.;
    window_size_ = min_length_;
  }

  vector<int> GetChangepoints(vector<vector<double> > &signal);

  /**
   * This detector only works reliably after at least 10 frames exist to be
   * analyzed
   **/
  unsigned int GetMinimumInputLength() { return min_length_; };

  void IncreaseSensitivity() {
    if (window_size_ > 3)
      --window_size_;
  }
  
  void DecreaseSensitivity() {
    ++window_size_;
    if (window_size_ >= min_length_)
      ++min_length_;
  }

  vector<double> & GetLastConfidences() { return confidences_; }

 private:
  /**
   * Indicates which dimension of the input vector to analyze
   **/
  int dimension_;
  int window_size_;
  int min_length_;
  vector<double> confidences_;
};

} // namespace Observation
