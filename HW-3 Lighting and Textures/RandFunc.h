#ifndef RandFunc  
#define RandFunc

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

double randDouble(double start, double end){
   int val = (rand() % (int)(end*100 - start*100) ) + (start*100);
   return val/100.0;
}

double randInt(int start, int end){
   int val = (rand() % (int)(end - start) ) + (start);
   return val/1.0;
}

#endif

