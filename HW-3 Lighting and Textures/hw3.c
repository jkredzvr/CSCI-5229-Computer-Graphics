/*
 *  Lighting
 *
 *  Demonstrates basic lighting using a sphere and a cube.
 *
 *  Key bindings:
 *  l          Toggles lighting
 *  a/A        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode
 *  F3         Toggle light distance (1/5)
 *  F8         Change ball increment
 *  F9         Invert bottom normal
 *  m          Toggles light movement
 *  []         Lower/rise light
 *  p          Toggles ortogonal/perspective projection
 *  +/-        Change field of view of perspective
 *  x          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx229.h"
//#include <math.h>

int axes=1;       //  Display axes
int mode=0;       //  mode 2 == move light manually

//mouse control
double Ox=0,Oy=0,Oz=0; //  LookAt Location
int move=0;            //  Move mode
int X,Y;               //  Last mouse location

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
int cycleNormals = 0;
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

// Textures
unsigned int texture[3]; // Texture names

//Stars
double numStars = 8; 

typedef struct {double x,y,z,sx,sy,sz,th;} StarPos;
StarPos *StarPosArr;

typedef struct{ float x,y,z; }Vector;
float dotProduct(Vector a, Vector b)
{
   return a.x*b.x+a.y*b.y+a.z*b.z;
}
 
Vector crossProduct(Vector a,Vector b)
{
   Vector c = {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
   return c;
}

Vector vecSubtract(Vector a,Vector b)
{
   Vector c = {a.x-b.x, a.y-b.y, a.z-b.z};
   return c;
}


Vector middleVec(Vector a,Vector b,Vector c)
{
   double meanx = (a.x+b.x+c.x)/3.0;
   double meany = (a.y+b.y+c.y)/3.0;
   double meanz = (a.z+b.z+c.z)/3.0;
   Vector d = {meanx,meany,meanz};
   return d;
}

Vector normalVec(Vector v1,Vector v2,Vector v3){
   Vector vec1 = {v1.x,v1.y,v1.z};
   Vector vec2 = {v2.x,v2.y,v2.z};
   Vector vec3 = {v3.x,v3.y,v3.z};
   Vector vecSurf1 = vecSubtract(vec1,vec2);
   Vector vecSurf2 = vecSubtract(vec1,vec3);
   Vector normVec = crossProduct(vecSurf1,vecSurf2);
   return normVec;
}

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

static double randDouble(double start, double end){
   int val = (rand() % (int)(end*100 - start*100) ) + (start*100);
   return val/100.0;
}

void GenerateStarMatrix(){
   StarPosArr = malloc(numStars*sizeof(StarPos));
   for(int i=0; i<numStars; i++) {
      //double x = randDouble(-(tankx/2)*.95,(tankx/2)*.95);
      double a = -tankx*.95;
      double b = tankx*.95;
      double x = randDouble(a ,b);
      double y = .1;
      double z = randDouble(a,b);
      double sx = randDouble(.05,.1);  
      double rot = randDouble(0,360);
      StarPos pos ={x,y,z,sx,sx,sx,rot};
      StarPosArr[i] = pos;
   }
}



/*
 *  Set projection
 */
