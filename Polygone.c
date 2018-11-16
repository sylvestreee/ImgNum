#include "Polygone.h"

//---------------------------------------------------------------------------
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
				pt_new->next = NULL;
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
	}
}

//---------------------------------------------------------------------------
void Poly_addPointOrder(Polygone *poly, int x, int y) {
	int pos = 1;
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			if(p_temp != NULL) {
				if(x <= poly->first->pt.x) {
					Poly_addPointFirst(poly, x, y);
				}
				else if(x >= poly->last->pt.x) {
					Poly_addPointLast(poly, x, y);
				}
				else {
			 		while(x > p_temp->pt.x) {
			 			p_temp = p_temp->next;
						pos++;
					}
					Poly_addPoint(poly, x, y, pos);
				}
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
						pt_new->next = pt_temp;
						pt_new->prev = pt_temp->prev;
						pt_temp->prev->next = pt_new;
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
//	Prend en paramètre une image (type Image) et un polygone (type Polygone).
//	Parcourt la chaîne et trace une ligne de Bresenham pour chaque point
//	avec son successeur, tant que celui-ci existe.
//---------------------------------------------------------------------------
void Poly_draw(Image *img, Polygone *poly, int cl) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;

			/*parcours de la chaîne tant que le dernier point n'est pas atteint*/
			while(p_temp->next != NULL) {

				/*dessin d'une droite de Bresenham entre le point courant et son successeur*/
				I_bresenham(img, p_temp->pt.x, p_temp->pt.y, p_temp->next->pt.x, p_temp->next->pt.y);
				p_temp = p_temp->next;
			}
		}
	}
}

//---------------------------------------------------------------------------
//	Prend en paramètre une image (type Image) et un polygone (type Polygone).
//	Comportement identique à Poly_draw, à la différence que les points
// 	sont choisis deux à deux.
//---------------------------------------------------------------------------
void Poly_drawSc(Image *img, Polygone *poly) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			while(p_temp != NULL && p_temp->next != NULL) {
				I_bresenham(img, p_temp->pt.x, p_temp->pt.y, p_temp->next->pt.x, p_temp->next->pt.y);

				/*blablabla*/
				p_temp = p_temp->next->next;
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
void Poly_select(Image *img, Polygone *poly, int pos, Color c) {
	if(poly != NULL) {
		if(poly->first != NULL) {
			struct node *p_temp = poly->first;
			int i = 1;

			/*parcours de la chaîne tant que la position souhaitée n'est pas atteinte*/
			while(p_temp != NULL && i <= pos) {
				if(i == pos) {

					/*une fois la position atteinte, dessin d'un carré coloré autour du point associé à cette position*/
					I_bresenhamColor(img, (p_temp->pt.x)-5,(p_temp->pt.y)+5,(p_temp->pt.x)+5,(p_temp->pt.y)+5,c);
					I_bresenhamColor(img, (p_temp->pt.x)+5,(p_temp->pt.y)+5,(p_temp->pt.x)+5,(p_temp->pt.y)-5,c);
					I_bresenhamColor(img, (p_temp->pt.x)+5,(p_temp->pt.y)-5,(p_temp->pt.x)-5,(p_temp->pt.y)-5,c);
					I_bresenhamColor(img, (p_temp->pt.x)-5,(p_temp->pt.y)-5,(p_temp->pt.x)-5,(p_temp->pt.y)+5,c);
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
			if(poly->length != (size_t)1) {
				p_temp->next->prev = NULL;
				poly->first = p_temp->next;
				free(p_temp);
				poly->length--;
			}

			/*
				si le premier point est aussi le dernier point de la chaîne,
				alors est réinitialisé le premier et dernier point de la chaîne à NULL,
				ainsi que la longueur de la chaîne à 0
			*/
			else {
				free(p_temp);
				poly->first = NULL;
				poly->last = NULL;
				poly->length = 0;
			}
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
			if(poly->length != (size_t)1) {
				p_temp->prev->next = NULL;
				poly->last = p_temp->prev;
				free(p_temp);
				poly->length--;
			}
			else {
				free(p_temp);
				poly->first = NULL;
				poly->last = NULL;
				poly->length = 0;
			}
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

						/*déplacement du point vers le haut*/
						case 0 :
							p_temp->pt.y-=1;
							break;

						/*déplacement du point vers le bas*/
						case 1 :
							p_temp->pt.y+=1;
							break;

						/*déplacement du point vers la gauche*/
						case 2 :
							p_temp->pt.x-=1;
							break;

						/*déplacement du point vers la droite*/
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

				/*l'indice du premier point de l'arête correspond à l'indice de l'arête*/
				if(i == pos) {
					if(p_temp->next != NULL) {
						I_bresenhamColor(img, p_temp->pt.x, p_temp->pt.y, p_temp->next->pt.x, p_temp->next->pt.y, c);
					}

					/*
						si la position atteinte est celle du dernier point,
						alors cela veut dire que le polygone est fermé
						donc dessin normal d'une droite colorée entre le dernier point
						et le premier point
					*/
					else {
						I_bresenhamColor(img, poly->last->pt.x, poly->last->pt.y, poly->first->pt.x, poly->first->pt.y, c);
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
					if(p_temp->next != NULL) {
						x_n = (p_temp->pt.x + p_temp->next->pt.x)/2;
						y_n = (p_temp->pt.y + p_temp->next->pt.y)/2;
						Poly_addPoint(poly, x_n, y_n, pos+1);
					}
					else {
						x_n = (poly->last->pt.x + poly->first->pt.x)/2;
						y_n = (poly->last->pt.y + poly->first->pt.y)/2;
						Poly_addPointLast(poly, x_n, y_n);
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
int closestEdge(Image *img, Polygone *poly, int x, int y, int cl) {
	int min, indice, dist, x_n, y_n;
	if(poly != NULL) {
		if(poly->first != NULL) {
			int i = 1;
			struct node *p_temp = poly->first;

			while(p_temp->next != NULL) {
				x_n = (p_temp->pt.x + p_temp->next->pt.x)/2;
				y_n = (p_temp->pt.y + p_temp->next->pt.y)/2;
				dist = sqrt(pow(x-x_n, 2) + pow(y-y_n, 2));
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

			if(cl == 1) {
				x_n = (poly->last->pt.x + poly->first->pt.x)/2;
				y_n = (poly->last->pt.y + poly->first->pt.y)/2;
				dist = sqrt(pow(x-x_n, 2) + pow(y-y_n, 2));
				if(dist < min) {
					min = dist;
					indice = i;
				}
			}
		}
	}
	return indice;
}
