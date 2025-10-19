# Proceduralni Plamen u OpenGL 4

Ovaj projekat demonstrira kako se može kreirati realistični efekat plamena koristeći proceduralnu generaciju u OpenGL 4.
Plamen je u potpunosti generisan u shader programima bez korišćenja tekstura.

## Pregled Projekta

Aplikacija prikazuje animirani plamen koji se proceduralno generiše koristeći matematičke funkcije, šum (noise) i
fraktalne algoritme. Ključna ideja je kombinacija:

- Osnovnog oblika plamena (teardrop shape)
- Višeslojnog šuma za turbulenciju i detalje
- Postepenog mešanja boja od jezgra do ivica
- Animacije kroz vreme za realistično kretanje i treperenje

## Tehnička Implementacija

### 1. Osnovni OpenGL Setup (main.cpp)

- Inicijalizacija GLFW i GLAD za OpenGL 4 kontekst
- Kreiranje jednostavnog kvadrata (dva trougla) kao osnove za plamen
- Podešavanje vertex i fragment shadera
- Implementacija alpha blending-a (`GL_BLEND` + `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`)
- Render petlja koja:
    - Ažurira uniformne promenljive (time, center, scale)
    - Renderuje kvadrat sa efektom plamena

```cpp
// Omogućavanje blending-a za transparenciju plamena
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Slanje vremena za animaciju u shader
const auto loc_time = glGetUniformLocation(shader, "time");
glUniform1f(loc_time, currentTime);
```

### 2. Vertex Shader

Vertex shader izvodi dve ključne funkcije:

- Primenjuje suptilni talasasti efekat za prirodniji izgled:
  ```glsl
  pos.x += sin(time * 2.0 + pos.y * 3.0) * 0.02;
  ```
- Skalira i pozicionira geometriju koristeći uniforme `center` i `scale`:
  ```glsl
  pos = pos * scale + center;
  ```

Ova jednostavna deformacija doprinosi organskom izgledu plamena, iako se većina vizuelnog efekta dešava u fragment
shaderu.

### 3. Fragment Shader - Srce Projekta

Fragment shader je najkompleksniji deo i odgovoran je za sam izgled plamena. Proces se može podeliti u nekoliko faza:

#### 3.1. Noise funkcije

Implementacija uključuje tri nivoa funkcija za generisanje šuma:

- **random(vec2 st)** - Pseudo-random generator koji koristi sinus i dot proizvod:
  ```glsl
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
  ```

- **noise(vec2 st)** - Bilinearna interpolacija između uglova ćelije:
  ```glsl
  // Interpolacija između četiri random vrednosti na ćelijskim uglovima
  return mix(corner00, corner10, blend.x) + 
         (corner01 - corner00) * blend.y * (1.0 - blend.x) + 
         (corner11 - corner10) * blend.x * blend.y;
  ```

- **fbm(vec2 st)** - Fractal Brownian Motion koji kombinuje više oktava noise-a:
  ```glsl
  // Akumulacija 5 oktava noise-a sa rastućom frekvencijom i opadajućom amplitudom
  for (int i = 0; i < 5; i++) {
      value += amplitude * noise(st * frequency);
      st *= 2.0;
      amplitude *= 0.5;
      frequency *= 1.1;
  }
  ```

#### 3.2. Generisanje oblika plamena (flameShape)

Funkcija `flameShape(vec2 position, float time)` kreira osnovni oblik plamena:

1. **Bazični oblik** - Kreiranje teardrop oblika šireg na dnu i užeg na vrhu:
   ```glsl
   float aspect = 1.7;  // Čini plamen višim nego širim
   flamePos.x *= 1.0 + flamePos.y * 0.8;  // Sužavanje ka vrhu
   float dist = length(vec2(flamePos.x, flamePos.y / aspect));
   float flame = 0.35 - dist;  // Bazični kružni oblik
   ```

2. **Odsecanje dna** - Da bi se dobio oblik plamena koji se uzdiže:
   ```glsl
   if (flamePos.y < -0.05) {
       flame -= (abs(flamePos.y) + 0.05) * 3.0;
   }
   ```

