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
| 148*100 pixels        | 10 ms, 100 FPS        |
| 1920*1080 pixels      | 600 ms, 1.65 FPS      |

- Chaque pixel correspond à 1 rayon, mais il est possible d'en mettre plus.
- Seul le processeur est utilisé pour générer l'image avec
  - Deux threads par coeurs du CPU
  - 100 % du CPU par rafraichissement (le rafraichissement automatique est donc limité par un timer)
- Le GPU est un peu utilisé afin d'afficher l'image à l'écran, mais il n'y a pas besoin d'une carte graphique dédiée

---

## Fonctionnalités

- Déplacement dans un monde de cubes et de dalles
- Réflexion
- Réfraction
- Gestion du niveau de luminosité

---

## Comment l'obtenir ?

Outils nécessaires :

- [Git](https://git-scm.com/downloads)
- [CMake](https://cmake.org/download/)
- L'utilisation de MinGW est recommandée (via Qt par exemple)


Télécharger le projet et les librairies externes :
```bash
git clone https://github.com/Jiogo18/RayTracing
git submodule update --init --recursive
```

Compiler les librairies externes :
```bash
# freeglut.lib et freeglut.dll :
cd libaries/freeglut
mkdir build
cd build
"*" > .gitignore # Ignorer le dossier build
cmake -G "MinGW Makefiles" ..
cmake --build .

# OpenCL.lib et OpenCL.dll
cd ../../OpenCL-SDK
mkdir build
cd build
"*" > .gitignore # Ignorer le dossier build
cmake ..
cmake --build . --config Release
cp 
# opengl32.lib et glu32.lib : Windows Kits / Windows 10 SDK
```


Compiler le projet :
```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
```


---

## Comment l'utiliser ?

- F5 : Actualiser l'image à tout moment
- F6 : Compteur de FPS
- F7 : 2 secondes de rafraichissement continue pour calculer le FPS
- zqsd : Déplacer la caméra
- espace/shift : Monter et descendre
- Flèches directionnelles : orientation de la caméra

---

## Crédits

J'utilise les textures d'un ancien pack de ressources de Minecraft afin de simplifier la conception.
