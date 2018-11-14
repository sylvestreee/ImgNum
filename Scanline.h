#ifndef _SCANLINE_H_
#define _SCANLINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <math.h>

#include "Polygone.h"
#include "Ppm.h"

int getXmin								(Polygone *poly);
int getYmin								(Polygone *poly);
int getXmax								(Polygone *poly);
int getYmax								(Polygone *poly);
int get_line_intersection	(int xA, int yA, int xB, int yB, int xC, int yC, int xD, int yD, int *xI, int *yI);
void scan_line						(Image *img, Polygone *poly);

#endif
