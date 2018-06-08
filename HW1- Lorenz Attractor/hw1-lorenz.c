/*
 *  Justin Chin CSCI 5229 Computer Graphics Summer 2018
 *
 *  Display Lorenz Attractor
 *
 *  Key bindings:
 *  1      2D coordinates
 *  2      2D coordinates with fixed Z value
 *  3      3D coordinates
 *  4      4D coordinates
 *  +/-    Increase/decrease z or w
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC    Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Globals
int th=0;       // Azimuth of view angle
int ph=0;       // Elevation of view angle
int mode=1;     // Dimension (1-4)
double dim=100;   // Dimension of orthogonal box
char* text[] = {"","2D","3D constant Z","3D","4D"};  // Dimension display text

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

int i;


/*  Intial Coordinates  */
double x = 1;
double y = 1;
double z = 1;
/*  Time step  */
double dt = 0.001;

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Display the scene
 */
void display()
{  
   double initx, inity, initz;
   printf("%lf %lf %lf ",x,y,z);
   initx=x;
   inity=y;
   initz=z;

   printf("%lf %lf %lf ",initx,inity,initz);
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();
   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   
   //  Draw 10 pixel yellow points
   glColor3f(1,1,0);
   glPointSize(5);
   glBegin(GL_POINTS);
   switch (mode)
   {
   //  Two dimensions
   case 2:
      glVertex2d(0.1,0.1);
      glVertex2d(0.3,0.3);
      glVertex2d(0.5,0.5);
      glVertex2d(0.7,0.7);
      glVertex2d(0.9,0.9);
      break;
   //  Three dimensions - constant Z
   case 1:
	   /*
	    *  Integrate 50,000 steps (50 time units with dt = 0.001)
	    *  Explicit Euler integration
	    */
	   glVertex3d(x,y,z);
	   for (i=0;i<50000;i++)
	   {
	      double dx = s*(y-x);
	      double dy = x*(r-z)-y;
	      double dz = x*y - b*z;
	      x += dt*dx;
	      y += dt*dy;
	      z += dt*dz;
	      glVertex3d(x,y,z);
	   }	

      //Reset the original coordinates
      x=initx;
      y=inity;
      z=initz;

      break;
   //  Three dimensions - variable Z
   case 3:
      glVertex3d(0.1,0.1,0.1);
      glVertex3d(0.3,0.3,0.2);
      glVertex3d(0.5,0.5,0.4);
      glVertex3d(0.7,0.7,0.6);
      glVertex3d(0.9,0.9,0.9);
      break;
   //  Four dimensions
   case 4:
      glVertex3d(0.1,0.1,0.1);
      glVertex3d(0.3,0.3,0.2);
      glVertex3d(0.5,0.5,0.4);
      glVertex3d(0.7,0.7,0.6);
      glVertex3d(0.9,0.9,0.9);
      break;
   }
   glEnd();
   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(90,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,90,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,90);
   glEnd();
   //  Label axes
   glRasterPos3d(90,0,0);
   Print("X");
   glRasterPos3d(0,90,0);
   Print("Y");
   glRasterPos3d(0,0,90);
   Print("Z");
   //  Display parameters
   glWindowPos2i(5,5);
   Print("View Angle=%d,%d  %s \n x:%lf, y:%lf, z:%lf",th,ph,text[mode],x,y,z);
   //  Flush and swap
   glFlush();
   glutSwapBuffers();


}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Increase w by 0.1
   else if (ch == '+')
   {
      //What to do when + pressed
   }
   //  Decrease w by 0.1
   else if (ch == '-')
   {
      //What to do when 0 pressed
   }
   else{
      //Unassigned key pressed, so dont Redisplay
      return;
   }
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   else{
      //undefined special key pressed, dont redisplay
      return;
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Justin Chin HW 1");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
  //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
