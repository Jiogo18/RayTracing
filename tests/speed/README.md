
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

---

# Speed

| Date       | FPS 150*100 | Time mini frame | FPS Full screen | Time Full screen |
| :--------- | :---------: | :-------------: | :-------------: | :--------------: |
| 16/03/2021 |      5      |  194 ms/frame   |      0.12       |  8200 ms/frame   |
| 29/03/2021 |    21.5     |   46 ms/frame   |      0.28       |  3500 ms/frame   |
| 30/03/2021 |     24      |   43 ms/frame   |      0.36       |  2800 ms/frame   |
| 31/03/2021 |    28.5     |   34 ms/frame   |      0.39       |  2600 ms/frame   |
| 31/03/2021 |     36      |   27 ms/frame   |      0.38       |  2600 ms/frame   |
| 31/03/2021 |     37      |   27 ms/frame   |      0.41       |  2450 ms/frame   |