static void Project1()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation  
   switch (mode){
         //Mode 0 ==  Orthogonal projection
         case 0:
            gluPerspective(fov,asp,dim/4,4*dim);            
            break;
      //Mode 1 == PerspectiveView
         case 1:
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
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,0,0);
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glNormal3f( 0, 0,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glNormal3f(+1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glNormal3f( 0,+1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glNormal3f( 0,-one, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
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
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glNormal3f( 0, 0,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glNormal3f(+1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   glEnd();
   glDisable(GL_BLEND);
   glDepthMask(1);


   glBegin(GL_QUADS);
   //  Bottom
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
                 double th)
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
   // top-right
   
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
      

      if(cycleNormals == i){
         
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
         glColor4f(1,.98,.8,alpha*.01);
         glBindTexture(GL_TEXTURE_2D,texture[0]);
         glBegin(GL_POLYGON);
         glNormal3f(normalVector.x, normalVector.y, normalVector.z);
         glTexCoord2f(0,0); glVertex3f( vertexPosArr[i][0],vertexPosArr[i][1],vertexPosArr[i][2]);
         glTexCoord2f(.33,.33); glVertex3f( vertexPosArr[i][3],vertexPosArr[i][4],vertexPosArr[i][5]);
         glTexCoord2f(0,1); glVertex3f( vertexPosArr[i][6],vertexPosArr[i][7],vertexPosArr[i][8]); 
      }
      else{
         glColor4f(1,.98,.8,alpha*.01);
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

static void star(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   // top-right
   
   //vec1 on surface
   Vector vec1; 
   Vector vec3; 
   Vector vec2; 
   Vector normalVector;
   Vector midVec;

   vec1 = (Vector){0,0,1};
   vec2 = (Vector){1,1,0};
   vec3 = (Vector){0,3,0};
   normalVector = normalVec(vec1,vec2,vec3);
   
   //Draw normal vector
   midVec = middleVec(vec1,vec2,vec3);
   
   glColor3f(1,.2,.2);
   glBegin(GL_LINES);
   glVertex3f(midVec.x,midVec.y,midVec.z);
   glVertex3f(midVec.x+normalVector.x,midVec.y+normalVector.y,midVec.z+normalVector.z);
   glEnd();
   
   
   glColor3f(1,0,1);
   glNormal3f(normalVector.x, normalVector.y, normalVector.z);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 1.0, 1.0, 0.0);
   glVertex3f( 0.0, 3.0, 0.0);
   glEnd();

   ///   
   vec1 = (Vector){0,0,1};
   vec2 = (Vector){0,3,0};
   vec3 = (Vector){-1,1,0};
   normalVector = normalVec(vec1,vec2,vec3);
   
   //Draw normal vector
   midVec = middleVec(vec1,vec2,vec3);
   
   glColor3f(1,.2,.2);
   glBegin(GL_LINES);
   glVertex3f(midVec.x,midVec.y,midVec.z);
   glVertex3f(midVec.x+normalVector.x,midVec.y+normalVector.y,midVec.z+normalVector.z);
   glEnd();


   // top-left
   glColor3f(1,1,0);
   glNormal3f(normalVector.x, normalVector.y, normalVector.z);
   glBegin(GL_POLYGON);
   glVertex3f( 0.0, 0.0, 1);
   glVertex3f( 0.0, 3.0, 0.0);
   glVertex3f( -1.0, 1.0, 0.0);
   glEnd();

   ///
   vec1 = (Vector){0,0,1};
   vec2 = (Vector){2.9,.9,0};
   vec3 = (Vector){1,1,0};
   normalVector = normalVec(vec1,vec2,vec3);
   
   
   //Draw normal vector
   midVec = middleVec(vec1,vec2,vec3);
   
   glColor3f(1,.2,.2);
   glBegin(GL_LINES);
   glVertex3f(midVec.x,midVec.y,midVec.z);
   glVertex3f(midVec.x+normalVector.x,midVec.y+normalVector.y,midVec.z+normalVector.z);
   glEnd();

   // topright-left
   glColor3f(1,1,0);
   glNormal3f(normalVector.x, normalVector.y, normalVector.z);
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
   glVertex3f( -1.0, 1.0, 0.0);
   glVertex3f( -2.9, 0.9, 0.0);
   
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
   glVertex3f( -2.9, 0.9, 0.0);
   glVertex3f( -1.0, 1.0, 0.0);
   glEnd();

   //  Undo transformations
   glPopMatrix();
}


/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = Ox-2*dim*Sin(th)*Cos(ph);
      double Ey = Oy+2*dim        *Sin(ph);
      double Ez = Oz+2*dim*Cos(th)*Cos(ph);
      //gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
      gluLookAt(Ex,Ey,Ez , Ox,Oy,Oz , 0,1,0);

   }
   //  Orthogonal - set world orientation
   else
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
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

        if(mode == 0){
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
   }
   else
     glDisable(GL_LIGHTING);

   //  Draw scene
   //Draw stars
   
   for (int i =0 ; i<=numStars ; i++){
      newstar(StarPosArr[i].x,StarPosArr[i].y,StarPosArr[i].z , StarPosArr[i].sx,StarPosArr[i].sy,StarPosArr[i].sz, StarPosArr[i].th);
   }
   
   fishtank(0,1,0, tankx,tanky,tankz , 0);


   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
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
   }

   glColor3f(0,1,0);
   //Grid surface
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

   if(debugMode){
      //  Display parameters
      glWindowPos2i(5,5);
      Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
        th,ph,dim,fov,mode?"Perpective":"Orthogonal",light?"On":"Off");
      if (light)
      {
         glWindowPos2i(5,45);
         Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
         glWindowPos2i(5,25);
         Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
      } 
   }
   

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
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
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 5 : 1;
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8)
      inc = (inc==10)?3:10;
   //  Flip sign
   else if (key == GLUT_KEY_F9)
      one = -one;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project1();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

void handleLightMovement(unsigned char ch){
   if (ch=='a')
      dx -= 1;
   else if (ch=='d')
      dx += 1;
   //  Diffuse level
   else if (ch=='w')
      dz -= 1;
   else if (ch=='s')
      dz += 1;
   else if (ch=='e')
      dy += 1;
   else if (ch=='q')
      dy -= 1;
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
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      mode = (mode+1)%2;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   else if (ch == 'z')
      cycleNormals = (cycleNormals+1)%20;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == 'g')
      GenerateStarMatrix();
   else if (ch == 'k')
      alpha--;
   else if (ch == 'K')
      alpha++;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   else if (mode == 1)
      handleLightMovement(ch);
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   else if (ch=='`')
      debugMode = (debugMode+1)%2;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project1();
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
   Project1();
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
      Ox += (X-x)/50.0;
      //  Near/far or Up/down movement
      if (move<0)
         Oy -= (Y-y)/50.0;
      else
         Oz += (Y-y)/50.0;
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
   glutCreateWindow("Lighting");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);

   glutMouseFunc(mouse);
   glutMotionFunc(motion);

   glutIdleFunc(idle);

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
