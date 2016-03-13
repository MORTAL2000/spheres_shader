/*****************************************************************************/
/**
 * These examples demonstrate the performance of raycasted spheres
   (Sphere Impostors) and geometry based spheres (with geometry instancing).
   You can use the code for your own work. If you find some improvements, just
   let me know. These examples are based on several tutorials around the web.
 *
 * @author Matthias Werner
 * @sa http://11235813tdd.blogspot.de/
 * @date 2016/03/12: Refactoring of code.
 * @date 2013/04/26: Release.
 * @date 2013/04/02: Initial commit.
 *****************************************************************************/

#include "gl_globals.h"
#include "camera.h"
#include "tools.h"

#include "spheres_instancing.h"

// compiler defined macro
#ifdef SPHERES
#include "spheres_billboard_vbo.h"
#include "spheres_billboard_tbo.h"
#include "spheres_point_sprite.h"
#include "spheres_billboard_geometry_shader.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUMBER_SPHERES 100000
#define RADIUS_MEAN 0.005f
#define RADIUS_VAR 0.06f

#define USE_OPENGL_TIMERS 1
/// after n-th frame the averages are computed and shown.
#define BENCHMARK_FRAME_COUNTER 200
#define FIELD_OF_VIEW 60.0f


//-----------------------------------------------------------------------------
int initGL(int argc, char** argv);

void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void reshape(int w, int h);
float calculate_fps();
//-----------------------------------------------------------------------------
typedef struct
{
  int buttons;
  int x, y;
  int button_x;
  int button_y;
} mouse_state_t;
//-----------------------------------------------------------------------------
bool recompile = false;
Camera camera;
mouse_state_t g_mouse = { 0, 0, 0, 0, 0 };
int width = 800, height = 600;
float fov = FIELD_OF_VIEW;

