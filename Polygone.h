#ifndef _POLYGONE_H_
#define _POLYGONE_H_

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <math.h>

#include "Image.h"
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
void Poly_draw						(Image *img, Polygone *poly);
void Poly_drawSc					(Image *img, Polygone *poly);
void Poly_select					(Image *img, Polygone *poly, int pos, Color c);
void Poly_deletePf				(Image *img, Polygone *poly);
void Poly_deletePl				(Image *img, Polygone *poly);
void Poly_deleteP					(Image *img, Polygone *poly, int pos);
void Poly_move						(Image *img, Polygone *poly, int pos, int d);
void Poly_selectE					(Image *img, Polygone *poly, int pos, Color c);
void Poly_addE						(Image *img, Polygone *poly, int pos);
int closestVertex					(Image *img, Polygone *poly, int x, int y);
int closestEdge						(Image *img, Polygone *poly, int x, int y);

#endif
