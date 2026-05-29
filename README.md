[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/BtHP8Xvz)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=24036326&assignment_repo_type=AssignmentRepo)
# Pac-Man Simplificado en SDL2

Proyecto integrador para la materia de Fundamentos de Programación.

El objetivo del proyecto es desarrollar un videojuego tipo Pac-Man utilizando:

- Lenguaje C
- SDL2
- Programación estructurada
- Funciones
- Arreglos
- Archivos
- Modularización

---

# Objetivos Académicos

Durante el desarrollo del proyecto deberán aplicarse los conceptos de:

- Resolución de problemas
- Diagramas de flujo
- Pseudocódigo
- Funciones
- Arreglos unidimensionales y bidimensionales
- Manejo de archivos
- Modularización
- Trabajo colaborativo

---

# Características Generales

El juego deberá permitir:

- Movimiento del jugador
- Colisiones con paredes
- Consumo de pellets
- Sistema de puntaje
- Vidas
- Fantasmas con comportamiento simple
- Carga de mapas desde archivos
- Guardado de puntajes

---

# Restricciones

NO se permite:

- C++
- Motores de videojuegos
- IA avanzada
- Pathfinding complejo
- Generar el proyecto completo usando IA sin comprensión

El equipo debe ser capaz de explicar completamente el funcionamiento del proyecto.

---

# Uso de Inteligencia Artificial

El uso de herramientas de IA es obligatorio como apoyo al desarrollo.

La IA puede utilizarse para:

- resolver dudas,
- depurar errores,
- generar ideas,
- sugerir estructuras,
- comprender SDL2,
- generar pseudocódigo.

Sin embargo:

- el equipo debe comprender el código,
- debe poder modificarlo,
- y debe documentar su uso en la bitácora.

---

# Estructura del Repositorio

```text
pacman-sdl2/
│
├── assets/
│   ├── sprites/
│   ├── maps/
│   │   └── nivel1.txt
│   └── fonts/
│
├── docs/
│   ├── diagramas/
│   ├── pseudocodigo/
│   └── bitacora.md
│
├── include/
│   ├── game.h
│   ├── map.h
│   ├── player.h
│   ├── ghost.h
│   ├── render.h
│   └── file_manager.h
│
├── src/
│   ├── main.c
│   ├── game.c
│   ├── map.c
│   ├── player.c
│   ├── ghost.c
│   ├── render.c
│   └── file_manager.c
│
├── scripts/
│   ├── build.sh
│   └── run.sh
│
├── Makefile
├── README.md
└── .gitignore
```

---

# Descripción de Carpetas

## assets/

Contiene recursos gráficos y archivos del juego.

### sprites/
Sprites utilizados para:

- jugador,
- fantasmas,
- paredes,
- pellets,
- fondos.

### maps/
Archivos de niveles.

### fonts/
Fuentes tipográficas opcionales.

---

## docs/

Documentación del proyecto.

### diagramas/
Diagramas de flujo.

### pseudocodigo/
Pseudocódigo del proyecto.

### bitacora.md
Bitácora de desarrollo.

---

## include/

Archivos de encabezado (`.h`).

---

## src/

Código fuente del proyecto.

---

## scripts/

Scripts auxiliares para compilar y ejecutar.

---

# Ejemplo de Archivo de Mapa

```text
####################
#........##........#
#.####.#.##.####.#.#
#..................#
####################
```

---

# Representación del Mapa

| Símbolo | Significado |
|---|---|
| # | pared |
| . | pellet |
| P | jugador |
| F | fantasma |
| espacio | camino libre |

---

# Fragmentos de Código Sugeridos

## Representación del mapa

```c
char mapa[FILAS][COLUMNAS];
```

---

## Estructura del jugador

```c
typedef struct {
    int x;
    int y;
} Player;
```

---

## Estructura del fantasma

```c
typedef struct {
    int x;
    int y;
    int direction;
} Ghost;
```

---

## Funciones sugeridas

```c
void cargarMapa();
void moverJugador();
void moverFantasma();
void detectarColisiones();
void renderizarMapa();
void guardarScore();
```

