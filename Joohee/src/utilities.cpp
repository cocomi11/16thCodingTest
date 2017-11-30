#include "utilities.h"

// get time in micro seconds for invoking a function (N) times
// start: starting time
// end: end time
// N : number of repetitions 
double get_time_us( std::chrono::time_point<std::chrono::steady_clock> & start, 
					std::chrono::time_point<std::chrono::steady_clock> & end, 
					uint32_t N){
  auto diff = end-start;
  return (long double)(std::chrono::duration_cast<std::chrono::microseconds>(diff).count())/N;
}
