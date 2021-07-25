
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
| 02/04/2021 |     38      |   26 ms/frame   |      0.41       |  2400 ms/frame   |
| 03/04/2021 |     39      |   25 ms/frame   |      0.42       |  2380 ms/frame   |
| 03/04/2021 |     65      |   15 ms/frame   |      0.83       |  1200 ms/frame   |
| 03/04/2021 |     79      |   12 ms/frame   |       1.1       |   900 ms/frame   |
| 04/04/2021 |     96      |   10 ms/frame   |       1.4       |   720 ms/frame   |
| 05/04/2021 |     102     |   9 ms/frame    |      1.45       |   690 ms/frame   |
| 05/04/2021 |     102     |   9 ms/frame    |      1.65       |   600 ms/frame   |
| 22/07/2021 |     112     |   8 ms/frame    |      3.26       |   305 ms/frame   |
| 23/07/2021 |     125     |   8 ms/frame    |      3.51       |   284 ms/frame   |
| 24/07/2021 |     120     |   8 ms/frame    |      3.20       |   312 ms/frame   |
| 25/07/2021 |     126     |   7 ms/frame    |      3.55       |   281 ms/frame   |
| 25/07/2021 |     128     |   7 ms/frame    |      3.60       |   277 ms/frame   |

# Speed without process

Speed max if the ray.process is 0 % of the time (max of # Speed)

| Date       | FPS 150*100 | Time mini frame | FPS Full screen | Time Full screen |
| :--------- | :---------: | :-------------: | :-------------: | :--------------: |
| 04/04/2021 |     140     |   7 ms/frame    |       3.7       |   270 ms/frame   |
| 05/04/2021 |     140     |   7 ms/frame    |       15        |   67 ms/frame    |
| 23/07/2021 |     136     |   7 ms/frame    |       20        |   49 ms/frame    |
| 24/07/2021 |     126     |  7 msec/frame   |       20        |   50 ms/frame    |
| 24/07/2021 |     130     |  7 msec/frame   |      15.7       |   63 ms/frame    |
| 25/07/2021 |     136     |  7 msec/frame   |      21.2       |   47 ms/frame    |

# Speed without worker distributor

Speed max if the Worker Distributor isn't started (no process)
The time is mostly used by "paint" (with setPixel)

| Date       | FPS 150*100 | Time mini frame | FPS Full screen | Time Full screen |
| :--------- | :---------: | :-------------: | :-------------: | :--------------: |
| 23/07/2021 |    1127     |   0 ms/frame    |       74        |   13 ms/frame    |
| 24/07/2021 |    1426     |   0 ms/frame    |       74        |   13 ms/frame    |
| 25/07/2021 |    1505     |   0 ms/frame    |      80.8       |   12 ms/frame    |
