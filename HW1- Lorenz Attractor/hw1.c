/*
 *  Justin Chin CSCI 5229 Computer Graphics Summer 2018
 *
 *  Display Lorenz Attractor
 *
 *  Key bindings:
 *  1      (1) Lorenz Attractor
 *  2      (2) Lorenz Attractors w/ modifable 2nd Lorenz initial conditions
 *  q/w    Decrease/Increase x intial coordinate for 2nd Lorenz Attractor 
 *  a/s    Decrease/Increase y intial coordinate for 2nd Lorenz Attractor 
 *  z/x    Decrease/Increase z intial coordinate for 2nd Lorenz Attractor
 *  o/p    Decrease/Increase s intial coordinate for 2nd Lorenz Attractor
 *  k/l    Decrease/Increase b intial coordinate for 2nd Lorenz Attractor
 *  n/m    Decrease/Increase r intial coordinate for 2nd Lorenz Attractor
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
int mode=1;     // Dimension (1-2)
double dim=100;   // Dimension of orthogonal box
char* text[] = {"","(1) Lorenz Attractor","(2) Lorenz Attractors w/ modifable 2nd Lorenz initial conditions"};  // Dimension display text

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

/*  Intial Coordinates  */
double x = 1;
double y = 1;
double z = 1;

/* Lorenz Calculation Time step  */
double dt = 0.001;

double x2 = 3;
double y2 = 3;
double z2 = 3;
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

void lorenz(double x, double y, double z,double s,double b ,double r){
   /*
   *  Integrate 50,000 steps (50 time units with dt = 0.001)
   *  Explicit Euler integration
   */
   glVertex3d(x,y,z);   
   for (int i=0;i<50000;i++)
   {
      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      glVertex3d(x,y,z);
   }  
}

/*
 *  Display the scene
 */
void display()
{  
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();
   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   
   glBegin(GL_LINE_STRIP);
   switch (mode)
   {
   //  One Lorenz
   case 1:
      glColor3f(1,1,0);
	   //call lorenz function to generate vertices
      lorenz(x,y,z,s,b,r);
      
      break;
   //  Two Lorenzs
   case 2:
      //First Lorenz
      glColor3f(1,1,0);
      lorenz(x,y,z,s,b,r);
      
      //Second Lorenz
      glColor3f(1,0,0);
      lorenz(x2,y2,z2,s,b,r);
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
   
   if(mode == 1){
      glWindowPos2i(5,45);
      Print("View Angle=%d,%d",th,ph);
      glWindowPos2i(5,25);
      Print("Lorenz Attractor 1 Initial Coordinates: x:%.1lf, y:%.1lf, z:%.1lf",x,y,z);
      glWindowPos2i(5,5);
      Print("Lorenz Params: s:%.1lf b:%.1lf r:%.1lf ",s,b,r);
   }   
   else if (mode == 2){
      glWindowPos2i(5,65);
      Print("View Angle=%d,%d",th,ph);
      glWindowPos2i(5,45);
      Print("Lorenz Attractor 1 Initial Coordinates: x:%.1lf, y:%.1lf, z:%.1lf",x,y,z);
      glWindowPos2i(5,25);
      Print("Lorenz Attractor 2 Initial Coordinates: x:%.1lf, y:%.1lf, z:%.1lf",x2,y2,z2);
      glWindowPos2i(5,5);
      Print("Lorenz Params: s:%.1lf b:%.1lf r:%.1lf ",s,b,r);
   }
     
   // Title Box
   glWindowPos2i(5,750);
   Print("%s",text[mode]);
   
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
   //Swap mode 1
   else if (ch == '1')
      mode = 1;
   //Swap mode 2
   else if (ch == '2')
      mode = 2;
   // Change Lorenz params 
   else if (ch == 'p')
   {
      s += 1;
   }
   else if (ch == 'o')
   {
      s -= 1;
   }
   else if (ch == 'l')
   {
      b += 1;
   }
   else if (ch == 'k')
   {
      b -= 1;
   }
   else if (ch == 'm')
   {
      r += 1;
   }
   else if (ch == 'n')
   {
      r -= 1;
   }
   //Control second Lorenz starting coordinates
   else if (ch == 'w' && mode == 2)
   {
      x2 += 1;
   }
   else if (ch == 'q' && mode == 2)
   {
      x2 -= 1;
   }
   else if (ch == 's' && mode == 2)
   {
      y2 += 1;
   }
   else if (ch == 'a' && mode == 2)
   {
      y2 -= 1;
   }
   else if (ch == 'x' && mode == 2)
   {
      z2 += 1;
   }
   else if (ch == 'z' && mode == 2)
   {
      z2 -= 1;
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
   //  Request 800 x 800 pixel window
   glutInitWindowSize(800,800);
   //  Create the window
   glutCreateWindow("Assignment 1:Justin Chin");
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
