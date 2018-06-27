#ifndef MatrixFunc
#define MatrixFunc

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

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

#endif

