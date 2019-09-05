/*
|--------------------------------------------------------------------------
| Strange Attractions
|--------------------------------------------------------------------------
|
| School of Computer Science
| The University of Manchester
|
| This code is licensed under the terms of the Creative Commons
| Attribution 2.0 Generic (CC BY 3.0) License.
|
| Skeleton code for COMP37111 coursework, 2013-14
|
| Authors: Struan McDonough, Arturs Bekasovs, Toby Howard
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <GL/glut.h>

#define NUM_PARTICLES 1000

#define RUN_SPEED  10
#define TURN_ANGLE 4.0
#define DEG_TO_RAD 0.017453293

GLdouble lat,     lon;              /* View angles (degrees)    */
GLdouble mlat,    mlon;             /* Mouse look offset angles */
GLfloat  eyex,    eyey,    eyez;    /* Eye point                */
GLfloat  centerx, centery, centerz; /* Look point               */
GLfloat  upx,     upy,     upz;     /* View up vector           */
GLfloat gravity=0.002;
GLfloat particle_size=1;
GLint randcolor = 1;
GLint addSource;
GLint sourcenum;
GLchar title[50];
GLint frame,   currenttime, timebase;
// particle stuffs
typedef struct
{
int id;
float lifetime;                       // total lifetime of the particle
float decay;                          // decay speed of the particle
float r, g, b, a;                     // color values of the particle
float xpos, ypos, zpos;               // position of the particle
float xspeed, yspeed, zspeed;         // speed of the particle
int active;                          // is particle active or not?
int    size;
} Particle;

// file scope particle pointer and properties
int populator = 1;
Particle Particles[NUM_PARTICLES];      // Particle Array (Room For Particle Info)
float X_SPEED = 1;
float Y_SPEED = 3;
float Z_SPEED = 1;
const float X_START_POS = 0.0f;
const float Y_START_POS = 0.0f;
const float Z_START_POS = 0.0f;

// file scope particle forces and accelerations
float xfriction = 1.0f;
float yfriction = 1.0f;
float zfriction = 1.0f;
float xaccel = 0.0f;
float yaccel = -0.3f;
float zaccel = 0.0f;

// frames per second
const unsigned int GLUT_TIMER_DELAY = 16; // ~60 fps



///////////////////////////////////////////////

double myRandom()
//Return random double within range [0,1]
{
  return (rand()/(double)RAND_MAX);
}

///////////////////////////////////////////////

void DrawGround()
{
  int x, z;
  int L= 150;
  // the ground quad and the grid lines are co-planar, which would lead to horrible Z-fighting,
  // so we resort to 2 hacks. First, fiddle with the Z-buffer depth range, using glDepthRange(),
  // and second, draw the lines 0.01 higher in Y than the ground plane
  glColor3f(0.4, 0.4, 0.4);
  glBegin(GL_QUADS);
  glVertex3f(-L, 0, -L);
  glVertex3f(L, 0, -L);
  glVertex3f(L, 0, L);
  glVertex3f(-L, 0, L);
  glEnd();

  glColor3f(0.2, 0.2, 0.2);
  glLineWidth(1.0);
  glBegin(GL_LINES);
  for (x= -L; x <= L; x+=10)  {
    glVertex3f((GLfloat) x, 0.1, -L);
    glVertex3f((GLfloat) x, 0.1,  L);
  }
  for (z= -L; z <= L; z+=10)  {
    glVertex3f(-L, 0.1, (GLfloat) z);
    glVertex3f( L, 0.1, (GLfloat) z);
  }
  glEnd();

}

///////////////////////////////////////////////
void Timer(int value)
{
  glutPostRedisplay();
  glutTimerFunc(GLUT_TIMER_DELAY, Timer, 0);
}

