/*
 *  HW 3: Lighting and textures
 *
 * Draws a scene of a fish tank with colored star fish on the bottom
   View 1 shows a perspective view with a point light orbiting automatically
   View 2 shows a perspective view allowing the user to control the point light with
   (w,a,s,d,q,e) keys

   Users can also click and drag the scene left,right,forward,and back using the left click on the mouse.
   Raising and lowering the scene can be done by right clicking and dragging up and down on the scene.
 *
 *  Key bindings:
 *  `          Toggles on/off the text, axis, and normal vectors pointing out of star surface 
 *  arrows     Change view angle in View 1 and 2
   w           Moves light forward
   a           Moves light back
   s           Moves light left 
   d           Moves light right
 *  g          Generates another 10 randomly placed and randomly colored stars
 *  x          Turns on and off the axis 
 *  m          Toggles between automatic light view to user controlled light position
 *  +/-        Change field of view of perspective
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle back to 0,0
 *  ESC        Exit
 */

#include "CSCIx229.h"
#include "MatrixFunc.h"
#include "RandFunc.h"

int axes=1;       //  Display axes
int mode=0;       //  mode 2 == move light manually
double inc = 5;

//mouse control
double Ox=0,Oy=0,Oz=0; //  LookAt Location
int move=0;            //  Move mode
int X,Y;               //  Last mouse location
int light = 1;
int th=0;         //  Azimuth of view angle
int ph=45;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world

// Light values
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  20;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light


//Debug mode flag
int debugMode = 1;

// Light deltaPosition
double Lx,Ly,Lz;
double dx,dy,dz;
double moveSpeed = 0.2;

//glass mat
double alpha = 100;
double tankx = 2.0;
double tanky = 1.0;
double tankz = 2.0;
int tankNumPoly = 20;

// Textures
unsigned int texture[3]; // Texture names

char* modeTitles[]= {"Light Auto Rotate","Light User Control"}; 

//Stars
double numStars = 12; 

typedef struct {double x,y,z,sx,sy,sz,th,r,g,b;} StarPos;
StarPos *StarPosArr;

typedef struct {double x,y,z;} vertexPos;
double vertexPosArr[20][9] = {
   {0.0,0.0,1.0,1.0, 1.0, 0.0,0.0, 3.0, 0.0  },
   {0.0, 0.0, 1.0, 0.0, 3.0, 0.0, -1.0, 1.0, 0.0 },
   {0.0, 0.0, 1.0, 1.5, -0.5, 0.0, 2.9, 0.9, 0.0},
   { 0.0, 0.0, 1.0, 2.9, 0.9, 0.0, 1.0, 1.0, 0.0 },
   {0.0, 0.0, 1.0, 0.0, -1.0, 0.0, 1.8, -2.7, 0.0},   
   {0.0, 0.0, 1.0, 1.8, -2.7, 0.0, 1.5, -0.5, 0.0},
   {0.0, 0.0, 1.0, -1.5, -0.5, 0.0, -1.8, -2.7, 0.0},
   {0.0, 0.0, 1.0, -1.8, -2.7, 0.0, 0.0, -1.0, 0.0},
   {0.0, 0.0, 1.0, -1.0, 1.0, 0.0,-2.9, 0.9, 0.0},
   {0.0, 0.0, 1.0, -2.9, 0.9, 0.0, -1.5, -0.5, 0.0},
   {0.0, 0.0, -1.0, -1.0, 1.0, 0.0, 0.0, 3.0, 0.0},
   {0.0, 0.0, -1.0, 0.0, 3.0, 0.0, 1.0, 1.0, 0.0},
   {0.0, 0.0, -1.0, 1.0, 1.0, 0.0, 2.9, 0.9, 0.0},
   {0.0, 0.0, -1.0, 2.9, 0.9, 0.0, 1.5, -0.5, 0.0},
   {0.0, 0.0, -1.0, 1.5, -0.5, 0.0, 1.8, -2.7, 0.0},
   {0.0, 0.0, -1.0, 1.8, -2.7, 0.0, 0.0, -1.0, 0.0},
   {0.0, 0.0, -1.0, 0.0, -1.0, 0.0, -1.8, -2.7, 0.0},
   {0.0, 0.0, -1.0, -1.8, -2.7, 0.0, -1.5, -0.5, 0.0},
   {0.0, 0.0, -1.0, -1.5, -0.5, 0.0, -2.9, 0.9, 0.0},
   {0.0, 0.0, -1.0, -2.9, 0.9, 0.0, -1.0, 1.0, 0.0}
   
};



