#pragma once

#include <limits>
#include <algorithm>
#include "CycleTimer.h"

/**
 * @brief Run function num_runs times, reporting minimum time
 *
 * @param num_runs Number of benchmark runs
 * @param fn Function to execute
 * @param args Arguments to be forwarded to fn
 * @return double Minimum time measured
 */
template <class Fn, class... Args>
double Benchmark(int num_runs, Fn&& fn, Args&&... args) {
  double min_time = std::numeric_limits<double>::max();
  for (int i = 0; i < num_runs; ++i) {
    double start_time = CycleTimer::currentSeconds();
    fn(std::forward<Args>(args)...);
    double end_time = CycleTimer::currentSeconds();
    min_time = std::min(min_time, end_time - start_time);
  }
  return min_time;
}