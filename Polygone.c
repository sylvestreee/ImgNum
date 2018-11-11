#include "Polygone.h"

Point P_new(int x, int y) {
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

//---------------------------------------------------------------------------

Polygone *Poly_new(void) {
	Polygone *poly_new = malloc(sizeof *poly_new);
	if(poly_new != NULL) {
		poly_new->length = 0;
		poly_new->first = NULL;
		poly_new->last = NULL;
	}
	return poly_new;
}

//---------------------------------------------------------------------------

void Poly_addPointLast(Polygone *poly, int x, int y) {
	if(poly != NULL) {
		struct node *pt_new = malloc(sizeof *pt_new);
		if(pt_new != NULL) {
			pt_new->pt = P_new(x,y);
			pt_new->next = NULL;

			if(poly->last == NULL) {
				poly->first = pt_new;
				poly->last = pt_new;
			}

			else {
				pt_new->prev = poly->last;
				poly->last->next = pt_new;
				poly->last = pt_new;
			}
		}
		poly->length++;
		printf("nombre de points : %zu\n", poly->length);
	}
}

//---------------------------------------------------------------------------

void Poly_addPointFirst(Polygone *poly, int x, int y) {
	if(poly != NULL) {
		struct node *pt_new = malloc(sizeof *pt_new);
		if(pt_new != NULL) {
			pt_new->pt = P_new(x,y);
			pt_new->prev = NULL;

			if(poly->first == NULL) {
				poly->first = pt_new;
				poly->last = pt_new;
			}

			else {
				pt_new->next = poly->first;
				poly->first->prev = pt_new;
				poly->first = pt_new;
			}
		}
		poly->length++;
		printf("nombre de points : %zu\n", poly->length);
	}
}

//---------------------------------------------------------------------------

void Poly_addPointOrder(Polygone *poly, int x, int y) {
	int pos = 0;
	if(poly != NULL) {
		struct node *p_temp = poly->first;
		if(p_temp != NULL) {
			if(y < poly->first->pt.y) {
				printf("add first\n");
				Poly_addPointFirst(poly, x, y);
			}
			if(y > poly->last->pt.y) {
				printf("add last\n");
				Poly_addPointLast(poly, x, y);
			}
			else {
				/*ERROR*/
				printf("poly->first->pt.y : %d\n", p_temp->pt.y);
				while(p_temp != NULL && y > p_temp->pt.y) {
					printf("lalalala\n");
					p_temp = p_temp->next;
					pos++;
				}
				Poly_addPoint(poly, x, y, pos);
				printf("add\n");
			}
		}
	}
}

//---------------------------------------------------------------------------

void Poly_addPoint(Polygone *poly, int x, int y, int pos) {
	if(poly != NULL) {
		struct node *pt_temp = poly->first;
		int i = 1;
		while(pt_temp != NULL && i <= pos) {
			if(i == pos) {
				if(poly->last == NULL) {
					Poly_addPointLast(poly, x, y);
				}
				else {
					struct node *pt_new = malloc(sizeof *pt_new);
					if(pt_new != NULL) {
						pt_new->pt = P_new(x, y);

						//bound new with next
						pt_new->next = pt_temp;

						//bound new with prev
						pt_new->prev = pt_temp->prev;

						//bound prev with new
						pt_temp->prev->next = pt_new;

						//bound next with new
						pt_temp->prev = pt_new;
						poly->length++;
					}
				}
			}
			else {
				pt_temp = pt_temp->next;
			}
			i++;
		}
	}
}

//---------------------------------------------------------------------------

void Poly_delete(Polygone **poly) {
	if(*poly != NULL) {
		struct node *p_temp = (*poly)->first;
		while(p_temp != NULL) {
			struct node *p_del = p_temp;
			p_temp = p_temp->next;
			free(p_del);
		}
		free(*poly); *poly = NULL;
	}
}

//---------------------------------------------------------------------------

Color C_new(float red, float green, float blue)
{
	Color c;
	c._red = red;
	c._green = green;
	c._blue = blue;
	return c;
}

//---------------------------------------------------------------------------

void C_check(Color c, char *message)
{
	fprintf(stderr,"%s : %f %f %f\n",message,c._red,c._green,c._blue);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

Image* I_new(int width, int height)
{
	Image *img_new = (Image*)malloc(sizeof(Image));
	img_new->_width = width;
	img_new->_height = height;
	img_new->_xzoom = 0;
	img_new->_yzoom = 0;
	img_new->_zoom = 1.0;

	img_new->_xoffset=0;
	img_new->_yoffset=0;

	img_new->_current_color = C_new(255,255,255);

	img_new->_buffer = (Color**)calloc(width,sizeof(Color*));

	int x;
	for(x=0;x<width;x++)
		img_new->_buffer[x] = (Color*)calloc(height,sizeof(Color));

	return img_new;
}

//---------------------------------------------------------------------------

static void _plot(Image *img, int x, int y, Color c)
{
	img->_buffer[x][y] = c;
}

//-----

static int _isPpm(char *imagefilename)
{
	FILE *imagefile;
	imagefile = fopen(imagefilename,"r");
	if(imagefile==NULL) {perror(imagefilename); exit(1); }

	else
	{
		int c1 = fgetc(imagefile);
		int c2 = fgetc(imagefile);
		fclose(imagefile);

		if((c1=='P')&&(c2=='6'))	return 1;
		else						return 0;
	}
}

//-----

Image* I_read(char *imagefilename)
{
	Image *img;
	char command[100];

	if(_isPpm(imagefilename))	sprintf(command,"cp %s input.ppm",imagefilename);
	else					sprintf(command,"convert %s input.ppm",imagefilename);

	int stat = system(command);
	if(stat!=0)
	{
		fprintf(stderr,"Convert : %s -> input.ppm impossible conversion.\n", imagefilename);
		exit(1);
	}
	else
	{
		Ppm ppm = PPM_nouv("input.ppm", PPM_LECTURE);
		system("rm input.ppm");

		fprintf(stderr,"%d x %d\n",PPM_largeur(ppm),PPM_hauteur(ppm));

		if(ppm!=NULL)
		{
			img = I_new(PPM_largeur(ppm),PPM_hauteur(ppm));
			int nb_bits=ppm->_nb_bits;
			int valmax = ppm->_valmax;

			int nb_pixels = img->_width*img->_height;

			if(nb_bits <= 8)
			{
				unsigned char *donnees = (unsigned char*)calloc(3*nb_pixels,sizeof(unsigned char));
				PPM_lectureDonneesChar(ppm, donnees);

				int x,y;
				for(y=0;y<img->_height;y++)
					for(x=0;x<img->_width;x++)
					{
						int indice = (img->_height-y)*img->_width + x;
						Color c = C_new((1.0*donnees[3*indice  ])/valmax,
										(1.0*donnees[3*indice+1])/valmax,
										(1.0*donnees[3*indice+2])/valmax);
						_plot(img,x,y,c);
					}
			}
			else
			{
				unsigned short *donnees = (unsigned short*)calloc(3*nb_pixels,sizeof(unsigned short));
				PPM_lectureDonneesShort(ppm, donnees);
				int x,y;
				for(y=0;y<img->_height;y++)
					for(x=0;x<img->_width;x++)
					{
						int indice = (img->_height-y)*img->_width + x;
						Color c = C_new((1.0*donnees[3*indice  ])/valmax,
										(1.0*donnees[3*indice+1])/valmax,
										(1.0*donnees[3*indice+2])/valmax);
						img->_buffer[x][y] = c;
					}
			}
			PPM_fermeture(ppm);
			return(img);
		}
		else
			return(NULL);
	}
}

//---------------------------------------------------------------------------

void I_fill(Image *img, Color c)
{
	int x,y;
	for(x=0;x<img->_width;x++)
		for(y=0;y<img->_height;y++)
			img->_buffer[x][y]=c;
}

//---------------------------------------------------------------------------

void I_checker(Image *img, Color c1, Color c2, int step)
{
	int x,y;
	for(x=0;x<img->_width;x++)
		for(y=0;y<img->_height;y++)
		{
			int n_x = x/step;
			int n_y = y/step;
			if((n_x+n_y)%2==0)	_plot(img,x,y,c1);
			else				_plot(img,x,y,c2);
		}
}

//---------------------------------------------------------------------------

void I_changeColor(Image *img, Color c)
{
	img->_current_color = c;
}

//---------------------------------------------------------------------------

void I_plot(Image *img, int x, int y)
{
	if((x>=0)&&(x<img->_width)&&
	   (y>=0)&&(y<img->_height))
		img->_buffer[x][y] = img->_current_color;
	else
	{
		fprintf(stderr,"I_plot : ERROR !!!\n");
		fprintf(stderr,"x (=%d) must be in the [%d,%d] range and\n", x, 0, img->_width);
		fprintf(stderr,"y (=%d) must be in the [%d,%d] range\n", y, 0, img->_height);
	}
}

//---------------------------------------------------------------------------

void I_plotColor(Image *img, int x, int y, Color c)
{
	if((x>=0)&&(x<img->_width)&&
	   (y>=0)&&(y<img->_height))
		img->_buffer[x][y] = c;
	else
	{
		fprintf(stderr,"I_plotColor : ERROR !!!\n");
		fprintf(stderr,"x (=%d) must be in the [%d,%d] range and\n", x, 0, img->_width);
		fprintf(stderr,"y (=%d) must be in the [%d,%d] range\n", y, 0, img->_height);
	}
}

//---------------------------------------------------------------------------
// Changement de repère

static void _windowToImage(Image *img, int xwin, int ywin, int *ximg, int *yimg)
{

	*ximg = img->_xoffset + img->_xzoom + (xwin-img->_xzoom) / img->_zoom;
	*yimg = img->_yoffset + img->_yzoom + (ywin-img->_yzoom) / img->_zoom;
}

//-----
// Changement de repère inverse
/*
static void _imageToWindow(Image *img, int ximg, int yimg, int *xwin, int *ywin)
{

	*xwin = img->_xoffset + img->_xzoom + (ximg-img->_xzoom-img->_xoffset) * img->_zoom;
	*ywin = img->_yoffset + img->_yzoom + (yimg-img->_yzoom-img->_yoffset) * img->_zoom;
}
*/
//-----

void I_focusPoint(Image *img, int xwin, int ywin)
{
	int dx = xwin - img->_xzoom;
	int dy = ywin - img->_yzoom;
	img->_xoffset -= dx*(1-1.0/img->_zoom);
	img->_yoffset -= dy*(1-1.0/img->_zoom);
	img->_xzoom = xwin;
	img->_yzoom = ywin;
}

//---------------------------------------------------------------------------

void I_zoomInit(Image *img)
{
	img->_xoffset = 0;
	img->_yoffset = 0;
	img->_zoom = 1.0;
}

//---------------------------------------------------------------------------

void I_zoom(Image *img, double zoom_coef)
{
	img->_zoom = img->_zoom * zoom_coef;
}

//---------------------------------------------------------------------------

void I_move(Image *img, int x, int y)
{
	img->_xoffset += x;
	img->_yoffset += y;
}

//---------------------------------------------------------------------------

void I_draw(Image *img)
{
	glBegin(GL_POINTS);
	int xwin, ywin, ximg, yimg;
	for(xwin=0;xwin<img->_width;xwin++)
		for(ywin=0;ywin<img->_height;ywin++)
		{
			_windowToImage(img, xwin, ywin, &ximg, &yimg);
			Color c;
			if((ximg>=0)&&(ximg<img->_width)&&
			   (yimg>=0)&&(yimg<img->_height))
				c = img->_buffer[ximg][yimg];
			else
				c = C_new(0,0,0);

			glColor3f(c._red,c._green,c._blue);
			glVertex2i(xwin,ywin);
		}
	glEnd();
}

//---------------------------------------------------------------------------

void I_bresenhamOrigin(Image *img, int x, int y) {
	int dx = x, dy = y;
	int incrd1 = 2 * dy, incrd2 = 2 * (dy - dx);
	int x_Bre = 0, y_Bre = 0, d = 2 * dy - dx;
	while(x_Bre < x) {
		I_plot(img, x_Bre, y_Bre);
		x_Bre = x_Bre+1;
		if(d<0) {
			d=d+incrd1;
		}
		else {
			y_Bre=y_Bre+1;
			d=d+incrd2;
		}
	}
	I_plot(img, x_Bre, y_Bre);
}

//---------------------------------------------------------------------------

void Z2_to_firstOctant(int xA, int yA, int xB, int yB, int* xA_1o, int* yA_1o, int* xB_1o, int* yB_1o) {
	int xA_1q, yA_1q;
	int xB_1q, yB_1q;

	if(xB > xA) {
		xA_1q = xA; xB_1q = xB;
	}
	else {
		xA_1q = -xA; xB_1q = -xB;
	}

	if(yB > yA) {
		yA_1q = yA; yB_1q = yB;
	}
	else {
		yA_1q = -yA; yB_1q = -yB;
	}

	if(xB_1q - xA_1q > yB_1q - yA_1q) {
		*xA_1o = xA_1q; *xB_1o = xB_1q;
		*yA_1o = yA_1q; *yB_1o = yB_1q;
	}
	else {
		*xA_1o = yA_1q; *xB_1o = yB_1q;
		*yA_1o = xA_1q; *yB_1o = xB_1q;
	}
}

//---------------------------------------------------------------------------

void firstOctant_to_Z2(int xA, int yA, int xB, int yB, int x_1o, int y_1o, int* x_Z2, int* y_Z2) {
	int x_1q, y_1q;
	if(abs(xB - xA) > abs(yB - yA)) {
		x_1q = x_1o; y_1q = y_1o;
	}
	else {
		x_1q = y_1o; y_1q = x_1o;
	}

	if(xB > xA) {
		*x_Z2 = x_1q;
	}
	else {
		*x_Z2 = -x_1q;
	}

	if(yB > yA) {
		*y_Z2 = y_1q;
	}
	else {
		*y_Z2 = -y_1q;
	}
}

//---------------------------------------------------------------------------

void I_bresenham(Image *img, int xA, int yA, int xB, int yB) {
	int xA_1o, yA_1o;
	int xB_1o, yB_1o;
	int x_Z2, y_Z2;
	int dx, dy, d;
	int incrd1, incrd2;
	int x, y;

	Z2_to_firstOctant(xA, yA, xB, yB, &xA_1o, &yA_1o, &xB_1o, &yB_1o);
	dx = xB_1o - xA_1o; dy = yB_1o - yA_1o;
	incrd1 = 2*dy; incrd2 = 2*(dy-dx);
	x = xA_1o; y = yA_1o; d = 2*dy-dx;
	while(x<xB_1o) {
		firstOctant_to_Z2(xA, yA, xB, yB, x, y, &x_Z2, &y_Z2);
		I_plot(img, x_Z2, y_Z2);
		x=x+1;
		if(d<0) {
			d=d+incrd1;
		}
		else {
			y=y+1; d=d+incrd2;
		}
	}
	firstOctant_to_Z2(xA, yA, xB, yB, x, y, &x_Z2, &y_Z2);
	I_plot(img, x_Z2, y_Z2);
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), des coordonnées (type int)
//	et une couleur (type Color).
//	Relie deux points par une droite de Bresenham colorée par
// 	la couleur indiquée.
//---------------------------------------------------------------------------

void I_bresenhamColor(Image *img, int xA, int yA, int xB, int yB, Color c) {
	int xA_1o, yA_1o;
	int xB_1o, yB_1o;
	int x_Z2, y_Z2;
	int dx, dy, d;
	int incrd1, incrd2;
	int x, y;

	Z2_to_firstOctant(xA, yA, xB, yB, &xA_1o, &yA_1o, &xB_1o, &yB_1o);
	dx = xB_1o - xA_1o; dy = yB_1o - yA_1o;
	incrd1 = 2*dy; incrd2 = 2*(dy-dx);
	x = xA_1o; y = yA_1o; d = 2*dy-dx;
	while(x<xB_1o) {
		firstOctant_to_Z2(xA, yA, xB, yB, x, y, &x_Z2, &y_Z2);
		I_plotColor(img, x_Z2, y_Z2, c);
		x=x+1;
		if(d<0) {
			d=d+incrd1;
		}
		else {
			y=y+1; d=d+incrd2;
		}
	}
	firstOctant_to_Z2(xA, yA, xB, yB, x, y, &x_Z2, &y_Z2);
	I_plotColor(img, x_Z2, y_Z2, c);
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image) et un polygone (type Polygone).
//	Parcourt la chaîne et trace une ligne de Bresenham pour chaque point
//	avec son successeur, tant que celui-ci existe.
//---------------------------------------------------------------------------
void Poly_draw(Image *img, Polygone *poly) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			while(p_temp->next != NULL) {
				I_bresenham(img, p_temp->pt.x, p_temp->pt.y, p_temp->next->pt.x, p_temp->next->pt.y);
				p_temp = p_temp->next;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone)
//	et une position (type int).
//	Parcourt la chaîne jusqu'à atteindre le point indiqué par la position.
// 	Une fois ce dernier atteint, un carré est dessiné à l'aide de droites de
//	Bresenham autour du point pour le mettre en évidence.
//---------------------------------------------------------------------------
void Poly_select(Image *img, Polygone *poly, int pos) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL && i <= pos) {
				if(i == pos) {
					printf("point n°%d\n",pos);
					I_bresenham(img, (p_temp->pt.x)-5,(p_temp->pt.y)+5,(p_temp->pt.x)+5,(p_temp->pt.y)+5);
					I_bresenham(img, (p_temp->pt.x)+5,(p_temp->pt.y)+5,(p_temp->pt.x)+5,(p_temp->pt.y)-5);
					I_bresenham(img, (p_temp->pt.x)+5,(p_temp->pt.y)-5,(p_temp->pt.x)-5,(p_temp->pt.y)-5);
					I_bresenham(img, (p_temp->pt.x)-5,(p_temp->pt.y)-5,(p_temp->pt.x)-5,(p_temp->pt.y)+5);
				}
				else {
					p_temp = p_temp->next;
				}
				i++;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone).
//	Supprime le premier point de la chaîne.
//---------------------------------------------------------------------------
void Poly_deletePf(Image *img, Polygone *poly) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			p_temp->next->prev = NULL;
			poly->first = p_temp->next;
			free(p_temp);
			poly->length--;
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone).
//	Supprime le dernier point de la chaîne.
//---------------------------------------------------------------------------
void Poly_deletePl(Image *img, Polygone *poly) {
	if(poly != NULL) {
		if(poly->last != NULL) {
			struct node *p_temp = poly->last;
			p_temp->prev->next = NULL;
			poly->last = p_temp->prev;
			free(p_temp);
			poly->length--;
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone)
//	et une position (type int).
//	Parcourt la chaîne jusqu'à atteindre le point indiqué par la position.
// 	Une fois ce dernier atteint, il est supprimé de la chaîne.
//---------------------------------------------------------------------------
void Poly_deleteP(Image *img, Polygone *poly, int pos) {
	if(poly != NULL) {
		if(pos == 1) {
			Poly_deletePf(img, poly);
		}
		else if((size_t)pos == poly->length) {
			Poly_deletePl(img, poly);
		}
		else {
			if(poly->first != NULL) {
				struct node *p_temp = poly->first;
				int i = 1;
				while(p_temp != NULL && i <= pos) {
					if(i == pos) {
						p_temp->prev->next = p_temp->next;
						p_temp->next->prev = p_temp->prev;
						free(p_temp);
						poly->length--;
					}
					else {
						p_temp = p_temp->next;
					}
					i++;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone)
//	une position (type int) et une direction (type int).
//	Parcourt la chaîne jusqu'à atteindre le point indiqué par la position.
// 	Une fois ce dernier atteint, il est déplacé dans la direction indiquée
//	(0 -> haut / 1 -> bas / 2 -> gauche / 3 -> droite).
//---------------------------------------------------------------------------
void Poly_move(Image *img, Polygone *poly, int pos, int d) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL && i <= pos) {
				if(i == pos) {
					switch(d) {
						case 0 :
							p_temp->pt.y-=1;
							break;
						case 1 :
							p_temp->pt.y+=1;
							break;
						case 2 :
							p_temp->pt.x-=1;
							break;
						case 3 :
							p_temp->pt.x+=1;
							break;
						default :
							break;
					}
				}
				else {
					p_temp = p_temp->next;
				}
				i++;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone)
//	et une position (type int) et une couleur (type Color).
//	Parcourt la chaîne jusqu'à atteindre l'arête indiquée par la position.
// 	Une fois cette dernière atteinte, elle est redessinée selon
//	la couleur indiquée.
//---------------------------------------------------------------------------
void Poly_selectE(Image *img, Polygone *poly, int pos, Color c) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL && i <= pos) {
				if(i == pos) {
					printf("arête n°%d\n",pos);
					I_bresenhamColor(img, p_temp->pt.x, p_temp->pt.y, p_temp->next->pt.x, p_temp->next->pt.y, c);
				}
				else {
					p_temp = p_temp->next;
				}
				i++;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone)
//	et une position (type int).
//	Parcourt la chaîne jusqu'à atteindre l'arête indiquée par la position.
// 	Une fois cette dernière atteinte, un point est ajouté
//	entre ses extrêmités.
//---------------------------------------------------------------------------
void Poly_addE(Image *img, Polygone *poly, int pos) {
	int x_n, y_n;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL && i <= pos) {
				if(i == pos) {
					x_n = (p_temp->pt.x + p_temp->next->pt.x)/2;
					y_n = (p_temp->pt.y + p_temp->next->pt.y)/2;
					Poly_addPoint(poly, x_n, y_n, pos+1);
				}
				else {
					p_temp = p_temp->next;
				}
				i++;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone)
//	et des coordonnées (type int).
//	Parcourt la chaîne et calcule la distance pour chaque point avec le point
// 	aux coordonnées indiquées.
// 	Retourne la position du point le plus proche de ce dernier.
//---------------------------------------------------------------------------
int closestVertex(Image *img, Polygone *poly, int x, int y) {
	int min, indice, dist;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL) {
				dist = sqrt(pow(x-p_temp->pt.x, 2) + pow(y-p_temp->pt.y, 2));
				printf("distance : %d\n",dist);
				if(i == 1) {
					min = dist;
					indice = i;
				}
				else {
					if(dist < min) {
						min = dist;
						indice = i;
					}
				}
				i++;
				p_temp = p_temp->next;
			}
		}
	}
	return indice;
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image), un polygone (type Polygone)
//	et des coordonnées (type int).
//	Parcourt la chaîne et calcule la distance pour chaque arête de son milieu
// 	avec le point aux coordonnées indiquées.
// 	Retourne la position de la chaîne la plus proche de ce dernier.
//---------------------------------------------------------------------------
int closestEdge(Image *img, Polygone *poly, int x, int y) {
	int min, indice, dist, x_n, y_n;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp->next != NULL) {
				x_n = (p_temp->pt.x + p_temp->next->pt.x)/2;
				y_n = (p_temp->pt.y + p_temp->next->pt.y)/2;
				dist = sqrt(pow(x-x_n, 2) + pow(y-y_n, 2));
				printf("distance : %d\n",dist);
				if(i == 1) {
					min = dist;
					indice = i;
				}
				else {
					if(dist < min) {
						min = dist;
						indice = i;
					}
				}
				i++;
				p_temp = p_temp->next;
			}
		}
	}
	return indice;
}

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
			while(p_temp->next != NULL) {
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
			while(p_temp->next != NULL) {
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
			while(p_temp->next != NULL) {
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
			while(p_temp->next != NULL) {
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
				*yI = (int)(xA + (t*xAB));
			}
			return 1;
		}
	}
	return 0;
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image) et un polygone (type Polygone).
//	blablabla
//---------------------------------------------------------------------------
void scan_line(Image *img, Polygone *poly) {
	Polygone *poly_inter = Poly_new();
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

		*x_inter = -1;
		*y_inter = -1;

		if(poly != NULL) {
			if(poly->first != NULL) {
				struct node *p_temp = poly->first;
				while(p_temp->next != NULL) {
					inter = get_line_intersection(xA, y, xB, y, p_temp->pt.x, p_temp->pt.y, p_temp->next->pt.x, p_temp->next->pt.y, x_inter, y_inter);
					if(inter) {
						if(poly_inter->length == (size_t)0)
						{
							printf("first\n");
							Poly_addPointFirst(poly_inter, *x_inter, *y_inter);
						}
						printf("not first\n");
						Poly_addPointOrder(poly_inter, *x_inter, *y_inter);
					}
					p_temp = p_temp->next;
				}
				printf("not not first\n");
				inter = get_line_intersection(xA, y, xB, y, poly->first->pt.x, poly->first->pt.y, poly->last->pt.x, poly->last->pt.y, x_inter, y_inter);
				if(inter) {
					Poly_addPointOrder(poly_inter, *x_inter, *y_inter);
				}
			}
		}
	}
	free(x_inter);
	free(y_inter);
	printf("fill\n");
	Poly_draw(img, poly_inter);
}