---

# Ejemplo de Reglas Simples para Fantasmas

Los fantasmas NO requieren inteligencia artificial avanzada.

Puede utilizarse lógica simple como:

```text
Si jugador_x < fantasma_x
    intentar izquierda

Si jugador_x > fantasma_x
    intentar derecha

Si jugador_y < fantasma_y
    intentar arriba

Si jugador_y > fantasma_y
    intentar abajo
```

Si existe una pared:

```text
seleccionar otra dirección válida
```

También puede combinarse:

- 70% persecución
- 30% movimiento aleatorio

---

# TODOs Sugeridos

Ejemplo de comentarios útiles:

```c
// TODO:
// Implementar colisiones con paredes
```

```c
// TODO:
// Implementar sistema de puntaje
```

```c
// TODO:
// Implementar movimiento de fantasmas
```

---

# Etapas Sugeridas

| Etapa | Objetivo |
|---|---|
| 1 | Crear ventana SDL2 |
| 2 | Cargar mapa |
| 3 | Movimiento del jugador |
| 4 | Colisiones |
| 5 | Fantasmas |
| 6 | Sistema de puntaje |
| 7 | Guardado de archivos |
| 8 | Pulido final |

---

# Bitácora

Cada equipo deberá documentar:

- avances,
- problemas,
- decisiones,
- pruebas,
- uso de IA,
- distribución de tareas.

Ejemplo:

```markdown
# Fecha
# Integrante
# Actividad realizada
# Problema encontrado
# Uso de IA
# Solución implementada
```

---

# Recomendaciones

- Modularizar desde el inicio
- Hacer commits frecuentes
- Probar constantemente
- Documentar funciones
- Mantener responsabilidades claras dentro del equipo

---

# Compilación

Ejemplo usando GCC:

```bash
gcc src/*.c -o pacman \
-Iinclude \
-lSDL2
```

---

# Ejecución

```bash
./pacman
```

---

# Créditos

Proyecto académico para la materia de Fundamentos de Programación.



# Sugerencias

# Código Base Sugerido

---

## player.h

```c
#ifndef PLAYER_H
#define PLAYER_H

typedef struct {

    int x;
    int y;

    int width;
    int height;

    int speed;

    int score;
    int lives;

} Player;

void initPlayer(Player *player);

void movePlayer(
    Player *player,
    int dx,
    int dy,
    char mapa[][28]
);

void renderPlayer(Player *player);

#endif
```

---

## player.c

```c
#include "player.h"

void initPlayer(Player *player)
{
    /*
        TODO:
        Inicializar jugador
    */
}

void movePlayer(
    Player *player,
    int dx,
    int dy,
    char mapa[][28]
)
{
    /*
        TODO:
        Movimiento del jugador
    */

    /*
        TODO:
        Colisiones con paredes
    */

    /*
        TODO:
        Consumir pellets
    */
}

void renderPlayer(Player *player)
{
    /*
        TODO:
        Dibujar jugador usando SDL2
    */
}
```

---

## ghost.h

```c
#ifndef GHOST_H
#define GHOST_H

typedef struct {

    int x;
    int y;

    int direction;

    int speed;

} Ghost;

void initGhost(
    Ghost *ghost,
    int x,
    int y
);

void moveGhost(
    Ghost *ghost,
    int playerX,
    int playerY,
    char mapa[][28]
);

void renderGhost(Ghost *ghost);

#endif
```

---

## ghost.c

```c
#include "ghost.h"

void initGhost(
    Ghost *ghost,
    int x,
    int y
)
{
    /*
        TODO:
        Inicializar fantasma
    */
}

void moveGhost(
    Ghost *ghost,
    int playerX,
    int playerY,
    char mapa[][28]
)
{
    /*
        TODO:
        Implementar reglas simples
        de persecución
    */

    /*
        TODO:
        Evitar paredes
    */

    /*
        TODO:
        Movimiento alternativo
    */
}

void renderGhost(Ghost *ghost)
{
    /*
        TODO:
        Dibujar fantasma usando SDL2
    */
}
```