#ifdef SPHERES
SPHERES<NUMBER_SPHERES> spheres;
#else
SpheresInstancing<NUMBER_SPHERES> spheres;
#endif


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void print_help()
{
  printf("\nKey Mappings:\n ESC\t Exit\n '-'\t reduce FOV by 1.0\n '+'\t increase FOV by 1.0\n"
      " a\t move camera left\n d\t move camera right\n w\t move camera forward\n s\t move camera backward\n"
      " q\t move target of camera up\n e\t move target of camera down\n r\t recompile shader\n\n");
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  if (initGL(argc, argv) != 0)
  {
    fprintf(stderr, "Unable to init OpenGL.");
    return EXIT_FAILURE;
  }

  if (spheres.create(RADIUS_MEAN, RADIUS_VAR) != 0)
  {
    fprintf(stderr, "Unable to create spheres.");
    return EXIT_FAILURE;
  }
  printf("Spheres Renderer Benchmark 2013/04/26 - Update 2016/03/12.\n");
  print_help();

  printf("%s\n", spheres.getDescription().c_str());

  printf("\nAvg1\t\tAvg2\t\tMin1\t\tMin2\t\tMax1\t\tMax2\n");
  glutMainLoop();
  return EXIT_SUCCESS;
}
//-----------------------------------------------------------------------------
// init OpenGL, mostly GLUT-related stuff here
//-----------------------------------------------------------------------------
int initGL(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(width, height);

  glutCreateWindow(spheres.getDescription().c_str());
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  // initialize necessary OpenGL extensions
  glewInit();
  if (!glewIsSupported("GL_VERSION_3_3 "))
  {
    fprintf(stderr, "ERROR: Support for necessary OpenGL extensions missing.");
    fflush(stderr);
    return 1;
  }

  initTools((bool)USE_OPENGL_TIMERS);

  // default initialization
  camera.setupCamera(4.0f, 3.0f, 1.0f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.0);
  camera.applyProjection(fov, width, height);
  camera.apply();

  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_DEPTH_CLAMP);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glClearColor(0.9f,0.9f,0.9f,1.0f);

  if (CHECK_GLERROR() != GL_NO_ERROR)
    return 1;
  return 0;
}
//-----------------------------------------------------------------------------
// Display callback
//-----------------------------------------------------------------------------
void display()
{
#if USE_OPENGL_TIMERS==1
  static uint cbuffer = 0xffff;
  static uint lbuffer = 0;
  static uint last_qindex = 0;
  static double avgt1=0.0, avgt2=0.0;
  static double mint1=999999.0, mint2=999999.0, maxt1=0.0, maxt2=0.0;
  static uint framecounter = BENCHMARK_FRAME_COUNTER;
  double eltime1, eltime2;

  if(cbuffer==0xffff) // first frame
  {
    cbuffer=0;
  }else{
    eltime1 = gpuTimeElapse(lbuffer,last_qindex);
    eltime2 = gpuTimeElapse(lbuffer,last_qindex+1);
    avgt1 += eltime1;
    avgt2 += eltime2;
    if(eltime1<mint1)
      mint1=eltime1;
    if(eltime2<mint2)
      mint2=eltime2;
    if(eltime1>maxt1)
      maxt1=eltime1;
    if(eltime2>maxt2)
      maxt2=eltime2;
    if(--framecounter==0)
    {
      framecounter = BENCHMARK_FRAME_COUNTER;
      printf("%-8.3lf\t%-8.3lf\t%-8.3lf\t%-8.3lf\t%-8.3lf\t%-8.3lf\t\n",
        avgt1/BENCHMARK_FRAME_COUNTER,
        avgt2/BENCHMARK_FRAME_COUNTER,
        mint1, mint2,
        maxt1, maxt2
       );
      avgt1 = 0.0;
      avgt2 = 0.0;
      mint1 = mint2 = 999999.0;
      maxt1 = maxt2 = 0.0;
    }
  }
  // here we start measuring the frame drawing time
  // (will be read into eltime1 next frame)
  gpuTimerStart(cbuffer);
#endif
  if (recompile)
  {
    printf("Recompile...\n");
    if(spheres.recompile()!=0)
      exit(EXIT_FAILURE);

    recompile = false;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glm::vec4 lightPos = camera.modelview_glm() * glm::vec4(1.5,2.5,1.5,0.0);
  // --- SPHERES ---

#if NUMBER_SPHERES>0
#if USE_OPENGL_TIMERS==1
  gpuTimerStart(cbuffer);
#endif

  spheres.bind(&lightPos.x, camera);

#if USE_OPENGL_TIMERS==1
  gpuTimerStop(cbuffer);
#endif

  spheres();

  spheres.unbind();

  if (CHECK_GLERROR() != GL_NO_ERROR)
    exit(1);
#endif

#if USE_OPENGL_TIMERS==1
  last_qindex = gpuTimerStop(cbuffer);
  // swap
  lbuffer = cbuffer;
  cbuffer = cbuffer==0 ? 1 : 0;
#endif
  glutPostRedisplay();
  glutSwapBuffers();

#if USE_OPENGL_TIMERS==0
  char sfps[32];
  sprintf(sfps, "FPS: %.1f", calculate_fps());
  glutSetWindowTitle(sfps);
  if (CHECK_GLERROR() != GL_NO_ERROR)
    exit(1);
#endif
}
//-----------------------------------------------------------------------------
// calculate_fps
//-----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 27:
    exit(0);
  case '+':
    fov -= 1.f;
    camera.applyProjection(fov, width, height);
    break;
  case '-':
    fov += 1.f;
    camera.applyProjection(fov, width, height);
    break;
  case 'a':
    camera.strafeCamera(0.01f);
    break;
  case 'd':
    camera.strafeCamera(-0.01f);
    break;
  case 'w':
    camera.distCamera(0.02f);
    break;
  case 's':
    camera.distCamera(-0.02f);
    break;
  case 'q':
    camera.advanceTargetZ(0.05f);
    break;
  case 'e':
    camera.advanceTargetZ(-0.05f);
    break;
  case 'r':
    recompile = true;
    break;
  }
  camera.apply();
}
//-----------------------------------------------------------------------------
// mouse callback
//-----------------------------------------------------------------------------
void mouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN)
  {
    g_mouse.buttons |= 1 << button;
  }
  else if (state == GLUT_UP)
  {
    g_mouse.buttons = 0;
  }

  g_mouse.button_x = x;
  g_mouse.button_y = y;
}
//-----------------------------------------------------------------------------
// mouse motion callback
//-----------------------------------------------------------------------------
void motion(int x, int y)
{
  float dx, dy;
  dx = (float) (x - g_mouse.button_x);
  dy = (float) (y - g_mouse.button_y);

  if (g_mouse.buttons & 1)
  {
    camera.rotatePosition(0.001f * dx);
    camera.advancePositionZ(-0.002f * dy);
  }
  else if (g_mouse.buttons & 4)
  {
    camera.distCamera(0.001f * dy);
    camera.rotate(0.0005f * dx);
  }

  g_mouse.button_x = x;
  g_mouse.button_y = y;
  camera.apply();
}
//-----------------------------------------------------------------------------
// window reshape callback
//-----------------------------------------------------------------------------
void reshape(int w, int h)
{
  width = w;
  height = h;
  glViewport(0, 0, width, height);
  camera.applyProjection(fov, width, height);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
float calculate_fps()
{
  static int frame_count = 0;
  static int current_time = 0, previous_time = 0;
  static float fps = 0;
  int time_interval = 0;
  //  Increase frame count
  frame_count++;

  //  Get the number of milliseconds since glutInit called
  //  (or first call to glutGet(GLUT ELAPSED TIME)).
  current_time = glutGet(GLUT_ELAPSED_TIME);

  //  Calculate time passed
  //int time_interval = current_time - previous_time;
  time_interval = current_time - previous_time;

  if (time_interval > 1000)
  {
    //  calculate the number of frames per second
    fps = frame_count / (time_interval / 1000.0f);

    //  Set time
    previous_time = current_time;

    //  Reset frame count
    frame_count = 0;
  }
  return fps;
}
