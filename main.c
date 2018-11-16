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

/*0 -> polygone ouvert | 1 -> polygone fermé*/
int cl = 0;

/*position du point / de l'arête sélectionné(e)*/
int pos = 1;

/*direction de déplacement d'un point*/
int d = -1;

/*0 -> scan line désactivé | 1 -> scan line activé*/
int s = 0;

/*0 -> mode insert désactivé | 1 -> mode insert activé*/
int ins = 1;

/*0 -> mode vertex désactivé | 1 -> mode vertex activé*/
int ver = 0;

/*0 -> mode edge désactivé | 1 -> mode edge activé*/
int edg = 0;

//------------------------------------------------------------------
//	C'est le display callback. A chaque fois qu'il faut
//	redessiner l'image, c'est cette fonction qui est
//	appelee. Tous les dessins doivent etre faits a partir
//	de cette fonction.
//------------------------------------------------------------------
void display_CB() {
    glClear(GL_COLOR_BUFFER_BIT);
    Color black = {0, 0, 0};
    I_fill(img, black);
    glColor3ub(255,255,255);

    Poly_draw(img, poly, cl);

    /*fermeture du polygone*/
    if(cl == 1) {

      /*fermeture possible uniquement si le polygone est constitué d'au moins 2 points*/
      if(poly->length > (size_t)1) {
        I_bresenham(img, poly->last->pt.x, poly->last->pt.y, poly->first->pt.x, poly->first->pt.y);
      }
    }

    /*scan line*/
    if(s == 1) {

      /*scan line possible uniquement si le polygone est fermé*/
      if(cl == 1) {
        scan_line(img, poly);
      }
    }

    /*mode vertex*/
    if(ver == 1) {
      Color green = {0, 1, 0};

      /*sélection d'un sommet*/
      Poly_select(img, poly, pos, green);
    }

    /*mode edge*/
    else if(edg == 1) {
    	Color red = {1, 0, 0};

      /*sélection d'une arête*/
      Poly_selectE(img, poly, pos, red);
    }
    I_draw(img);
    glutSwapBuffers();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir en fonction de la position de
// la souris (x,y), en fonction du bouton de la souris qui a été
// pressé ou relaché.
//------------------------------------------------------------------
void mouse_CB(int button, int state, int x, int y) {

  /*clic gauche*/
  if((button==GLUT_LEFT_BUTTON)&&(state==GLUT_DOWN)) {
    I_focusPoint(img,x,img->_height-y);

    /*mode insert*/
	  if(ins == 1) {

      /*ajout d'un nouveau point (ajouté à la suite du dernier point)*/
      Poly_addPointLast(poly, x, y);
    }

    /*mode vertex*/
    else if(ver == 1) {

      /*récupération de la position du point le plus proche du clic*/
      pos = closestVertex(img, poly, x, y);
    }

    /*mode edge*/
    else if(edg == 1) {

      /*récupération de la position (du premier point) de l'arête la plus proche du clic*/
      pos = closestEdge(img, poly, x, y, cl);
    }
  }

  /*bouton du milieu*/
	else if((button==GLUT_MIDDLE_BUTTON)&&(state==GLUT_DOWN)) {

    /*mode vertex*/
    if(edg == 1) {

      /*ajout d'un point au milieu de l'arête sélectionnée*/
			Poly_addE(img, poly, pos);
		}
	}
	glutPostRedisplay();
}

//------------------------------------------------------------------
// Cette fonction permet de réagir au fait que l'utilisateur
// presse une touche (non-spéciale) du clavier.
//------------------------------------------------------------------
void keyboard_CB(unsigned char key, int x, int y) {
	switch(key)
	{
    /*echap*/
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
			break;
		case 127 :

      /*mode vertex*/
      if(ver == 1) {
        Poly_deleteP(img, poly, pos);

        /*
          si le point supprimé correspondait au dernier point du polygone
          et que celui-ci n'est pas vide, alors la valeur de la position
          est décrémentée de 1 pour ramener la sélection du point au "nouveau" dernier point
        */
        if((size_t)pos == (poly->length+1) && poly->length >= (size_t)1) {
          pos--;
        }

        /*
          si le point supprimé était le dernier point du polygone,
          alors la valeur de la position est réinitialisée à 1, pour afficher
          le premier point ou la première arête en cas de sélection après
          le dessin d'un nouveau polygone
        */
        if(poly->length == (size_t)0) {
          pos = 1;
        }
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
void special_CB(int key, int x, int y) {
	switch(key)
	{
		case GLUT_KEY_UP : d = 0; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_DOWN : d = 1; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_LEFT  : d = 2; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_RIGHT : d = 3; Poly_move(img, poly, pos, d); break;
		case GLUT_KEY_PAGE_DOWN :
      d = -1;

      /*mode vertex*/
      if(ver == 1) {

        /*
          si le point sélectionné n'est pas le dernier point du polygone,
          alors la valeur de la position est normalement incrémentée
        */
      	if((size_t)pos < poly->length) {
      		pos++;
        }

        /*si le point sélectionné est le dernier point du polygone...*/
        else if((size_t)pos == poly->length) {

          /*
            ...et si le polygone est fermé alors la valeur de la position
            est ramenée à 1 pour ramener la sélection au premier point du polygone
          */
          if(cl == 1) {
            pos = 1;
          }
        }
      }

      /*edge mode*/
    	else if(edg == 1) {

        /*
          si le point sélectionné n'est pas l'avant-dernier point du polygone,
          alors la valeur de la position est normalement incrémentée
        */
      	if((size_t)pos < (poly->length)-1) {
      		pos++;
      	}

        /*si le point sélectionné est l'avant-dernier point du polygone...*/
        else if((size_t)pos == (poly->length)-1) {

          /*
            ...et si le polygone est fermé alors
            la valeur de la position est normalement incrémentée
          */
          if(cl == 1) {
            pos++;
          }
        }

        /*
          si le point sélectionné est le dernier point du polygone,
          ce qui ne peut arriver que si le polygone est fermé,
          alors la valeur de la position est ramenée à 1 pour ramener
          la sélection à la première arête du polygone
        */
        else if((size_t)pos == poly->length) {
          pos = 1;
        }
    	}
    	break;
		case GLUT_KEY_PAGE_UP :

      /*
        si le point sélectionné n'est pas le premier point du polygone,
        alors la valeur de la position est normalement décrémentée
      */
			if(pos > 1) {
				pos--;
			}

      /*si le point sélectionné est l'avant-dernier point du polygone...*/
      else if(pos == 1) {

        /*
          ...et si le polygone est fermé alors la valeur de la position
          est ramenée au nombre de points du polygone
          pour ramener la sélection au dernier point du polygone
        */
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
    /*initialisation d'un nouveau polygone*/
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

		glutMainLoop();
		return 0;
	}
}
