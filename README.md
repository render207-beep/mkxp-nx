# mkxp-nx

Proyecto para llevar un reproductor de juegos de RPG Maker XP / VX / VX Ace a Nintendo Switch,
basado en mkxp-z (codigo abierto). No contiene codigo ni assets de ningun juego: cada usuario
aporta sus propios juegos.

## Estado: Hito 1 (prueba de OpenGL)

Este primer `.nro` solo comprueba que version de OpenGL / OpenGL ES puede crear la Switch con SDL2,
cuanta RAM tiene el proceso y si se puede dibujar. Con ese resultado decidimos como seguir.

## Como compilar sin saber compilar (desde el movil o el PC)

1. Crea una cuenta en github.com y un repositorio nuevo (publico o privado), por ejemplo `mkxp-nx`.
2. En el repositorio: **Add file > Upload files** y sube `main.c`, `Makefile` y `README.md`.
3. **Add file > Create new file**, en el nombre escribe exactamente `.github/workflows/build.yml`
   y pega dentro el contenido de `build.yml`. Pulsa **Commit changes**.
4. Abre la pestana **Actions**. Se ejecutara solo; si no, entra en "build" y pulsa **Run workflow**.
5. Cuando termine (circulo verde), abre la ejecucion y baja a **Artifacts**: descarga `mkxp-nx-hito1`
   (es un zip con `mkxp-nx.nro`).
6. Si sale en rojo, copia el texto del paso que fallo y mandamelo.

## Como probarlo en la Switch

1. Copia `mkxp-nx.nro` a la tarjeta SD, en `sdmc:/switch/mkxp-nx/`.
2. Lanzalo **manteniendo pulsado R al abrir un juego instalado** (asi tiene toda la RAM).
   Si lo abres desde el Album, tendra muy poca memoria y el dato no sera representativo.
3. La pantalla mostrara un resumen. Pulsa + para salir.
4. Mandame el archivo `sdmc:/switch/mkxp-nx/log.txt` (o una foto del resumen).

## Hoja de ruta

1. Prueba de OpenGL y memoria (este hito).
2. Compilar Ruby para Switch.
3. Compilar el resto de dependencias (PhysFS, libsigc++, pixman, SDL_sound, OpenAL, sintetizador MIDI).
4. Compilar mkxp-z y arrancar un juego sencillo.
5. Controles, selector de juegos y RTP.

## Licencias y creditos

mkxp-z y sus dependencias tienen sus propias licencias: hay que conservar sus avisos al redistribuir.
