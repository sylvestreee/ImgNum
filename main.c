#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include "Polygone.h"

Image *img;
Polygone *poly;
int cl = 0;
int pos = 1;
int d = -1;

int ins = 1;
int ver = 0;

//------------------------------------------------------------------
//	C'est le display callback. A chaque fois qu'il faut
//	redessiner l'image, c'est cette fonction qui est
//	appelee. Tous les dessins doivent etre faits a partir
//	de cette fonction.
//------------------------------------------------------------------

void display_CB()
{
    glClear(GL_COLOR_BUFFER_BIT);
	I_draw(img);
    glColor3ub(255,255,255);
    if(ins == 1) {
    	Poly_draw(img, poly);
    }
    else if(ver == 1) {
    	Poly_select(img, poly, pos);
    }
    /*
    	if(d > 0) {
    		Poly_move(img, poly, pos, d);
    	}
    	if(cl == 1) {
    		I_bresenham(img, poly->first->pt.x, poly->first->pt.y, poly->last->pt.x, poly->last->pt.y);
    	}	
    */
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
		printf("x : %d ; y : %d\n", x, y);
		if(ins == 1) {
			Poly_addPointLast(poly, x, y);
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
	// fprintf(stderr,"key=%d\n",key);
	switch(key)
	{
		case 27 : Poly_delete(&poly); exit(1); break;
		case 'z' : I_zoom(img,2.0); break;
		case 'Z' : I_zoom(img,0.5); break;
		//case 'i' : I_zoomInit(img); break;
		case 'i' : ins = 1; ver = 0; break;
		case 'v' : ins = 0; ver = 1; break;
		/*
			case 99 : 
				if(cl == 0) {
					cl = 1;
				}
				else {
					cl = 0;
				} 
				break;
			case 38 : d = 0; break;
			case 40 : d = 1; break;
			case 37 : d = 2; break;
			case 39 : d = 3; break;
		*/
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
	// int mod = glutGetModifiers();

	int d = 10;

	switch(key)
	{
		case GLUT_KEY_UP    : I_move(img,0,d); break;
		case GLUT_KEY_DOWN  : I_move(img,0,-d); break;
		case GLUT_KEY_LEFT  : I_move(img,d,0); break;
		case GLUT_KEY_RIGHT : I_move(img,-d,0); break;

		//mode vertex
		if(ver == 1) {
			case 105 : if((size_t)pos != poly->length) pos++; break; // page suivante
			case 104 : if(pos != 1) pos--; break; // page précédente
		}

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
			/*
        Color rouge = C_new(100,0,0);
			  Color blanc = C_new(200,200,255);
        I_checker(img,rouge,blanc,50);
      */
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

		//glOrtho(0,largeur,0,hauteur,-1,1);
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
