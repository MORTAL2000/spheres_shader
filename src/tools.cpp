/*****************************************************************************/
/**
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2013/04/26: Release.
 * @date 2013/04/02: Initial commit.
 *****************************************************************************/
#include "tools.h"
#include "gl_globals.h"

#define QUERY_COUNTERS 8

#ifdef M_WINDOWS
#include <Windows.h>
LARGE_INTEGER frequency;                // ticks per second
LARGE_INTEGER start[QUERY_COUNTERS], end[QUERY_COUNTERS];       // ticks
#else
#include <ctime>
#include <sys/time.h>
timeval start[QUERY_COUNTERS];
timeval end[QUERY_COUNTERS];
long seconds;
long useconds;
#endif


GLuint gpuQueryIDStart[2][QUERY_COUNTERS];
GLuint gpuQueryIDEnd[2][QUERY_COUNTERS];

int g_timer_counter = 0;
int g_gpu_timer_counter = 0;
bool g_use_gpu_timers = false;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void initTools(bool use_gpu_timers)
{
#ifdef M_WINDOWS
  QueryPerformanceFrequency(&frequency);  // get ticks per second
#endif
  g_use_gpu_timers = use_gpu_timers;
  if(g_use_gpu_timers)
  {
    glGenQueries(QUERY_COUNTERS, gpuQueryIDStart[0]);
    glGenQueries(QUERY_COUNTERS, gpuQueryIDStart[1]);
    glGenQueries(QUERY_COUNTERS, gpuQueryIDEnd[0]);
    glGenQueries(QUERY_COUNTERS, gpuQueryIDEnd[1]);
    // dummy query to prevent OpenGL errors from popping out
    glQueryCounter(gpuQueryIDStart[0][0], GL_TIMESTAMP);
  }
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void timerStart()
{
  if(g_timer_counter>=QUERY_COUNTERS){
    printf("Error. Limit for Timer reached.\n");
    return;
  }
#ifdef M_WINDOWS
  QueryPerformanceCounter(&start[g_timer_counter]);
#else
  gettimeofday(&start[g_timer_counter], 0);
#endif
  ++g_timer_counter;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
double timerStop()
{
  double eltime;
  if(g_timer_counter<1){
    printf("Stack for Timer incomplete.\n");
    return 0;
  }
  --g_timer_counter;
#ifdef M_WINDOWS
  QueryPerformanceCounter(&end[g_timer_counter]); // stop timer
  eltime = double(end[g_timer_counter].QuadPart - start[g_timer_counter].QuadPart) * 1000.0 / frequency.QuadPart;
#else
  gettimeofday(&end[g_timer_counter], 0);

  seconds  = end[g_timer_counter].tv_sec  - start[g_timer_counter].tv_sec;
  useconds = end[g_timer_counter].tv_usec - start[g_timer_counter].tv_usec;
  eltime = (double)((seconds) * 1000 + useconds/1000.0) + 0.5;
#endif
  return eltime;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void gpuTimerStart(uint buffer)
{
  if(g_use_gpu_timers==false)
    return;
  if(g_gpu_timer_counter>=QUERY_COUNTERS){
    printf("Error. Limit for Timer reached.\n");
    return;
  }
  //gettimeofday(&start[g_timer_counter], 0);
  glQueryCounter(gpuQueryIDStart[buffer][g_gpu_timer_counter], GL_TIMESTAMP);

  ++g_gpu_timer_counter;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int gpuTimerStop(uint buffer)
{
  if(g_use_gpu_timers==false)
    return -1;
  if(g_gpu_timer_counter<1){
    printf("Stack for Timer incomplete.\n");
    return 0;
  }
  --g_gpu_timer_counter;

  glQueryCounter(gpuQueryIDEnd[buffer][g_gpu_timer_counter], GL_TIMESTAMP);

  return g_gpu_timer_counter;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
double gpuTimeElapse(uint buffer, uint query_index)
{
  if(g_use_gpu_timers==false)
    return -9999.0;
  GLuint64 startTime, stopTime;
  glGetQueryObjectui64v(gpuQueryIDStart[buffer][query_index], GL_QUERY_RESULT, &startTime);
  glGetQueryObjectui64v(gpuQueryIDEnd[buffer][query_index], GL_QUERY_RESULT, &stopTime);
  return ( (stopTime - startTime) / 1000000.0 );
}

