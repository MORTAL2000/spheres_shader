/*****************************************************************************/
/**
 * @file tools.h
 * @brief Some functions such as time measurement and output stuff.
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2016/03/12: mrand() added.
 * @date 2013/04/26: Release.
 * @date 2013/04/02: Initial commit.
 * @sa http://www.lighthouse3d.com/cg-topics/opengl-timer-query/
 *****************************************************************************/

#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdio.h>
#include <stdlib.h>

void initTools(bool use_gpu_timers);

/// CPU Timer
void timerStart();
/**
 * @retval Time in milliseconds.
 */
double timerStop();


/**
 * GPU Timer (OpenGL).
 * @param[in] buffer 0 or 1.
 */
void gpuTimerStart(unsigned buffer);
/**
 * @param[in] buffer 0 or 1.
 * @retval Current Query Index (for lookup later).
 */
int gpuTimerStop(unsigned buffer);
/**
 * @param[in] buffer 0 or 1.
 * @param[in] query_index GPU Query Index got from gpuTimerStop().
 * @retval Time in milliseconds.
 */
double gpuTimeElapse(unsigned buffer, unsigned query_index);


/**
 * My float randomizer for randoms in [a,b]
 * @param a lower bound
 * @param b upper bound
 * @return random float in [a,b]
 */
inline float mrand(float a, float b)
{
  //2.0f*rand()/RAND_MAX-1.0f
  float r = (float) rand() / RAND_MAX;
  return (b - a) * r + a;
}

#endif
