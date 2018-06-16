/*
 *  Projections
 *
 *  Draws stars in the sky
 	1st (orthogonal) view shows overhead view
 	2nd perspective view shows overhead view
 	3rd perspective first person view on the ground
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */

#include <stdio.h>
//Enable booleans
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int axes=1;       //  Display axes
int mode=0;       //  Projection mode

// View angles for Overhead view (View 1,2)
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio

//Overhead Size of world control
double dim=5.0;   //  Size of world


double zh=0;      //  Rotation variable of stars


char* modeTitles[]= {"Orthogonal","Perspective","FPS"}; 

//FPS Parameters
//Rotation of FPS 
int th_fps=0;     //  Azimuth of view angle
int ph_fps=0;     //  Elevation of view angle

double head_height = 0.2; //Position of eye level

//Eye Position
double Ex = 0;
double Ey = 0;
double Ez = 0;

//Camera Look at Position
double Cx = 0;
double Cy = 0;
double Cz = 1;

//Delta Position (distance moving to )
double dx = 0;
double dy = 0;
double dz = 0;

bool left_right = false;
double rotationSpeed =2;

typedef struct {double x,y,z,zhSpinRate;} StarPos;
StarPos StarPosArr[] = {
   {1, 2.4, -1,10},
   {2, 2.4, -2,10},
   {2, 2.4, -5,10},
   {4, 2.4, -6,10},
   {6, 2.4, -8,10},
   {4, 2.4, -1.5,10},

   {-2.2, 2.4, -.2,8},
   {-3.2, 2.4, 1.2,20},
   {3.1, 2.4, -.5,-2},
   };


