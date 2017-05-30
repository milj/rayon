
Wywolanie programu

tryb interaktywny:
   rayon -i plik.mgf

tryb wsadowy (raytracer):
   rayon rt.cfg plik1.mgf ... plikN.mgf image.ppm

tryb mapowania tonow:
   rayon -ev plik.hdr plik.ppm    EV (-8  ...  20 1/3  co 1/3)
   rayon -tm plik.hdr plik.ppm    Tumblin-Rushmeier
   rayon -tw plik.hdr plik.ppm    Ward
   rayon -tg plik.hdr plik.ppm    gamma + skalowanie
   rayon -tl plik.hdr plik.ppm    skalowanie


Zaimplementowane triki przyspieszajace:

- kd-drzewa z heurystyka wyboru plaszczyzny podzialu
  oparta na sredniej wazonej mediany przestrzennej i mediany obiektow
- promien odbity od powierzchni nie jest sledzony, jesli powierzchnia
  w ogole nie jest odbijajaca, podobnie dla promienia przepuszczonego


Zaimplementowane elementy Monte Carlo, poprawnosci fizycznej i mapowania tonow:

- path tracer
- BRDF Warda
- rosyjska ruletka do terminowania sciezek
- oswietlenie bezposrednie
- Mapowanie Tumblin-Rushmeier i Ward
- probkowanie wazone dla odbicia rozproszonego i zwierciadlanego


Obsluga programu w trybie interaktywnym

a - obrot w lewo
d - obrot w prawo
w - obrot w gore
s - obrot w dol
z - obrot wokol kierunku patrzenia w lewo
x - obrot wokol kierunku patrzenia w prawo

cursor_left - przesuniecie w lewo
cursor_right - przesuniecie w prawo
cursor_up - przesuniecie w przod
cursor_down - przesuniecie w tyl
home - przesuniecie w gore
end - przesuniecie w dol

l - zoom out
o - zoom in

b - solid
n - wireframe
m - swiatla

r - raytracing
t - powrot do przegladania sceny (po raytracingu)
v - zapis konfiguracji

esc - wyjscie

