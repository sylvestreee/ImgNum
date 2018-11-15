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

    Poly_draw(img, poly, cl);
    /*mode vertex*/
    if(ver == 1) {
      Color green = {0, 1, 0};
      Poly_select(img, poly, pos, green);
    }

    /*mode edge*/
    else if(edg == 1) {
      if((size_t)pos == poly->length) {
        d = -1;
        pos--;
      }
    	Color red = {1, 0, 0};
      Poly_selectE(img, poly, pos, red);
    }

    /*scan_line*/
    if(s == 1) {
      if(cl == 1) {
        scan_line(img, poly);
      }
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
      pos = closestEdge(img, poly, x, y, cl);
    }
  }

	else if((button==GLUT_MIDDLE_BUTTON)&&(state==GLUT_DOWN)) {

    /*mode vertex*/
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
		case 'i' : ins = 1; ver = 0; edg = 0; d = -1; break;
		case 'v' : ins = 0; ver = 1; edg = 0; d = -1; break;
		case 'e' : ins = 0; ver = 0; edg = 1; d = -1; break;
		case 'f' :
      s = !s;
			break;
		case 'c' :
      cl = !cl;

      /*fermeture du polygone*/
      if(cl == 1) {
        if(poly != NULL && poly->length >= 3) {
          poly->last->next = poly->first;
        }
      }

      /*ouverture du polygone*/
      else {
        poly->last->next = NULL;
      }
			break;
		case 127 :
      if(ver == 1) {
        Poly_deleteP(img, poly, pos, cl);
        if((size_t)pos == (poly->length+1) && poly->length >= (size_t)1) {
          pos--;
        }
        if(poly->length == (size_t)0) {
          pos = 1;
        }
        printf("pos : %d\n",pos);
      }
      break;

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
		case GLUT_KEY_UP : d = 0; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_DOWN : d = 1; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_LEFT  : d = 2; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_RIGHT : d = 3; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_PAGE_DOWN :
      d = -1;
      //printf("pos : %d\n",pos);
      //printf("length : %d\n", (int)poly->length);

      /*mode vertex*/
      if(ver == 1) {
      	if((size_t)pos < poly->length) {
      		pos++;
        }
        else if((size_t)pos == poly->length) {
          if(cl == 1) {
            pos = 1;
          }
        }

        /*polygone fermé -> arrivé au dernier point
        if(cl == 1) {
          if((size_t)pos > poly->length) {
            printf("hey\n");
            pos = 1;
          }
        }*/
      }

      /*edge mode*/
    	else if(edg == 1) {
      	if((size_t)pos != (poly->length)-1) {
      		pos++;
      	}

        /*polygone fermé*/
        if(cl == 1) {

          /*arrivé à l'avant-dernier point*/
          if((size_t)pos == (poly->length)-1) {
            pos++;
          }

          /*arrivé au dernier point*/
          else if((size_t)pos == poly->length) {
            pos = 1;
          }
        }
    	}
    	break;
		case GLUT_KEY_PAGE_UP :
			if(pos > 1) {
				pos--;
			}
      else if(pos == 1) {
        if(cl == 1) {
          pos = (int)poly->length;
        }
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
