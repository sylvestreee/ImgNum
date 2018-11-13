#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <math.h>

#include "Image.h"
#include "Polygone.h"
#include "Scanline.h"

Image *img;
Polygone *poly;

int cl = 0;
int pos = 1;
int d = -1;
int s = 0;

/*modes*/
int ins = 1;
int ver = 0;
int edg = 0;

//------------------------------------------------------------------
//	C'est le display callback. A chaque fois qu'il faut
//	redessiner l'image, c'est cette fonction qui est
//	appelee. Tous les dessins doivent etre faits a partir
//	de cette fonction.
//------------------------------------------------------------------

void display_CB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Color black = {0, 0, 0};
    I_fill(img, black);
    glColor3ub(255,255,255);

    /*mode insert*/
    if(ins == 1) {
    	Poly_draw(img, poly);
    }

    /*mode vertex*/
    else if(ver == 1) {
    	Poly_draw(img, poly);

      /*clavier*/
      Poly_select(img, poly, pos);

      /*souris*/
    	//Poly_select(img, poly, pos);

    	if(d >= 0) {
    		Poly_move(img, poly, pos, d);
    	}
    }

    /*mode edge*/
    else if(edg == 1) {
    	Color red = {1, 0, 0};
    	Poly_draw(img, poly);

      /*clavier*/
      Poly_selectE(img, poly, pos, red);

    	/*souris*/
    	//Poly_selectE(img, poly, pos, red);
    }

    /*fermer le polygone*/
    if(cl == 1) {
    	I_bresenham(img, poly->first->pt.x, poly->first->pt.y, poly->last->pt.x, poly->last->pt.y);
    }

    /*scan_line*/
    if(s == 1) {
    	scan_line(img, poly);
    }
    I_draw(img);
    glutSwapBuffers();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir en fonction de la position de
// la souris (x,y), en fonction du bouton de la souris qui a été
// pressé ou relaché.
//------------------------------------------------------------------

void mouse_CB(int button, int state, int x, int y)
{
  if((button==GLUT_LEFT_BUTTON)&&(state==GLUT_DOWN)) {
    I_focusPoint(img,x,img->_height-y);

    /*mode insert*/
	  if(ins == 1) {
      Poly_addPointLast(poly, x, y);
    }

    /*mode vertex*/
    else if(ver == 1) {
      pos = closestVertex(img, poly, x, y);
    }

    /*mode edge*/
    else if(edg == 1) {
      pos = closestEdge(img, poly, x, y);
    }
  }
	else if((button==GLUT_MIDDLE_BUTTON)&&(state==GLUT_DOWN)) {
		if(edg == 1) {
			Poly_addE(img, poly, pos);
		}
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche (non-spéciale) du clavier.
//------------------------------------------------------------------

void keyboard_CB(unsigned char key, int x, int y)
{
	/*fprintf(stderr,"key=%d\n",key);*/

	switch(key)
	{
		case 27 : Poly_delete(&poly); exit(1); break;
		case 'z' : I_zoom(img,2.0); break;
		case 'Z' : I_zoom(img,0.5); break;
		/*case 'i' : I_zoomInit(img); break;*/
		case 'i' : ins = 1; ver = 0; edg = 0; d = -1; break;
		case 'v' : ins = 0; ver = 1; edg = 0; d = -1; break;
		case 'e' : ins = 0; ver = 0; edg = 1; d = -1; break;
		case 'f' :
			if(s == 0) {
				s = 1;
			}
			else {
				s = 0;
			}
			break;
		case 99 :
			if(cl == 0) {
				cl = 1;
			}
			else {
				cl = 0;
			}
			break;
		case 127 : Poly_deleteP(img, poly, pos); break;

		default : fprintf(stderr,"keyboard_CB : %d : unknown key.\n",key);
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche spéciale (F1, F2 ... F12, home, end, insert,
// haut, bas, droite, gauche etc).
//------------------------------------------------------------------

void special_CB(int key, int x, int y)
{
	/*
    int mod = glutGetModifiers();
	  int d = 10;
  */

	switch(key)
	{
		case 101 : d = 0; break;
		case 103 : d = 1; break;
		case 100  : d = 2; break;
		case 102 : d = 3; break;
		case 105 :
      /*mode vertex*/
      if(ver == 1) {
      	if((size_t)pos != poly->length) {
      		d = -1;
      		pos++;
        }
      }

      /*edge mode*/
    	else if(edg == 1) {
      	if((size_t)pos != (poly->length)-1) {
      		d = -1;
      		pos++;
      	}
    	}
    	break;
		case 104 :
			if(pos != 1) {
				d = -1;
				pos--;
			}
			break;

		default : fprintf(stderr,"special_CB : %d : unknown key.\n",key);
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------------

int main(int argc, char **argv)
{
	if((argc!=3)&&(argc!=2))
	{
		fprintf(stderr,"\n\nUsage \t: %s <width> <height>\nou",argv[0]);
		fprintf(stderr,"\t: %s <ppmfilename> \n\n",argv[0]);
		exit(1);
	}
	else
	{
		poly = Poly_new();
		int largeur, hauteur;
		if(argc==2)
		{
			img = I_read(argv[1]);
			largeur = img->_width;
			hauteur = img->_height;
		}
		else
		{
			largeur = atoi(argv[1]);
			hauteur = atoi(argv[2]);
			img = I_new(largeur,hauteur);
		}
		int windowPosX = 100, windowPosY = 100;

		glutInitWindowSize(largeur,hauteur);
		glutInitWindowPosition(windowPosX,windowPosY);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
		glutInit(&argc, argv);
		glutCreateWindow(argv[0]);

		glViewport(0, 0, largeur, hauteur);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glOrtho(0,largeur,hauteur,0,-1,1);

		glutDisplayFunc(display_CB);
		glutKeyboardFunc(keyboard_CB);
		glutSpecialFunc(special_CB);
		glutMouseFunc(mouse_CB);
		// glutMotionFunc(mouse_move_CB);
		// glutPassiveMotionFunc(passive_mouse_move_CB);

		glutMainLoop();
		return 0;
	}
}