void GenerateStarMatrix(){
   StarPosArr = malloc(numStars*sizeof(StarPos));
   for(int i=0; i<numStars; i++) {
      double negx = -tankx*.95;
      double posx = tankx*.95;
      double x = randDouble(negx ,posx);
      double y = .1;
      double z = randDouble(negx ,posx);
      double sx = randDouble(.05,.1);  
      double rot = randDouble(0,360);
      double r = randDouble(0,1);
      double g = randDouble(0,1);
      double b = randDouble(0,1);
      StarPos pos ={x,y,z,sx,sx,sx,rot,r,g,b};
      StarPosArr[i] = pos;
   }
}

void UpdateLightPosition(){
   if ( dx ==0 && dy==0 && dz == 0){
      // no translation then continue
   }
   else{
      //New Eye position
      Lx += moveSpeed*dx;
      Ly += moveSpeed*dy;
      Lz += moveSpeed*dz;
   }
   //Reset dx,dy,dz so it doesnt continuously calcualte new movement on next call
    dx=0;
    dy=0;
    dz=0;
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
   gluPerspective(fov,asp,dim/10,5*dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}


static void fishtank(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{  
   //  Set specular color to white
   float glass_spec[] = {0.9, 0.9, 0.9, 1.0};
   float glass_emission[] = {0.0,0.0,0.0,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,.96);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,glass_spec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,glass_emission);

   glEnable(GL_BLEND);
   glColor4f(0,.4,.5,alpha*.01);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
   glDepthMask(0);

   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //multi mesh glass
   double mul = 2.0/tankNumPoly;

   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   for (int i=0;i<tankNumPoly;i++)
      for (int j=0;j<tankNumPoly;j++){
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+0)-1,-1);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+0)-1,-1);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+1)-1,-1);
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+1)-1,-1);
      }
   glEnd();

   glPushMatrix();
   glRotated(180,0,1,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   for (int i=0;i<tankNumPoly;i++)
      for (int j=0;j<tankNumPoly;j++){
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+0)-1,-1);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+0)-1,-1);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+1)-1,-1);
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+1)-1,-1);
      }
   glEnd();
   glPopMatrix();

   glPushMatrix();
   glTranslated(1,0,0);
   glRotated(90,0,1,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,1);
   for (int i=0;i<tankNumPoly;i++)
      for (int j=0;j<tankNumPoly;j++){
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+0)-1,0);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+0)-1,0);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+1)-1,0);
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+1)-1,0);
      }
   glEnd();
   glPopMatrix();

   glPushMatrix();
   glTranslated(-1,0,0);
   glRotated(90,0,1,0);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   for (int i=0;i<tankNumPoly;i++)
      for (int j=0;j<tankNumPoly;j++){
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+0)-1,0);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+0)-1,0);
         glVertex3d(1*mul*(i+1)-1,1*mul*(j+1)-1,0);
         glVertex3d(1*mul*(i+0)-1,1*mul*(j+1)-1,0);
      }
   glEnd();
   glPopMatrix();
   glDisable(GL_BLEND);
   glDepthMask(1);
 

   //  Bottom
   glBegin(GL_QUADS);  
   glColor4f(0,.4,.5,1);
   glNormal3f( 0,-1, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   glEnd();

   //Sand bottom
   //  Set texture
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   glBegin(GL_QUADS);
   glColor4f(0,.5,.5,1);
   glNormal3f( 0,1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-.98,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-.98,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-.98,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-.98,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

static void newstar(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,double r,double g, double b)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0.01*emission,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   //Rotate flat
   glRotated(270,1,0,0);
   //Rotate angled
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);
   
   //vec1 on surface
   Vector vec1; 
   Vector vec3; 
   Vector vec2; 
   Vector normalVector;
   Vector midVec;
   
   for(int i = 0; i < 20; i++){
      vec1 = (Vector){vertexPosArr[i][0],vertexPosArr[i][1],vertexPosArr[i][2]};
      vec2 = (Vector){vertexPosArr[i][3],vertexPosArr[i][4],vertexPosArr[i][5]};
      vec3 = (Vector){vertexPosArr[i][6],vertexPosArr[i][7],vertexPosArr[i][8]};
      normalVector = normalVec(vec1,vec2,vec3);
      
      //Draw normal vector
      if(debugMode){
         
         glColor3f(1,.2,.2);
         midVec = middleVec(vec1,vec2,vec3);
      
         glBegin(GL_LINES);
         glVertex3f(midVec.x,midVec.y,midVec.z);
         glVertex3f(midVec.x+normalVector.x,midVec.y+normalVector.y,midVec.z+normalVector.z);
         glEnd();
      }
      
      //  Enable textures
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE); //GL_REPLACE  //modulate mixes the color with texture color      
      
      // Enable Blending
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      
      if(i%2 ==0){
         //glColor4f(1,1,.2,alpha*.01);
         glColor4d(r,g,b,(double)alpha*.01);   
         glBindTexture(GL_TEXTURE_2D,texture[0]);
         glBegin(GL_POLYGON);
         glNormal3f(normalVector.x, normalVector.y, normalVector.z);
         glTexCoord2f(0,0); glVertex3f( vertexPosArr[i][0],vertexPosArr[i][1],vertexPosArr[i][2]);
         glTexCoord2f(.33,.33); glVertex3f( vertexPosArr[i][3],vertexPosArr[i][4],vertexPosArr[i][5]);
         glTexCoord2f(0,1); glVertex3f( vertexPosArr[i][6],vertexPosArr[i][7],vertexPosArr[i][8]); 
      }
      else{
         glColor4d(r,g,b,(double)alpha*.01); 
         glBindTexture(GL_TEXTURE_2D,texture[0]);
         glBegin(GL_POLYGON);
         glNormal3f(normalVector.x, normalVector.y, normalVector.z); 
         glTexCoord2f(0,0); glVertex3f( vertexPosArr[i][0],vertexPosArr[i][1],vertexPosArr[i][2]);
         glTexCoord2f(0,1); glVertex3f( vertexPosArr[i][3],vertexPosArr[i][4],vertexPosArr[i][5]);
         glTexCoord2f(.33,.33); glVertex3f( vertexPosArr[i][6],vertexPosArr[i][7],vertexPosArr[i][8]); 
      }
      glEnd();
   }    
   //  Undo transformations
   glPopMatrix();
   //Disable texture applied from other polygons
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   double Ex = Ox-2*dim*Sin(th)*Cos(ph);
   double Ey = Oy+2*dim        *Sin(ph);
   double Ez = Oz+2*dim*Cos(th)*Cos(ph);
   gluLookAt(Ex,Ey,Ez , Ox,Oy,Oz , 0,Cos(ph),0);

   //  Flat or smooth shading
   glShadeModel(GL_SMOOTH ); //GL_FLAT 

   //  Light switch
   //  Translate intensity to color vectors
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
   
   //  Draw light position as ball (still no lighting here)
   glColor3f(1,1,1);

   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Location of viewer for specular calculations
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);

   //Position of Light
   if(mode == 0){
      float Position[]  = {3*Cos(zh),ylight,3*Sin(zh),1.0};
      ball(Position[0],Position[1],Position[2] , 0.1);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else{
      //Update position of light using dxdydz
      UpdateLightPosition();  
      ball(Lx,Ly,Lz,0.1);
      float Position[]  = {Lx,Ly,Lz,1.0};
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }

 
   //  Draw scene
   /*
   drawTree2(0.0,0.0,5.0,15);  
   cylinder(0.0,0.0,0.0,0.5,2.0,0.0);
   */
   cylinder(1.0,0.0,1.0,0.5,2.0,20.0,1);

   cylinder(1.0,0.0,1.0,0.5,2.0,-20.0,1);
   

   //double startx,double starty,double startz, double len, int theta
   drawCylinderTrees(0.0,0.0,0.0,5.0,45);
     /*
   for (int i =0 ; i<=numStars ; i++){
      newstar(StarPosArr[i].x,StarPosArr[i].y,StarPosArr[i].z , StarPosArr[i].sx,StarPosArr[i].sy,StarPosArr[i].sz, StarPosArr[i].th,StarPosArr[i].r,StarPosArr[i].g,StarPosArr[i].b);
   }
   newstar(0,0,0 , .1,.1,.1, 0,1,0,1);
   */
   
   //fishtank(0,1,0, tankx,tanky,tankz , 0);
   

   glDisable(GL_LIGHTING);

   //  Draw yellow square
   glColor3f(1,1,0);
   glBegin(GL_QUADS);
   glVertex2f(-1.0,-1.0);
   glVertex2f(+1.0,-1.0);
   glVertex2f(+1.0,+1.0);
   glVertex2f(-1.0,+1.0);
   glEnd();

   
 
   /*
   //  Draw blue spiral
   glColor3f(0,0,1);
   glBegin(GL_LINE_STRIP);
   for (int k=0;k<=1000;k+=10)
      glVertex2f(k*Cos(k)/1000,k*Sin(k)/1000);
   glEnd();
*/
   //  Draw axes - no lighting from here on
   const double len=dim*.9;  //  Length of axes
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
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");

      //Grid surface
      glColor3f(0,1,0);
      glBegin(GL_LINES); 
      for(int i=(-dim);i<=dim;i++){
         glVertex3d(i,0.0,-dim);
         glVertex3d(i,0.0,dim);
      }

      for(int i=(-dim);i<=dim;i++){
         glVertex3d(-dim,0.0,i);
         glVertex3d(dim,0.0,i);
      }      
      glEnd();
   }


   if(debugMode){
      //  Display parameters
      glWindowPos2i(5,5);
      Print("Angle=%d,%d  Dim=%.1f FOV=%d",th,ph,dim,fov);
      
      glWindowPos2i(5,25);
      Print("%s",modeTitles[mode]);
   }
   

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

