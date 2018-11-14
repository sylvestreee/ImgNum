#include "Scanline.h"

//---------------------------------------------------------------------------
//	Prend en paramètre un polygone (type Polygone).
//	Parcourt la chaîne et renvoie la coordonnée X la plus petite.
//---------------------------------------------------------------------------
int getXmin(Polygone *poly) {
	int x_min;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL) {
				if(i == 1) {
					x_min = p_temp->pt.x;
				}
				else {
					if(p_temp->pt.x < x_min) {
						x_min = p_temp->pt.x;
					}
				}
				i++;
				p_temp = p_temp->next;
			}
		}
	}
	return x_min;
}

//---------------------------------------------------------------------------
//	Prend en paramètre un polygone (type Polygone).
//	Parcourt la chaîne et renvoie la coordonnée Y la plus petite.
//---------------------------------------------------------------------------
int getYmin(Polygone *poly) {
	int y_min;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL) {
				if(i == 1) {
					y_min = p_temp->pt.y;
				}
				else {
					if(p_temp->pt.y < y_min) {
						y_min = p_temp->pt.y;
					}
				}
				i++;
				p_temp = p_temp->next;
			}
		}
	}
	return y_min;
}

//---------------------------------------------------------------------------
//	Prend en paramètre un polygone (type Polygone).
//	Parcourt la chaîne et renvoie la coordonnée X la plus grande.
//---------------------------------------------------------------------------
int getXmax(Polygone *poly) {
	int x_max;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL) {
				if(i == 1) {
					x_max = p_temp->pt.x;
				}
				else {
					if(p_temp->pt.x > x_max) {
						x_max = p_temp->pt.x;
					}
				}
				i++;
				p_temp = p_temp->next;
			}
		}
	}
	return x_max;
}

//---------------------------------------------------------------------------
//	Prend en paramètre un polygone (type Polygone).
//	Parcourt la chaîne et renvoie la coordonnée Y la plus grande.
//---------------------------------------------------------------------------
int getYmax(Polygone *poly) {
	int y_max;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL) {
				if(i == 1) {
					y_max = p_temp->pt.y;
				}
				else {
					if(p_temp->pt.y > y_max) {
						y_max = p_temp->pt.y;
					}
				}
				i++;
				p_temp = p_temp->next;
			}
		}
	}
	return y_max;
}

//---------------------------------------------------------------------------
//	Prend en paramètre des coordonnées (type int).
// 	Teste l'intersection entre deux droites et indique
// 	si celle-ci existe ou non.
//---------------------------------------------------------------------------
int get_line_intersection(int xA, int yA, int xB, int yB, int xC, int yC, int xD, int yD, int *xI, int *yI) {
	float xAB, yAB, xDC, yDC, xCA, yCA;
	float s, t, d;

	xAB = (float)xB - (float)xA;
	yAB = (float)yB - (float)yA;
	xDC = (float)xC - (float)xD;
	yDC = (float)yC - (float)yD;
	xCA = (float)xA - (float)xC;
	yCA = (float)yA - (float)yC;

	d = yDC*xAB - yAB*xDC;

	if(d != 0) {
		s = (xDC*yCA - xCA*yDC)/d;
		t = (xCA*yAB - xAB*yCA)/d;

		if((s >= 0.0) && (s <= 1.0) && (t >= 0.0) && (t <= 1.0)) {
			if(xI != NULL) {
				*xI = (int)(xA + (s*xAB));
			}
			if(yI != NULL) {
				*yI = (int)(yA + (s*yAB));
			}
			return 1;
		}
	}
	return 0;
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image) et un polygone (type Polygone).
//	Effectue un scan line sur le polygone.
//---------------------------------------------------------------------------
void scan_line(Image *img, Polygone *poly) {
	int *x_inter = malloc(sizeof(int));
	int *y_inter = malloc(sizeof(int));

	int xMin = getXmin(poly);
	int yMin = getYmin(poly);
	int xMax = getXmax(poly);
	int yMax = getYmax(poly);

	int xA = xMin;
	int xB = xMax;
	int y, inter, i;

	for(i = yMin; i < yMax; i++) {
		y = i;
		Polygone *poly_inter = Poly_new();

		*x_inter = -1;
		*y_inter = -1;

		if(poly != NULL) {
			if(poly->first != NULL) {
				struct node *p_temp = poly->first;
				while(p_temp->next != NULL) {
					inter = get_line_intersection(xA, y, xB, y, p_temp->pt.x, p_temp->pt.y, p_temp->next->pt.x, p_temp->next->pt.y, x_inter, y_inter);
					if(inter) {
						if(*y_inter != fmax(p_temp->pt.y, p_temp->next->pt.y)) {
							if(poly_inter->length == (size_t)0) {
								Poly_addPointFirst(poly_inter, *x_inter, *y_inter);
							}
              else {
								Poly_addPointOrder(poly_inter, *x_inter, *y_inter);
							}
						}
					}
					p_temp = p_temp->next;
				}
				inter = get_line_intersection(xA, y, xB, y, poly->last->pt.x, poly->last->pt.y, poly->first->pt.x, poly->first->pt.y, x_inter, y_inter);
				if(inter) {
					if(*y_inter != fmax(poly->last->pt.y, poly->first->pt.y)) {
						Poly_addPointOrder(poly_inter, *x_inter, *y_inter);
					}
				}
			}
		}
		Poly_drawSc(img, poly_inter);
		Poly_delete(&poly_inter);
	}
	free(x_inter);
	free(y_inter);
}
