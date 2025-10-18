## Fragment shader

* Da li pripada obliku plamena
* Koju boju treba da ima na osnovu pozicije
* Koju transparentnost treba da ima
* Kako se menja tokom vremena

## Kljucne komponente

1. Osnovna geometrija (main.cpp)

Kvadrat se renderuje kao 2 trougla pomoću indeksa.

1. Vertex shader

* Prima pozicije temena i primenjuje transformacije
* Dodaje subtilni wave efekat za prirodniji izgled:
* Skalira i pozicionira geometriju

1. Fragment shader

* flameShape
    * random() - pseudo-random generator
    * noise() - bilinear interpolacija između uglova ćelije
    * fbm() - Fractal Brownian Motion sa 5 oktava
* Turbulencija i animacija
    * Dodaju se 2 sloja noise-a sa različitim frekvencijama
    * Pulsiranje: 0.05 * sin(time * 3.0 + flamePos.y * 8.0)
* Bojenje plamena - Boje se mešaju na osnovu intenziteta plamena.

## Kluljcne tehnike

* Proceduralno generisanje - plamen se generiše matematički, ne koriste se teksture
* Alpha blending - omogućava transparentnost na krajevima plamena
* Noise algoritmi - stvaraju prirodan, haotičan izgled
* Animacija kroz vreme - time uniform se koristi za kontinuiranu animaciju
* Multi-layer approach - kombinuje se više slojeva noise-a za složenost

## Rezultat

Fragment shader za svaki piksel izračunava:

* Da li pripada obliku plamena
* Koju boju treba da ima na osnovu pozicije
* Koju transparentnost treba da ima
* Kako se menja tokom vremena