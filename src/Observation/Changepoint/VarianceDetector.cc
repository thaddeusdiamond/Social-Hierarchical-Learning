/**
 * @file
 * @author Brad Hayes <hayesbh@gmail.com>
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 * Tracks the moving average variance over a timespan
 **/
 
#include "VarianceDetector.h"
#include <math.h>
#include <vector>

using std::vector;
namespace Observation {

  vector<int> VarianceDetector::GetChangepoints(
                    vector<vector<double> > &signal) {
    vector<int> changepoints;
    confidences_.clear();

    if (signal.size() < GetMinimumInputLength()) return changepoints;

    vector<double> moving_variance;
    vector<double> moving_variance_derivative;
    vector<double> confidence;

    int offset = window_size_ - 1;
    
    double mean = 0.;
    for (unsigned int i = 0; i < signal.size(); ++i) {
      mean += signal[i][dimension_];
    }
    mean /= signal.size();
    
    // Compute variance over time, storing N frame variance in first slot
    // of interval within moving_variance
    for (unsigned int i = 0; i < signal.size() - window_size_; ++i) {
      double mean = 0.;
      for (int j = 0; j < window_size_; ++j) {
        mean += signal[i + j][dimension_];
      }
      mean /= window_size_;

      double variance = 0.;
      for (int j = 0; j < window_size_; ++j) {
        double diff = signal[i+j][dimension_] - mean;
        variance += diff*diff;
      }
      variance /= window_size_;
      
      moving_variance.push_back(variance);
    }
    
    // Compute first derivative of moving_variance
    moving_variance_derivative.push_back(0);
    double max_first_derivative = 0.;
    for (unsigned int i = 1; i < moving_variance.size(); ++i) {
      double d = moving_variance[i] - moving_variance[i-1];
      if (fabs(d) > max_first_derivative)
        max_first_derivative = d;
      moving_variance_derivative.push_back(d);                                          
    }

    // Confidence of changepoint = abs(normalized first derivative graph)
    for (unsigned int i = 0; i < moving_variance_derivative.size(); ++i) {
      double normalized_derivative = moving_variance_derivative[i] / 
                                     max_first_derivative;
      if (normalized_derivative > 0.75) changepoints.push_back(i+offset);
      confidences_.push_back(normalized_derivative);
    }    
    
    return changepoints;
  }

} // namespace Observation
