#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct HSV
{
    double h,s,v;
};

struct RGB
{
    double r,g,b;
};

void createRGB(struct RGB*,double,double,double);
void conversion(struct HSV,struct RGB*);

#endif