///////////////////////////////////////////////
void calculate_lookpoint(void) { /* Given an eyepoint and latitude and longitude angles, will
     compute a look point one unit away */
    centerx = eyex + cos((lat+mlat)*DEG_TO_RAD)*sin((lon+mlon)*DEG_TO_RAD);
    centery = eyey+ sin((lat+mlat)*DEG_TO_RAD);
    centerz = eyez + cos((lat+mlat)*DEG_TO_RAD)*cos((lon+mlon)*DEG_TO_RAD);
  /* To be completed */

} // calculate_lookpoint()
//


void display()
{
  glLoadIdentity();
  calculate_lookpoint();
  gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
  glClear(GL_COLOR_BUFFER_BIT);
  DrawGround();


  glPushMatrix(); // objects-in-front-of-camera transformations
  {
    glPushMatrix(); // actor transformations
    {
      int i;
      for (i = 0 ; i < NUM_PARTICLES; i++ )
      {

        // only draw particle if it is active
        if ( Particles[i].active == 1 )
        {

        //  draw particle


          // glColor4f(Particles[i].r, Particles[i].g, Particles[i].b, Particles[i].a);
          // glTranslatef(Particles[i].xpos, Particles[i].ypos, Particles[i].zpos);
          // glutSolidSphere(Particles[i].size/2, 10, 2);


          // glBegin(GL_POINTS); {
          //   glVertex3f(Particles[i].xpos, Particles[i].ypos, Particles[i].zpos);
          //   glColor4f(Particles[i].r, Particles[i].g, Particles[i].b, Particles[i].a);
          // } glEnd();


          GLUquadric *myQuad;
          float d =  (float)(rand() % 200+100)/100 - particle_size;

          myQuad = gluNewQuadric();
          glColor4f(Particles[i].r, Particles[i].g, Particles[i].b, Particles[i].a);
          glTranslatef(Particles[i].xpos, Particles[i].ypos, Particles[i].zpos);
          gluSphere(myQuad,d,10,4);
          glTranslatef(-Particles[i].xpos, -Particles[i].ypos, -Particles[i].zpos);



          // decrement particle life time
          Particles[i].lifetime -= Particles[i].decay;
          //printf("Particles lifetime: %f \n", Particles[i].lifetime);

          // deactivate particle if it is past its lifetime
          if ( Particles[i].lifetime <= 0 )
            Particles[i].active = 0;
          else
          {
            // increment speeds from accelerations
            Particles[i].xspeed += xaccel;
            Particles[i].yspeed += -gravity * (float)(rand() % 150 + 100);
            Particles[i].zspeed += zaccel;

            // decrement speeds from friction
            Particles[i].xspeed /= xfriction;
            Particles[i].yspeed /= yfriction;
            Particles[i].zspeed /= zfriction;

            // increment positions from speeds
            Particles[i].xpos += Particles[i].xspeed;
            Particles[i].ypos += Particles[i].yspeed;
            Particles[i].zpos += Particles[i].zspeed;

            if ( Particles[i].ypos < -0.5f )
              Particles[i].active = 0;
          }
        }
        else
        {
          // re-activate particle!
          //srand( (int)(Particles[i].ypos + Particles[i].xpos + Particles[i].zpos + Particles[i].lifetime) + (int)(time(NULL) % NUM_PARTICLES) );
          Particles[i].lifetime = (float)(rand() % 40 + 100);
          Particles[i].decay = 1.0f;
          //Particles[i].r = ((double) rand() / (RAND_MAX)) + 1;
          //0, 204, 255
          if(randcolor ==1)
          {
            Particles[i].r = 0;
            Particles[i].g = 204;
            Particles[i].b = 255;
          }
          else
          {
            Particles[i].r = ((float)(rand() % 100)/100);
            Particles[i].g = ((float)(rand() % 100)/100);
            Particles[i].b = ((float)(rand() % 100)/100);
          }
          Particles[i].a = 1.0f;
          if(addSource==0)
          {
            Particles[i].xpos = X_START_POS;
            Particles[i].ypos = Y_START_POS;
            Particles[i].zpos = Z_START_POS;
          }
          else
          {
            Particles[i].xpos = (i%2)*100;
            Particles[i].ypos = Y_START_POS;
            Particles[i].zpos = (i%2)*100;
          }



          Particles[i].xspeed = 0.03 * (float)(rand() % 100 - 50);
          Particles[i].yspeed = 2 + (1.3*(float)(rand() % 3 + 3));
          Particles[i].zspeed = 0.03 * (float)(rand() % 100 - 50);
          Particles[i].active = 1;
        }
      }
    }

    glPopMatrix(); // end actor transformations
  }
  glPopMatrix(); // end objects-in-front-of-camera transformations
  //calculate the frames per second
  frame++;

  //get the current time
  currenttime = glutGet(GLUT_ELAPSED_TIME);

  //check if a second has passed
  if (currenttime - timebase > 1000)
  {
    sprintf(title, "COMP37111 Particles FPS: %4.2f",	frame*1000.0/(currenttime-timebase));
    glutSetWindowTitle(title);
    timebase = currenttime;
    frame = 0;
  }
  glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'z':
      gravity = gravity+0.0003;
      break;
    case 'x':
      gravity = gravity-0.0003;
      break;
    case 'c':
      particle_size = particle_size+0.03;
      break;
    case 'v':
      particle_size = particle_size-0.03;
      break;
    case 'b':
      if(randcolor ==0)
      randcolor = 1;
      else
      randcolor = 0;
      break;
    case '=':
      if(addSource ==0)
      addSource = 1;
      else
      addSource = 0;
      break;
    }
  glutPostRedisplay();
}

