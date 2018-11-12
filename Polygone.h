#ifndef _POLYGONE_H_
#define _POLYGONE_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <math.h>

#include "Ppm.h"

//-----------------------------------------------------
typedef struct {
	int x, y;
} Point;

Point P_new(int x, int y);

//-----------------------------------------------------
struct node {
	Point pt;
	struct node *next;
	struct node *prev;
};

//-----------------------------------------------------
typedef struct {
	size_t length;
	struct node *first;
	struct node *last;
} Polygone;

Polygone *Poly_new(void);
void Poly_addPointLast(Polygone *poly, int x, int y);
void Poly_addPointFirst(Polygone *poly, int x, int y);
void Poly_addPointOrder(Polygone *poly, int x, int y);
void Poly_addPoint(Polygone *poly, int x, int y, int pos);
void Poly_delete(Polygone **poly);

//-----------------------------------------------------

typedef struct
{
	float _red, _green, _blue;
} Color;

Color C_new(float red, float green, float blue);
void C_check(Color c, char *message);

//-----------------------------------------------------
typedef struct
{
	int _width, _height;
	int _xzoom, _yzoom;
	int _xoffset, _yoffset;
	double _zoom;
	Color _current_color;
	Color **_buffer;
} Image;

Image* I_new				(int _width, int _height);
Image* I_read				(char *ppmfilename);

void I_fill       			(Image *img, Color c);
void I_checker    			(Image *img, Color c1, Color c2, int step);

void I_changeColor			(Image *img, Color c);
void I_plot       			(Image *img, int x, int y);
void I_plotColor  			(Image *img, int x, int y, Color c);

void I_focusPoint 			(Image *img, int x, int y);
void I_zoomInit   			(Image *img);
void I_zoom       			(Image *img, double zoom_coef);
void I_move       			(Image *img, int x, int y);

void I_draw       			(Image *img);

void I_bresenhamOrigin		(Image *img, int x, int y);

void Z2_to_firstOctant		(int xA, int yA, int xB, int yB, int* xA_1o, int* yA_1o, int* xB_1o, int* yB_1o);
void firstOctant_to_Z2		(int xA, int yA, int xB, int yB, int xA_1o, int yA_1o, int* x_Z2, int* y_Z2);
void I_bresenham					(Image *img, int xA, int yA, int xB, int yB);
void I_bresenhamColor			(Image *img, int xA, int yA, int xB, int yB, Color c);

void Poly_draw						(Image *img, Polygone *poly);
void Poly_select					(Image *img, Polygone *poly, int pos);
void Poly_deletePf				(Image *img, Polygone *poly);
void Poly_deletePl				(Image *img, Polygone *poly);
void Poly_deleteP					(Image *img, Polygone *poly, int pos);
void Poly_move						(Image *img, Polygone *poly, int pos, int d);
void Poly_selectE					(Image *img, Polygone *poly, int pos, Color c);
void Poly_addE						(Image *img, Polygone *poly, int pos);
int closestVertex					(Image *img, Polygone *poly, int x, int y);
int closestEdge						(Image *img, Polygone *poly, int x, int y);

int getXmin								(Polygone *poly);
int getYmin								(Polygone *poly);
int getXmax								(Polygone *poly);
int getYmax								(Polygone *poly);
int get_line_intersection	(int xA, int yA, int xB, int yB, int xC, int yC, int xD, int yD, int *xI, int *yI);
void scan_line						(Image *img, Polygone *poly);

#endif
