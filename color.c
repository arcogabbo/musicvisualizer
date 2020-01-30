#include "color.h"

void createRGB(struct RGB* color,double r,double g,double b)
{
    color->r=r;
    color->g=g;
    color->b=b;
}

void conversion(struct HSV in,struct RGB* out)
{
    double c,m,x;
    c=m=x=0.0;

    c=in.v * in.s;
    x=c * (1.0 - fabs(fmod(in.h / 60,2) - 1.0));
    m=in.v - c;

    if(in.h>=0.0 && in.h<60.0)
        createRGB(out,c+m,x+m,m);
    else if(in.h>=60.0 && in.h<120.0)
        createRGB(out,x+m,c+m,m);
    else if(in.h>=120.0 && in.h<180.0)
        createRGB(out,m,c+m,x+m);
    else if(in.h>=180.0 && in.h<240.0)
        createRGB(out,m,x+m,c+m);
    else if(in.h>=240.0 && in.h<300.0)
        createRGB(out,x+m,m,c+m);
    else if(in.h>=300 && in.h<360.0)
        createRGB(out,c+m,m,x+m);
    else
        createRGB(out,m,m,m);

    out->r *= 255;
    out->g *= 255;
    out->b *= 255;
}
