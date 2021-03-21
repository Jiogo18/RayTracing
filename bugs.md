# Bugs connus

- [x] #1 trou noir
  - intersections demi droite interdites
  - fix : t<0

- [x] #2 pixels randoms (avec des ronds...)
  - en partit corrigé: #11 mais pas les faces avant/arrière
  - fix : doubli

- [ ] #3 avec plusieurs rayons pp les pixels randoms sont plus nombreux
  - à cause de int pppV?

- [x] #4 positions x, y, z changent les faces (z en hauteur)
  - fix : 4 bugs inversion determineColor :
    - de création des faces
    - getNextPoint
    - left-right entity
    - green et blue dans

- [x] #5 loupe / effet lentille (type judas)
  - fix : change ref dans getChildRot
  - (pb car on ne peut pas additionner les angles)

- [x] #6 ray parcours chunk,block,face
  - si la distance de la géométrie min du block est trop grande alors on le skip
  - fix chunkG, chunkGMid...

- [ ] #7 résultat de #1 il faudrait savoir si un chunk est derrière (demi-droite...)
  - pas prioritaire (optimisation)
  - possible: faire une sphère autour de l'objet, prendre le cercle perpendiculaire au ray et faire son carré le plus grand, comme pour une face

- [x] #8 se mettre sur une face fait un rendu de la permière face sans passer à travers
  - fix : variable "enter" mis à false et si c'est le même point alors on prend pas

- [x] #9 en -1.5, 1.5, 1.5 on voit la face verte et non la bleue (entièrement)
  - fix : pb avec Rect3D::getMiddle résolu en passant par le getMiddleGeometry

- [x] #10 les faces et les plans sont recalculés à chaque fois
  - fix : pointeurs

- [x] #11 point3D 1-1 = 10^-17...
  - fix: à toutes les fonctions avec + ou - on a une fonction verifOperator
  - [ ] meilleur fix à faire : changer les fonctions pour double::operator -(double)

- [x] #12 fonction sur les doubles de #11
  - pas prioritaire mais utile
  - et pareil pour qCos et qSin (si la marge est trop faible par rapport à 1...)
  - fix : doubli

- [x] #13 en 0,1.4,0.4 on voit la face magenta, mais en 0,1.4,0.5 on voit la face verte
  - relaté à #9, car à cette hauteur on ne voit plus la face verte
  - fix : #9

- [x] #14 doubli a ralenti le calcul (de #12 et indirectement #2)
  - il faudrait juste des fonctions qui surpasse celles de double
  - fix : juste une fonction round pour doubli

- [x] #15 relaté à #9 et #13 sur la face verte des fois ça traverse d'autres fois non
  - (quand on est sur la face bleue en même temps ou quand on est à des 0.5 ou 1 ou 0)

- [x] #16 à la suite de #4 les formes horizontales sont encore un peu rondes
  - fix : voir save 26/02/2021 (calcul avec asin(yPos / d))

- [x] #17 relaté à #15: en 0.5,0.5,1 on ne voit pas la face bleue
  - il faudrait quelques pixels pour (pas obligé)
  - fix : refusé

- [x] #18 ico

- [ ] #19 les blocks en z=-1 sont pas affichés

- [ ] #20 dans le miroir on a une ligne sur le plan xz

- [ ] #21 optimisation : on peut retirer les doubli ?

- [x] #22 arrondi : passé le sqrt(1+...) à sqrt(0.55+...) ça corrige un peu la distorsion, mais c'est toujours bizarre
  - (cf l'image des miroirs du 21/02/2021)
  - fix : comme #16

- [ ] #23 Rect3D avec 2 points
  - C'est une droite...
  - Problème pour les miroirs et le verre/l'eau

- [x] #24 Réfraction : le changement pour #23 a cassé le calcul de réfraction

- [ ] #24 Miroir : le changement pour #23 a cassé le calcul de réflexion