void cursor_keys(int key, int x, int y) {
  switch (key) {
    case  GLUT_KEY_LEFT :
      lon = lon + TURN_ANGLE;
      if(lon<-90)
        lon = -90;
      if(lon>90)
        lon = 90;
      break;
    case  GLUT_KEY_RIGHT :
      lon = lon - TURN_ANGLE;
      if(lon<-90)
        lon = -90;
      if(lon>90)
        lon = 90;
      break;
    case  GLUT_KEY_PAGE_UP :
      lat = lat + TURN_ANGLE;
      if(lat<-90)
        lat = -90;
      if(lat>90)
        lat = 90;
      break;
    case  GLUT_KEY_PAGE_DOWN :
      lat = lat - TURN_ANGLE;
      if(lat<-90)
        lat = -90;
      if(lat>90)
        lat = 90;
      break;
    case  GLUT_KEY_HOME :
      lon = 0;
      break;
    case GLUT_KEY_UP :
      eyex += RUN_SPEED * sin(lon*DEG_TO_RAD);
      eyez += RUN_SPEED * cos(lon*DEG_TO_RAD);
      break;
    case GLUT_KEY_DOWN :
      eyex -= RUN_SPEED * sin(lon*DEG_TO_RAD);
      eyez -= RUN_SPEED * cos(lon*DEG_TO_RAD);
      break;

      /* To be completed */
  }
} // cursor_keys()

///////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0, 0, 0, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 10000.0);
  glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  eyex=  0.0; /* Set eyepoint at eye height within the scene */
  eyey=  100;
  eyez=   -300.0;

  upx= 0.0;   /* Set up direction to the +Y axis  */
  upy= 1.0;
  upz= 0.0;

  lat= 0.0;   /* Look horizontally ...  */
  lon= 0.0;   /* ... along the +Z axis  */

  mlat= 0.0;  /* Zero mouse look angles */
  mlon= 0.0;
  glutInit(&argc, argv);
  glutInitWindowSize(1280, 720);
  glutInitWindowPosition(100, 100);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("COMP37111 Particles");
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  glutTimerFunc(GLUT_TIMER_DELAY, Timer, 0);

  // set opengl point size for particle system
  glPointSize(4.0f);

  // enable blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // antialiasing method (may work better for points than multisampling)
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  // glEnable(GL_LIGHTING);
  glutKeyboardFunc (keyboard);
  glutSpecialFunc (cursor_keys);
  glutMainLoop();

  return 0;
}