void drawTree(double startx,double starty, double len, int theta){
   if(len > 1){
      double endx = (len)*Cos(theta) + startx;
      double endy = (len)*Sin(theta) +starty;
      printf("Right sx:%f sy:%f ex:%f ey:%f \n",startx,starty,endx,endy);

      //Branch right
      drawTree(endx,endy,len*.5,theta+10);
      //Branch left

      double lendx = (-1)*(len-5.0)*Sin(theta+10)+startx;
      double lendy = (len-5.0)*Cos(theta+10)+starty;
      printf("Left sx:%f sy:%f ex:%f ey:%f \n",startx,starty,lendx,lendy);
   }
}

void drawTree2(double startx,double starty, double len, int theta){
   if(len > 1){
      double endx = (len)*Cos(90-theta) + startx;
      double endy = (len)*Sin(90-theta) + starty;
      glColor3f(0,0,1);
      glBegin(GL_LINES);
      glVertex2d(startx,starty);
      glVertex2d(endx,endy);
      glEnd();
      ball(endx,endy,0.0,theta/100.0); 

      //Branch right
      drawTree2(endx,endy,len*.5,theta+10);
      //Branch left
      double lendx = (len)*Cos(90+theta)+startx;
      double lendy = (len)*Sin(90+theta)+starty;
      glColor3f(1,0,0);
      glBegin(GL_LINE_STRIP);
      glVertex2d(startx,starty);
      glVertex2d(lendx,lendy);
      glEnd(); 

      //Branch right
      drawTree2(lendx,lendy,len*.5,theta+10);  
   }
}