//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
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
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation  
   switch (mode){
   		//Mode 0 ==  Orthogonal projection
   		case 0:
	   		glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
	   		break;
    	//Mode 1 == PerspectiveView
   		case 1:
     		gluPerspective(fov,asp,dim/4,4*dim);
     		break;		
      	case 2:
     		gluPerspective(fov,asp,dim/4,4*dim);
     		break;
   }
   
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor4f(1,0,0,.1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor4f(0,0,1,.1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor4f(1,1,0,.1);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor4f(0,1,0,.1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor4f(0,1,1,.1);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor4f(1,0,1,.1);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}


static void star(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   // top-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 1.0, 1.0, 0.0);
   glVertex3f( 0.0, 3.0, 0.0);
   glEnd();
   // top-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 0.0, 3.0, 0.0);
   glVertex3f( -1.0, 1.0, 0.0);
   glEnd();
   // topright-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 2.9, 0.9, 0.0);
   glVertex3f( 1.0, 1.0, 0.0);
   glEnd();
   // topright-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 1.5, -0.5, 0.0);
   glVertex3f( 2.9, 0.9, 0.0);
   glEnd();
   
   // bottomright-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 1.8, -2.7, 0.0);
   glVertex3f( 1.5, -0.5, 0.0);
   glEnd();
   // bottomright-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 0.0, -1.0, 0.0);
   glVertex3f( 1.8, -2.7, 0.0);
   glEnd();


   // bottomleft-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( -1.8, -2.7, 0.0);
   glVertex3f( 0.0, -1.0, 0.0);
   glEnd();
   // bottomleft-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( -1.5, -0.5, 0.0);
   glVertex3f( -1.8, -2.7, 0.0);
   glEnd();

   // topleft-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( -2.9, 0.9, 0.0);
   glVertex3f( -1.5, -0.5, 0.0);
   glEnd();
   // topleft-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( -2.9, 0.9, 0.0);
   glVertex3f( -1.0, 1.0, 0.0);
   
   //Backside
   // top-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( 0.0, 3.0, 0.0);
   glVertex3f( 1.0, 1.0, 0.0);
   
   glEnd();
   // top-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( -1.0, 1.0, 0.0);
   glVertex3f( 0.0, 3.0, 0.0);
   glEnd();
   // topright-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( 1.0, 1.0, 0.0);
   glVertex3f( 2.9, 0.9, 0.0);
   glEnd();
   // topright-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( 2.9, 0.9, 0.0);
   glVertex3f( 1.5, -0.5, 0.0);
   glEnd();
   
   // bottomright-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( 1.5, -0.5, 0.0);
   glVertex3f( 1.8, -2.7, 0.0);
   glEnd();
   // bottomright-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( 1.8, -2.7, 0.0);
   glVertex3f( 0.0, -1.0, 0.0);
   glEnd();


   // bottomleft-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( 0.0, -1.0, 0.0);
   glVertex3f( -1.8, -2.7, 0.0);
   glEnd();
   // bottomleft-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( -1.8, -2.7, 0.0);
   glVertex3f( -1.5, -0.5, 0.0);
   glEnd();

   // topleft-left
   glColor3f(1,1,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( -1.5, -0.5, 0.0);
   glVertex3f( -2.9, 0.9, 0.0);
   glEnd();
   // topleft-right
   glColor3f(1,0,0);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, -1);
   glVertex3f( -1.0, 1.0, 0.0);
   glVertex3f( -2.9, 0.9, 0.0);
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

//View 2 Perspective - Fixed position Eye position (orbiting around) the center (object position == orgin)
void PerspectiveEyeCalculation()
{
	double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
}

void FPSEyeCalculation()
{
    //Move Left or Right, just update the camera look at postion
    if(left_right){
    	//update rotation around the eye position
		th_fps -= dx*rotationSpeed;
		//update look at position
		Cx = Ex+Sin(th_fps); 
		Cy = Ey;
		Cz = Ez+Cos(th_fps);
		//return back the state
		left_right = false;
	}
	else if ( dx ==0 && dy==0 && dz == 0){
		// no translation then continue
	}
	else{
		//New Eye position
		Ex += dx;
		Ez += dz;

		//Update position being looked at
		Cx = Ex+Sin(th_fps); 
		Cy = Ey;
		Cz = Ez+Cos(th_fps);
	}
    
    gluLookAt(Ex,Ey,Ez, Cx,Cy,Cz , 0,1,0);
    
    //Reset dx,dy,dz so it doesnt continuously calcualte new movement on next call
    dx=0;
    dy=0;
    dz=0;

}

void GenerateStarMatrix(){

}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   int i,j,k;
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   switch (mode){
   		//Mode 0 Orthogonal
   		case 0:
   			//Move left/right
			if(left_right){
				th_fps += dx;
				Cx = Ex+Sin(th_fps); 
				Cy = Ey;
				Cz = Ez+Cos(th_fps);
				left_right = false;
			}
			else if ( dx ==0 && dy==0 && dz == 0){
				
			}
			else{
				
				//double Ex = -2*dim*Sin(th_fps)*Cos(ph_fps);
				//double Ey = +2*dim        *Sin(ph_fps);

				//New Eye position
				Ex += dx;
				//Ey += .1*dy;
				Ez += dz;

				Cx = Ex+Sin(th_fps); 
				Cy = Ey;
				Cz = Ez+Cos(th_fps);
			}
		    
			dx=0;
			dy=0;
			dz=0;

   			glRotatef(ph,1,0,0);
      		glRotatef(th,0,1,0);
      		break;
   		//Mode 1 == Perspective Orbital View	
   		case 1:
			PerspectiveEyeCalculation();
			break;
      	case 2:
      		//Mode 2 FPS POV 	
     		FPSEyeCalculation();
     		//glTranslated(-left,.2,-fwd);
     		//glRotatef(th_fps,0,1.0f,0); 
     		   
     		break;
   }

   
	cube(Ex,Ey,Ez,.1,.1,.1,th_fps);
	glColor3f(0,1,0);
    glBegin(GL_LINES);
    glVertex3f(Ex,Ey,Ez);
    glVertex3f(Cx,Cy,Cz);
    glEnd();
   

   //  Draw stars
   for (i=0;i<=8;i++) {
   		//double x = rand() % (int)(dim/2 - (-dim/2)) + (-dim/2);
   		//double z = rand() % (int)(dim/2 - (-dim/2)) + (-dim/2);
   		//double y = rand() % (int)(dim*.8 - dim*.7) + (dim*.7);
   		//printf("%f",z);
   		//printf("%f",x);
   		star(StarPosArr[i].x,StarPosArr[i].y,StarPosArr[i].z , 0.2,0.2,0.2 , zh*StarPosArr[i].zhSpinRate);
   }

  for (i=-1;i<=1;i++)
  	for (j=-1;j<=1;j++)
    	for (k=-1;k<=1;k++)
        	cube(i,j,k , 0.1,0.1,0.1 , 0);




   //  Draw axes
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();

      	//Grid surface
      glBegin(GL_LINES);	
      for(i=-dim/2;i<=dim/2;i++){
  		glVertex3d(i,0.0,-dim/2);
		glVertex3d(i,0.0,dim/2);
      }
		
      for(i=-dim/2;i<=dim/2;i++){
		glVertex3d(-dim/2,0.0,i);
		glVertex3d(dim/2,0.0,i);
	  }		
      glEnd();

      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   	
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Mode=%d, Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s th_fps=%d ph_fps=%d x_pos=%f y_pos=%f",mode,th,ph,dim,fov,modeTitles[mode],th_fps,ph_fps,Ex,Ez);
   //  Render the scene and make it visible
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
	//Handle POV for non FPS mode
	if(mode != 2){
	   //  Right arrow key - increase angle by 5 degrees
	   if (key == GLUT_KEY_RIGHT)
	      th += 5;
	   //  Left arrow key - decrease angle by 5 degrees
	   else if (key == GLUT_KEY_LEFT)
	      th -= 5;
	   //  Up arrow key - increase elevation by 5 degrees
	   else if (key == GLUT_KEY_UP)
	      ph += 5;
	   //  Down arrow key - decrease elevation by 5 degrees
	   else if (key == GLUT_KEY_DOWN)
	      ph -= 5;
	   
	}
	else {
		//  Right arrow key - increase angle by 5 degrees
	   if (key == GLUT_KEY_RIGHT)
	      th_fps += 5;
	   //  Left arrow key - decrease angle by 5 degrees
	   else if (key == GLUT_KEY_LEFT)
	      th_fps -= 5;
	   //  Up arrow key - increase elevation by 5 degrees
	   else if (key == GLUT_KEY_UP)
	      ph_fps += 5;
	   //  Down arrow key - decrease elevation by 5 degrees
	   else if (key == GLUT_KEY_DOWN)
	      ph_fps -= 5;
	}		
   //  PageUp key - increase dim
   if (key == GLUT_KEY_PAGE_UP)
   		dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   th_fps %= 360;
   ph_fps %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
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
   //  Toggle axes
   else if (ch == '1')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = (mode+1) % 3;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else

   //Mode 2 == FPS for WASD movement controls
   //if(mode == 2){
   if (ch == 'w'){
      dx = 1*Sin(th_fps);
      dy = 0;
      dz = 1*Cos(th_fps);
   }
   //  back
   else if (ch == 's') {
 	  dx = -1*Sin(th_fps);
      dy = 0;
      dz = -1*Cos(th_fps);
   }
   //  Toggle axes
   else if (ch == 'a'){
      left_right = true;
      dx = -1;
      dy = 0;
      dz = 0;
   }
   else if (ch == 'd') {
      left_right = true;
      dx = 1;
      dy = 0;
      dz = 0;
    }	
   //}

   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  GLUT calls this toutine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Projections");

   GenerateStarMatrix();
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   //  Tell GLUT to call "idle" when there is nothing else to do
   glutIdleFunc(idle);
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}