3. **Dodavanje turbulencije** - Dva sloja fbm sa različitim skalama:
   ```glsl
   float turb1 = fbm(vec2(flamePos.x * 4.0, flamePos.y * 4.0 + time * 1.5)) * 0.2;
   float turb2 = fbm(vec2(flamePos.x * 8.0 + sin(time * 0.3), flamePos.y * 2.0 + time)) * 0.1;
   flame += turb1 - turb2;
   ```

4. **Pulsiranje** - Dodatna animacija koja simulira plameni puls:
   ```glsl
   flame += 0.05 * sin(time * 3.0 + flamePos.y * 8.0);
   ```

#### 3.3. Bojenje i Alpha Kanali

Nakon generisanja oblika plamena, dodeljuju se boje i transparentnost:

1. **Mešanje boja** - Četiri bazične boje se mešaju prema intenzitetu plamena:
   ```glsl
   vec3 baseColor = vec3(1.0, 0.1, 0.0);     // Duboko crvena baza
   vec3 middleColor = vec3(1.0, 0.5, 0.0);   // Narandžasti srednji deo
   vec3 tipColor = vec3(1.0, 0.9, 0.0);      // Žuti vrh
   vec3 innerColor = vec3(1.0, 0.9, 0.6);    // Svetlo jezgro
   
   // Postupno mešanje boja od baze ka vrhu
   color = mix(color, middleColor, clamp(flame * 2.0, 0.0, 1.0));
   color = mix(color, tipColor, clamp(flame * 3.0 - 0.5, 0.0, 1.0));
   color = mix(color, innerColor, clamp(flame * 4.0 - 2.0, 0.0, 1.0));
   ```

2. **Varijacije u boji** - Dodatni sloj šuma za varijacije:
   ```glsl
   float colorNoise = fbm(position * 5.0 + time * 0.1) * 0.1;
   color += colorNoise;
   ```

3. **Alpha transparentnost** - Glatki prelaz na ivicama plamena:
   ```glsl
   float alpha = smoothstep(-0.05, 0.05, flame);
   ```

4. **Treperenje** - Variranje transparentnosti za efekat treperenja:
   ```glsl
   alpha *= 0.9 + 0.1 * sin(time * 10.0 + position.y * 20.0);
   ```

## Ključne tehnike

Projekat demonstrira nekoliko važnih tehnika računarske grafike:

1. **Proceduralno generisanje** - Plamen je matematički generisan, bez upotrebe tekstura
2. **Noise algoritmi** - Korišćenje value noise-a i fbm-a za prirodnu nepravilnost
3. **Alpha blending** - Za postizanje mekih ivica i transparencije
4. **Animacija kroz vreme** - Kontinuirano kretanje i treperenje
5. **Višeslojni pristup** - Kombinovanje različitih slojeva šuma i efekata

## Parametri za podešavanje

Projekat sadrži nekoliko parametara koje možete podešavati:

1. **U util.h:**
    - `winWidth`, `winHeight` - Dimenzije prozora
    - `centerX`, `centerY` - Pozicija plamena
    - `scale` - Veličina plamena

2. **U fragment.glsl:**
    - Parametri noise funkcije - Frekvencije i amplitude u fbm()
    - Boje plamena - `baseColor`, `middleColor`, `tipColor`, `innerColor`
    - Parametri animacije - Brzina i intenzitet treperenja

## Mogućnosti za unapređenje

1. **Performanse** - Optimizacija fbm funkcije za bolje performanse
2. **Interaktivnost** - Dodavanje mogućnosti da korisnik interaguje sa plamenom
3. **3D plamen** - Proširenje na 3D koristeći ray marching ili particle sisteme
4. **Post-procesiranje** - Dodavanje bloom efekta za veći osećaj svetlosti
5. **Fizički zasnovan model** - Implementacija modela baziranog na fizičkim principima širenja vatre

## Rezime

Fragment shader za svaki piksel izračunava:

- Da li pripada obliku plamena (na osnovu flameShape funkcije)
- Koju boju treba da ima (mešanje više boja na osnovu intenziteta)
- Koju transparentnost treba da ima (alpha kanal)
- Kako se menja tokom vremena (animacija kroz time uniformu)

Rezultat je vizuelno uverljiv, dinamičan plamen koji se kontinuirano menja i stvara iluziju organske vatre, sve bez
korišćenja tekstura ili sprajt animacija.
