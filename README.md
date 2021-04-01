# RayTracing

Un Moteur 3D qui utilise la technique du Ray Tracing.

---

## Quel est l'objectif ?

Pour le moment, il est uniquement possible d'afficher des cubes et des demis cubes.
Le projet est terminé, je souhaite continuer pour optimiser encore.

---

## Performances

| Dimensions de l'écran | Temps de calcul moyen |
| --------------------- | --------------------- |
| 148*100 pixels        | 27 msec, 37 fps       |
| 1920*1080 pixels      | 2.5 secondes          |

- Chaque pixel correspond à 1 rayon mais il est possible d'en mettre plus.
- Seul le processeur est utilisé pour générer l'image
  - Auttant de threads que le nombre de coeur du cpu
  - Le CPU est à 100 % pendant le calcul de l'image, le rafraichissement est donc limité
- Le GPU est un peu utilisé afin d'afficher l'image à l'écran mais il n'y a pas besoin d'une carte graphique dédiée

---

## Comment l'obtenir ?

Via le code source :

- Télécharger le code source
- Télécharger [Qt](https://www.qt.io/download) avec la version 6.0
  - Sélectionner le composant MinGW (recommandé : version 8.1 ou supérieur)
- Ouvrir le projet
- Compiler le projet, le programme devrait être lancé

---

## Comment l'utiliser ?

- F5 : Actualiser l'image à tout moment
- F7 : 2 secondes de rafraichissement continue pour calculer le FPS
- zqsd : Déplacer la caméra
- espace/shift : Monter et descendre
- Flèches directionnelles : orientation de la caméra

---

## Crédits

J'utilise les textures d'un ancien pack de ressources de Minecraft afin de simplifier la conception.