/*
 *  Draw vertex in polar coordinates
 */
static void VertexCyl(double th, double height)
{  
   //glColor3f(0.0 , 1.0f , 0.0);
   //glColor3f(Cos(th)*Cos(th) , 1.0f , Sin(th)*Sin(th));
   glNormal3f(Sin(th), height, Cos(th) ) ;
   glVertex3d(Sin(th), height , Cos(th));
}

void cylinder(double x,double y,double z,double r, double length, double rot, int dir)
{
   const int d=5;
   int th;
   double height=0;
   //# of segments/quads divided in the y/height direction
   const double heightinc=length/10.0;
   int increments = 10;
   //printf("%d %f \n",increments,length);
   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   ball(0,length,0,.1);
   glRotated(rot,0,0,1);
   glScaled(r,1,r);
     
   //  Latitude bands
   for (int i = 0;i<=increments; i++)
   {   
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         (dir) ? glColor3f(1,0,0):glColor3f(0,1,0);
         VertexCyl(th,height);
         VertexCyl(th,height+heightinc);
      }
      height += heightinc;
      glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}





void drawCylinderTrees(double startx,double starty,double startz, double len, int theta){
   if(len > 1){
      double endx = (len)*Cos(90-theta) + startx;
      double endy = (len)*Sin(90-theta) + starty;

      //Draw "right" branch
      cylinder(startx,starty,0.5,0.5,len, -theta,1);
      ball(endx,endy,0.0,theta/100.0); 

      //Recurse right
      drawCylinderTrees(endx,endy,0,0.5*len, theta+10);
      
      double lendx = (len)*Cos(90+theta)+startx;
      double lendy = (len)*Sin(90+theta)+starty;
      //Draw left branch
      cylinder(startx,starty,startz,0.5,len, theta,0);

      //Recurse right 
      drawCylinderTrees(lendx,lendy,0,0.5*len, theta+10);
   }
}


