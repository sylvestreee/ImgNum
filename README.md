# Fondements et Algorithmes de l'Imagerie Numérique - Projet
Projet réalisé dans le cadre de l'UE "Fondements et Algorithmes de l'Imagerie Numérique"

## Installation des dépendances

    sudo apt-get install freeglut3-dev

## Objectif

L’objectif de ce projet a été d'écrire un programme en C permettant à un utilisateur de dessiner un polygone plan et d'effectuer diverses opérations sur celui-ci.

## Lancement du programme

Après ouverture d'un terminal, écrire la commande `make` pour compiler le programme et le lancer avec `./plot [largeur] [valeur]`

## Manuel d'emploi

Le programme permet à l'utilisateur d'accéder à quelques fonctionnalités, dont les suivantes :

### Fermeture du polygone

Il est possible de fermer (respectivement ouvrir) le polygone à l'aide de la touche `c` :

![](illustrations/close.gif)

### Scan-line

La fonctionnalité `scan-line` permet de remplir (respectivement vider) le polygone grâce à la touche `f` :

![](illustrations/scanline.gif)

### Modes

Le programme possède également 3 modes différents :

#### Mode insert

Ce mode s'active en pressant la touche `i`. Il permet à l'utilisateur de créer des points sur le plan à l'aide du clic gauche de la souris :computer_mouse:, chaque nouveau point étant relié au précédent. Sur l'exemple ci-dessous, nous pouvons voir la création de trois points successifs correctement reliés entre eux par des droites de Bresenham :

![](illustrations/insert.gif)

#### Mode vertex

Ce mode, qui s'active en pressant la touche `v` possède 3 fonctionnalités :

##### fonctionnalité #1

La première fonctionnalité consiste à sélectionner un sommet du polygone, qui sera mis en évidence par rapport aux autres par un carré de couleur vert :

![](illustrations/vertex.gif)

La sélection des sommets peut se faire de deux manières : soit en manipulant les touches `page suivante` :arrow_double_down: et `page précédente` :arrow_double_up: pour atteindre respectivement le sommet suivant et le sommet précédent ; soit en cliquant à l'aide du `clic gauche` de la souris :computer_mouse: sur le plan, le sommet le plus proche du clic étant celui sélectionné.

Il n'est cependant pas possible de passer du premier point au dernier (respectivement du dernier au premier point) à l'aide des touches si le polygone n'est pas fermé. Ci-dessous se trouve une illustration de cette possibilité :

![](illustrations/vertex_close.gif)

##### fonctionnalité #2

La deuxième fonctionnalité permet de déplacer le point sélectionné vers le haut, le bas, la gauche ou la droite à l'aide des touches :arrow_up:, :arrow_down:, :arrow_left: et :arrow_right:. L'illustration suivante montre le déplacement d'un sommet vers la droite :

![](illustrations/move.gif)

##### fonctionnalité #3

La troisième et dernière fonctionnalité consiste à effacer le sommet sélectionné à l'aide de la touche `suppr` :

![](illustrations/suppr.gif)

Il est possible de supprimer l'intégralité du polygone, puis de redessiner à nouveau (ne pas oubliez de revenir au mode `insert` pour cela, les fonctionnalités d'un mode étant désactivées en passant à un autre mode).

#### Mode edge

Ce mode s'active à l'aide de la touche `e` et possède 2 fonctionnalités :

##### fonctionnalité #1

La première fonctionnalité est similaire à celle du mode `vertex` : les touches `page suivante` :arrow_double_down: et `page précédente` :arrow_double_up: permettent de passer à l'arête suivante ou précédente et le `clic gauche` de la souris :computer_mouse: permet de sélectionner l'arête la plus proche, l'arête sélectionnée étant coloriée en rouge :

![](illustrations/edge.gif)

Il n'est pas possible de parcourir le polygone à l'aide des touches si celui-ci n'est pas fermé. Cette possibilité est illustrée ci-dessous :

![](illustrations/edge_close.gif)

##### fonctionnalité #2

La deuxième et dernière fonctionnalité permet à l'utilisateur d'ajouter à l'aide du `bouton du milieu` de la souris :computer_mouse: un point au milieu de l'arête sélectionnée :

![](illustrations/add.gif)

## Exemple d'utilisation

![](illustrations/test.gif)

Pour réaliser les mêmes opérations que l'exemple ci-dessus, suivre les instructions suivantes :

1.  Ouvrez un terminal et compilez le programme à l'aide de la commande `make`.

2.  Lancez le programme en ouvrant une fenêtre en utilisant la commande `./plot 400 400`.

3.  Vous commencez en mode `insert`. Entrez successivement les points à l'aide du `clic gauche` de la souris :computer_mouse:.

4.  Fermez le polygone à l'aide de la touche `c`.

5.  Pour remplir le polygone, appuyez sur la touche `f` et réappuyer à nouveau sur cette même touche pour le vider.

6.  Entrez en mode `edge` en appuyant sur la touche `e`, la première arête est sélectionnée automatiquement.

7.  Appuyez à répétition sur la touche `page suivante` :arrow_double_down: pour parcourir le polygone jusqu'à revenir à la première arête.

8.  Créez un nouveau sommet au milieu de la première arête en appuyant sur le `bouton du milieu` de la souris :computer_mouse:.

9.  Entrez en mode `vertex` en appuyant sur la touche `v`, le premier sommet est sélectionné.

10. Appuyez à répétition sur la touche `page précédente` :arrow_double_up: pour parcourir le polygone en sens inverse jusqu'à revenir au premier sommet.

11. Déplacez le premier sommet sur la gauche puis vers le haut, à l'aide respectivement des touches :arrow_left: et :arrow_up:.

12. Enfin, supprimez le premier sommet à l'aide de la touche `suppr`.

13. Appuyez sur `echap` pour quitter le programme.
