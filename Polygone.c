#include "Polygone.h"

Point P_new(int x, int y) {
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

//------------------------------------------------------------------------

Polygone *Poly_new(void) {
	Polygone *poly_new = malloc(sizeof *poly_new);
	if(poly_new != NULL) {
		poly_new->length = 0;
		poly_new->first = NULL;
		poly_new->last = NULL;
	}
	return poly_new;
}

//------------------------------------------------------------------------

void Poly_addPointLast(Polygone *poly, int x, int y) {
	if(poly != NULL) {
		struct node *pt_new = malloc(sizeof *pt_new);
		if(pt_new != NULL) {
			pt_new->pt = P_new(x,y);
			pt_new->next = NULL;

			if(poly->last == NULL) {
				pt_new->prev = NULL;
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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

Color C_new(float red, float green, float blue)
{
	Color c;
	c._red = red;
	c._green = green;
	c._blue = blue;
	return c;
}

//------------------------------------------------------------------------

void C_check(Color c, char *message)
{
	fprintf(stderr,"%s : %f %f %f\n",message,c._red,c._green,c._blue);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

void I_fill(Image *img, Color c)
{
	int x,y;
	for(x=0;x<img->_width;x++)
		for(y=0;y<img->_height;y++)
			img->_buffer[x][y]=c;
}

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

void I_changeColor(Image *img, Color c)
{
	img->_current_color = c;
}

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------
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

//------------------------------------------------------------------------

void I_zoomInit(Image *img)
{
	img->_xoffset = 0;
	img->_yoffset = 0;
	img->_zoom = 1.0;
}

//------------------------------------------------------------------------

void I_zoom(Image *img, double zoom_coef)
{
	img->_zoom = img->_zoom * zoom_coef;
}

//------------------------------------------------------------------------

void I_move(Image *img, int x, int y)
{
	img->_xoffset += x;
	img->_yoffset += y;
}

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

void Strip_line(Image *img) {
	Point pt1 = P_new(100, 100);
	Point pt2 = P_new(200, 200);
	Point pt3 = P_new(300, 50);

	I_bresenham(img, pt1.x, pt1.y, pt2.x, pt2.y);
	I_bresenham(img, pt2.x, pt2.y, pt3.x, pt3.y);
}

//------------------------------------------------------------------------

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

//------------------------------------------------------------------------

void Poly_select(Image *img, Polygone *poly, int pos) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL && i <= pos) {
				if(i == pos) {
					glBegin(GL_LINE_LOOP);
					glVertex2i((p_temp->pt.x)-5,(p_temp->pt.y)+5);
					glVertex2i((p_temp->pt.x)+5,(p_temp->pt.y)+5);
					glVertex2i((p_temp->pt.x)+5,(p_temp->pt.y)-5);
					glVertex2i((p_temp->pt.x)-5,(p_temp->pt.y)-5);
					glEnd();
				}
				else {
					p_temp = p_temp->next;
				}
				i++;
			}
		}
	}
}

//------------------------------------------------------------------------

void Poly_move(Image *img, Polygone *poly, int pos, int d) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;
			while(p_temp != NULL && i <= pos) {
				if(i == pos) {
					switch(d) {
						case 0 :
							//fprintf()
							p_temp->pt.y+=1;
							break;
						case 1 :
							p_temp->pt.y-=1;
							break;
						case 2 :
							p_temp->pt.x+=1;
							break;
						case 3 :
							p_temp->pt.x-=1;
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