/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
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
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void handleLightMovement(unsigned char ch){
   if (ch=='a')
      dx -= 1;
   else if (ch=='d')
      dx += 1;
   else if (ch=='w')
      dz -= 1;
   else if (ch=='s')
      dz += 1;
   else if (ch=='e')
      dy += 1;
   else if (ch=='q')
      dy -= 1;
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
   else if (ch == '0'){
      th= 0; ph = 45;
   }
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      mode = (mode+1)%2;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == 'g')
      GenerateStarMatrix();
   else if (mode == 1)
      handleLightMovement(ch);
   else if (ch=='`')
      debugMode = (debugMode+1)%2;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project();
   //  Animate if requested
   glutIdleFunc(idle);
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
 *  GLUT calls this routine when a mouse is moved
 */
void motion(int x,int y)
{
   //  Do only when move is set
   //  WARNING:  this only works because by coincidence 1m = 1pixel
   if (move)
   {
         //  Left/right movement
         Ox += (X-x)/50.0*Cos(th) - (Y-y)/50.0*Sin(th);
      //  Near/far or Up/down movement
      if (move<0)
         Oy -= (Y-y)/50.0;
      else
         Oz += (X-x)/50.0*Sin(th)+(Y-y)/50.0*Cos(th);
      //  Remember location
      X = x;
      Y = y;
      glutPostRedisplay();
   }
}

/*
 *  GLUT calls this routine when a mouse button is pressed or released
 */
void mouse(int key,int status,int x,int y)
{
   //  On button down, set 'move' and remember location
   if (status==GLUT_DOWN)
   {
      move = (key==GLUT_LEFT_BUTTON) ? 1 : -1;
      X = x;
      Y = y;
   }
   //  On button up, unset move
   else if (status==GLUT_UP)
      move = 0;
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ALPHA);
   glutInitWindowSize(400,400);
   glutCreateWindow("HW3 : Justin Chin");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutIdleFunc(idle);
   drawTree(0.0,0.0,5.0,15);     
   GenerateStarMatrix();
   //Load Textures
   texture[0] = LoadTexBMP("corral_out.bmp");
   texture[1] = LoadTexBMP("gravel_img.bmp");
   texture[2] = LoadTexBMP("S2_out.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
