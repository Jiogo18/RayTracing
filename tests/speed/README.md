
# doubli

- un nombre pour gérer 6 décimales max (arrondit)
  - long double : précision sur 64 bits
  - double : précision sur 53 bits => pas assez
  - float : précision sur 24 bits

Optimisations :
round avec un cast à l'int

# #defines

- `#define ABS(x) ((x)<0 ? -(x) : (x))`
les parenthèses sont importantes (au moins pour -(x))
sinon il peut inverser :
  - Sans parenthèses : ABS(-6-3) => (6-3)   => 3
  - Avec parenthèses : ABS(-6-3) => -(-6-3) => 9

- `#define sqr(x) ((x) * (x))`
Idem, les parenthèses sont importantes (celles à l'intérieur)
