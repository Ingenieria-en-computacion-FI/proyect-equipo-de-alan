#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#undef main

#define FILAS 15
#define COLUMNAS 25
#define TAM 32

#define MAX_FANTASMAS 10

#define VELOCIDAD_MARIO 70.0
#define VELOCIDAD_FANTASMA 45.0
#define VELOCIDAD_FANTASMA_ASUSTADO 30.0

#define DURACION_PODER 8000
#define TIEMPO_REAPARICION 1200

/* Tamaños visuales de sprites */
#define TAM_CABALLERO 30
#define TAM_ORCO 28
#define TAM_PRINCESA 28
#define TAM_RAMO 23
#define TAM_POWER 24

/* Radios de círculos */
#define RADIO_CABALLERO 16
#define RADIO_ORCO 16
#define RADIO_PRINCESA 16
#define RADIO_RAMO 13
#define RADIO_POWER 14

char mapa[FILAS][COLUMNAS];

SDL_Texture *texturaCaballero = NULL;
SDL_Texture *texturaOrco = NULL;
SDL_Texture *texturaPrincesa = NULL;
SDL_Texture *texturaRamo = NULL;
SDL_Texture *texturaPower = NULL;

typedef struct{
    int fila;
    int col;
    int inicioFila;
    int inicioCol;

    int targetFila;
    int targetCol;

    double x;
    double y;

    int dirFila;
    int dirCol;

    int moviendo;
    int vivo;
    int esperaReaparicion;
} Fantasma;

Fantasma fantasmas[MAX_FANTASMAS];
int totalFantasmas = 0;

int marioFila = 0;
int marioCol = 0;
int marioInicioFila = 0;
int marioInicioCol = 0;

int marioTargetFila = 0;
int marioTargetCol = 0;

double marioX = 0;
double marioY = 0;

int marioMoviendo = 0;

int dirFila = 0;
int dirCol = 0;

int dirDeseadaFila = 0;
int dirDeseadaCol = 0;

int score = 0;
int vidas = 3;
int puntosRestantes = 0;

int victoria = 0;
int gameOver = 0;

int poderActivo = 0;
int poderTiempo = 0;

SDL_Texture *cargarTexturaPNG(SDL_Renderer *renderer, char nombreArchivo[]){
    SDL_Surface *superficie;
    SDL_Texture *textura;

    superficie = IMG_Load(nombreArchivo);

    if(superficie == NULL){
        printf("No se pudo cargar %s\n", nombreArchivo);
        printf("Error SDL_image: %s\n", IMG_GetError());
        return NULL;
    }

    textura = SDL_CreateTextureFromSurface(renderer, superficie);
    SDL_FreeSurface(superficie);

    if(textura == NULL){
        printf("No se pudo crear textura de %s\n", nombreArchivo);
        printf("Error SDL: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetTextureBlendMode(textura, SDL_BLENDMODE_BLEND);

    return textura;
}

void dibujarCirculo(SDL_Renderer *renderer, int centroX, int centroY, int radio){
    for(int y = -radio; y <= radio; y++){
        for(int x = -radio; x <= radio; x++){
            if(x*x + y*y <= radio*radio){
                SDL_RenderDrawPoint(renderer, centroX + x, centroY + y);
            }
        }
    }
}

void dibujarTexturaCentrada(SDL_Renderer *renderer, SDL_Texture *textura, int centroX, int centroY, int ancho, int alto){
    SDL_Rect destino;

    destino.w = ancho;
    destino.h = alto;
    destino.x = centroX - ancho / 2;
    destino.y = centroY - alto / 2;

    SDL_RenderCopy(renderer, textura, NULL, &destino);
}

void dibujarSpriteCircular(SDL_Renderer *renderer, SDL_Texture *textura, int centroX, int centroY, int ancho, int alto, int radio, int r, int g, int b){
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    dibujarCirculo(renderer, centroX, centroY, radio);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    dibujarCirculo(renderer, centroX, centroY, radio - 4);

    if(textura != NULL){
        dibujarTexturaCentrada(renderer, textura, centroX, centroY, ancho, alto);
    }
}

void cargarMapa(){
    FILE *archivo;
    char linea[100];

    archivo = fopen("mapa.txt", "r");

    if(archivo == NULL){
        printf("No se pudo abrir mapa.txt\n");
        return;
    }

    puntosRestantes = 0;
    totalFantasmas = 0;

    for(int i = 0; i < FILAS; i++){

        if(fgets(linea, 100, archivo) == NULL){
            break;
        }

        for(int j = 0; j < COLUMNAS; j++){

            if(linea[j] == '\n' || linea[j] == '\0' || linea[j] == '\r'){
                mapa[i][j] = ' ';
            }
            else{
                mapa[i][j] = linea[j];
            }

            if(mapa[i][j] == 'M'){
                marioFila = i;
                marioCol = j;
                marioInicioFila = i;
                marioInicioCol = j;

                marioTargetFila = i;
                marioTargetCol = j;

                marioX = j * TAM;
                marioY = i * TAM;

                mapa[i][j] = ' ';
            }

            if(mapa[i][j] == 'B'){
                if(totalFantasmas < MAX_FANTASMAS){
                    fantasmas[totalFantasmas].fila = i;
                    fantasmas[totalFantasmas].col = j;
                    fantasmas[totalFantasmas].inicioFila = i;
                    fantasmas[totalFantasmas].inicioCol = j;

                    fantasmas[totalFantasmas].targetFila = i;
                    fantasmas[totalFantasmas].targetCol = j;

                    fantasmas[totalFantasmas].x = j * TAM;
                    fantasmas[totalFantasmas].y = i * TAM;

                    fantasmas[totalFantasmas].dirFila = 0;
                    fantasmas[totalFantasmas].dirCol = 0;

                    fantasmas[totalFantasmas].moviendo = 0;
                    fantasmas[totalFantasmas].vivo = 1;
                    fantasmas[totalFantasmas].esperaReaparicion = 0;

                    totalFantasmas++;
                }

                mapa[i][j] = ' ';
            }

            if(mapa[i][j] == '.' || mapa[i][j] == 'C' || mapa[i][j] == 'R'){
                puntosRestantes++;
            }
        }
    }

    fclose(archivo);
}

void dibujarPared(SDL_Renderer *renderer, int fila, int col, int grosor){
    int centroX = col * TAM + TAM / 2;
    int centroY = fila * TAM + TAM / 2;

    int arriba = 0;
    int abajo = 0;
    int izquierda = 0;
    int derecha = 0;

    if(fila > 0 && mapa[fila - 1][col] == '#'){
        arriba = 1;
    }

    if(fila < FILAS - 1 && mapa[fila + 1][col] == '#'){
        abajo = 1;
    }

    if(col > 0 && mapa[fila][col - 1] == '#'){
        izquierda = 1;
    }

    if(col < COLUMNAS - 1 && mapa[fila][col + 1] == '#'){
        derecha = 1;
    }

    SDL_Rect parte;

    if(arriba == 1){
        parte.x = centroX - grosor / 2;
        parte.y = centroY - TAM / 2;
        parte.w = grosor;
        parte.h = TAM / 2;
        SDL_RenderFillRect(renderer, &parte);
    }

    if(abajo == 1){
        parte.x = centroX - grosor / 2;
        parte.y = centroY;
        parte.w = grosor;
        parte.h = TAM / 2;
        SDL_RenderFillRect(renderer, &parte);
    }

    if(izquierda == 1){
        parte.x = centroX - TAM / 2;
        parte.y = centroY - grosor / 2;
        parte.w = TAM / 2;
        parte.h = grosor;
        SDL_RenderFillRect(renderer, &parte);
    }

    if(derecha == 1){
        parte.x = centroX;
        parte.y = centroY - grosor / 2;
        parte.w = TAM / 2;
        parte.h = grosor;
        SDL_RenderFillRect(renderer, &parte);
    }

    dibujarCirculo(renderer, centroX, centroY, grosor / 2);
}

void dibujarFantasma(SDL_Renderer *renderer, Fantasma f){
    int centroX = (int)f.x + TAM / 2;
    int centroY = (int)f.y + TAM / 2;

    if(texturaOrco != NULL){
        if(poderActivo == 1){
            SDL_SetTextureColorMod(texturaOrco, 80, 120, 255);
            dibujarSpriteCircular(renderer, texturaOrco, centroX, centroY, TAM_ORCO, TAM_ORCO, RADIO_ORCO, 40, 80, 255);
            SDL_SetTextureColorMod(texturaOrco, 255, 255, 255);
        }
        else{
            dibujarSpriteCircular(renderer, texturaOrco, centroX, centroY, TAM_ORCO, TAM_ORCO, RADIO_ORCO, 90, 0, 0);
        }
    }
    else{
        SDL_SetRenderDrawColor(renderer, 0, 180, 60, 255);
        dibujarCirculo(renderer, centroX, centroY, 13);
    }
}

void dibujarMario(SDL_Renderer *renderer){
    int centroX = (int)marioX + TAM / 2;
    int centroY = (int)marioY + TAM / 2;

    if(texturaCaballero != NULL){
        dibujarSpriteCircular(renderer, texturaCaballero, centroX, centroY, TAM_CABALLERO, TAM_CABALLERO, RADIO_CABALLERO, 120, 0, 0);
    }
    else{
        SDL_SetRenderDrawColor(renderer, 255, 40, 40, 255);
        dibujarCirculo(renderer, centroX, centroY, 13);
    }
}

void dibujarMapa(SDL_Renderer *renderer){
    SDL_Rect cuadro;

    cuadro.w = TAM;
    cuadro.h = TAM;

    for(int i = 0; i < FILAS; i++){
        for(int j = 0; j < COLUMNAS; j++){

            cuadro.x = j * TAM;
            cuadro.y = i * TAM;

            if(mapa[i][j] == '#'){
                SDL_SetRenderDrawColor(renderer, 0, 20, 130, 255);
                dibujarPared(renderer, i, j, 20);

                SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);
                dibujarPared(renderer, i, j, 10);
            }

            else if(mapa[i][j] == '.'){
                SDL_SetRenderDrawColor(renderer, 255, 220, 80, 255);
                dibujarCirculo(renderer, cuadro.x + TAM / 2, cuadro.y + TAM / 2, 4);
            }

            else if(mapa[i][j] == 'R'){
                if(texturaPower != NULL){
                    dibujarSpriteCircular(renderer, texturaPower, cuadro.x + TAM / 2, cuadro.y + TAM / 2, TAM_POWER, TAM_POWER, RADIO_POWER, 150, 90, 0);
                }
                else{
                    SDL_SetRenderDrawColor(renderer, 255, 80, 210, 255);
                    dibujarCirculo(renderer, cuadro.x + TAM / 2, cuadro.y + TAM / 2, 11);
                }
            }

            else if(mapa[i][j] == 'C'){
                if(texturaRamo != NULL){
                    dibujarSpriteCircular(renderer, texturaRamo, cuadro.x + TAM / 2, cuadro.y + TAM / 2, TAM_RAMO, TAM_RAMO, RADIO_RAMO, 20, 120, 40);
                }
                else{
                    SDL_SetRenderDrawColor(renderer, 120, 65, 25, 255);
                    SDL_Rect chocolate = {cuadro.x + 7, cuadro.y + 10, 18, 12};
                    SDL_RenderFillRect(renderer, &chocolate);
                }
            }

            else if(mapa[i][j] == 'P'){
                if(texturaPrincesa != NULL){
                    dibujarSpriteCircular(renderer, texturaPrincesa, cuadro.x + TAM / 2, cuadro.y + TAM / 2, TAM_PRINCESA, TAM_PRINCESA, RADIO_PRINCESA, 220, 70, 170);
                }
                else{
                    SDL_SetRenderDrawColor(renderer, 255, 120, 210, 255);
                    dibujarCirculo(renderer, cuadro.x + TAM / 2, cuadro.y + TAM / 2, 13);
                }
            }
        }
    }

    for(int i = 0; i < totalFantasmas; i++){
        if(fantasmas[i].vivo == 1){
            dibujarFantasma(renderer, fantasmas[i]);
        }
    }

    dibujarMario(renderer);
}

int puedeMoverMario(int fila, int col){
    if(fila < 0 || fila >= FILAS || col < 0 || col >= COLUMNAS){
        return 0;
    }

    if(mapa[fila][col] == '#'){
        return 0;
    }

    if(mapa[fila][col] == 'P' && puntosRestantes > 0){
        return 0;
    }

    return 1;
}

int puedeMoverFantasma(int fila, int col){
    if(fila < 0 || fila >= FILAS || col < 0 || col >= COLUMNAS){
        return 0;
    }

    if(mapa[fila][col] == '#'){
        return 0;
    }

    if(mapa[fila][col] == 'P'){
        return 0;
    }

    return 1;
}

void reiniciarMario(){
    marioFila = marioInicioFila;
    marioCol = marioInicioCol;

    marioTargetFila = marioInicioFila;
    marioTargetCol = marioInicioCol;

    marioX = marioInicioCol * TAM;
    marioY = marioInicioFila * TAM;

    marioMoviendo = 0;

    dirFila = 0;
    dirCol = 0;

    dirDeseadaFila = 0;
    dirDeseadaCol = 0;
}

void reiniciarFantasmas(){
    for(int i = 0; i < totalFantasmas; i++){
        fantasmas[i].fila = fantasmas[i].inicioFila;
        fantasmas[i].col = fantasmas[i].inicioCol;

        fantasmas[i].targetFila = fantasmas[i].inicioFila;
        fantasmas[i].targetCol = fantasmas[i].inicioCol;

        fantasmas[i].x = fantasmas[i].inicioCol * TAM;
        fantasmas[i].y = fantasmas[i].inicioFila * TAM;

        fantasmas[i].dirFila = 0;
        fantasmas[i].dirCol = 0;

        fantasmas[i].moviendo = 0;
        fantasmas[i].vivo = 1;
        fantasmas[i].esperaReaparicion = 0;
    }
}

void perderVida(){
    vidas--;

    if(vidas <= 0){
        gameOver = 1;
    }
    else{
        reiniciarMario();
        reiniciarFantasmas();
    }
}

void comerFantasma(int indice){
    score = score + 200;

    fantasmas[indice].vivo = 0;
    fantasmas[indice].moviendo = 0;
    fantasmas[indice].esperaReaparicion = TIEMPO_REAPARICION;
}

void procesarCeldaMario(){
    char celda = mapa[marioFila][marioCol];

    if(celda == 'P'){
        if(puntosRestantes == 0){
            victoria = 1;
        }

        return;
    }

    if(celda == '.'){
        score = score + 10;
        puntosRestantes--;
        mapa[marioFila][marioCol] = ' ';
    }

    if(celda == 'C'){
        score = score + 50;
        puntosRestantes--;
        mapa[marioFila][marioCol] = ' ';
    }

    if(celda == 'R'){
        score = score + 100;
        puntosRestantes--;
        poderActivo = 1;
        poderTiempo = DURACION_PODER;
        mapa[marioFila][marioCol] = ' ';
    }
}

void iniciarMovimientoMario(){
    int nuevaFila;
    int nuevaCol;

    nuevaFila = marioFila + dirDeseadaFila;
    nuevaCol = marioCol + dirDeseadaCol;

    if(puedeMoverMario(nuevaFila, nuevaCol) == 1){
        dirFila = dirDeseadaFila;
        dirCol = dirDeseadaCol;
    }

    if(dirFila == 0 && dirCol == 0){
        return;
    }

    nuevaFila = marioFila + dirFila;
    nuevaCol = marioCol + dirCol;

    if(puedeMoverMario(nuevaFila, nuevaCol) == 1){
        marioTargetFila = nuevaFila;
        marioTargetCol = nuevaCol;
        marioMoviendo = 1;
    }
    else{
        dirFila = 0;
        dirCol = 0;
        marioMoviendo = 0;
    }
}

void moverHaciaObjetivo(double *x, double *y, double objetivoX, double objetivoY, double avance){
    if(*x < objetivoX){
        *x = *x + avance;

        if(*x > objetivoX){
            *x = objetivoX;
        }
    }

    if(*x > objetivoX){
        *x = *x - avance;

        if(*x < objetivoX){
            *x = objetivoX;
        }
    }

    if(*y < objetivoY){
        *y = *y + avance;

        if(*y > objetivoY){
            *y = objetivoY;
        }
    }

    if(*y > objetivoY){
        *y = *y - avance;

        if(*y < objetivoY){
            *y = objetivoY;
        }
    }
}

void actualizarMario(double delta){
    double objetivoX;
    double objetivoY;
    double avance;

    if(marioMoviendo == 0){
        iniciarMovimientoMario();
    }

    if(marioMoviendo == 1){
        objetivoX = marioTargetCol * TAM;
        objetivoY = marioTargetFila * TAM;

        avance = VELOCIDAD_MARIO * delta;

        moverHaciaObjetivo(&marioX, &marioY, objetivoX, objetivoY, avance);

        if(marioX == objetivoX && marioY == objetivoY){
            marioFila = marioTargetFila;
            marioCol = marioTargetCol;

            marioMoviendo = 0;

            procesarCeldaMario();
        }
    }
}

int distanciaManhattan(int fila1, int col1, int fila2, int col2){
    int df = fila1 - fila2;
    int dc = col1 - col2;

    if(df < 0){
        df = -df;
    }

    if(dc < 0){
        dc = -dc;
    }

    return df + dc;
}

void elegirDireccionFantasma(int indice, int *dirF, int *dirC){
    int movimientos[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    int opciones[4];
    int totalOpciones = 0;

    for(int i = 0; i < 4; i++){
        int nuevaFila = fantasmas[indice].fila + movimientos[i][0];
        int nuevaCol = fantasmas[indice].col + movimientos[i][1];

        if(puedeMoverFantasma(nuevaFila, nuevaCol) == 1){
            opciones[totalOpciones] = i;
            totalOpciones++;
        }
    }

    if(totalOpciones == 0){
        *dirF = 0;
        *dirC = 0;
        return;
    }

    if(poderActivo == 1){
        int elegido = opciones[rand() % totalOpciones];

        *dirF = movimientos[elegido][0];
        *dirC = movimientos[elegido][1];
        return;
    }

    if(rand() % 100 < 70){
        int mejorMovimiento = opciones[0];
        int mejorDistancia = 9999;

        for(int i = 0; i < totalOpciones; i++){
            int mov = opciones[i];

            int nuevaFila = fantasmas[indice].fila + movimientos[mov][0];
            int nuevaCol = fantasmas[indice].col + movimientos[mov][1];

            int distancia = distanciaManhattan(nuevaFila, nuevaCol, marioFila, marioCol);

            if(distancia < mejorDistancia){
                mejorDistancia = distancia;
                mejorMovimiento = mov;
            }
        }

        *dirF = movimientos[mejorMovimiento][0];
        *dirC = movimientos[mejorMovimiento][1];
    }
    else{
        int elegido = opciones[rand() % totalOpciones];

        *dirF = movimientos[elegido][0];
        *dirC = movimientos[elegido][1];
    }
}

void iniciarMovimientoFantasma(int indice){
    int nuevaDirFila = 0;
    int nuevaDirCol = 0;

    elegirDireccionFantasma(indice, &nuevaDirFila, &nuevaDirCol);

    int nuevaFila = fantasmas[indice].fila + nuevaDirFila;
    int nuevaCol = fantasmas[indice].col + nuevaDirCol;

    if(puedeMoverFantasma(nuevaFila, nuevaCol) == 1){
        fantasmas[indice].dirFila = nuevaDirFila;
        fantasmas[indice].dirCol = nuevaDirCol;

        fantasmas[indice].targetFila = nuevaFila;
        fantasmas[indice].targetCol = nuevaCol;

        fantasmas[indice].moviendo = 1;
    }
}

void actualizarFantasmas(double delta, int deltaMs){
    double velocidad;
    double avance;
    double objetivoX;
    double objetivoY;

    for(int i = 0; i < totalFantasmas; i++){

        if(fantasmas[i].vivo == 0){
            fantasmas[i].esperaReaparicion = fantasmas[i].esperaReaparicion - deltaMs;

            if(fantasmas[i].esperaReaparicion <= 0){
                fantasmas[i].fila = fantasmas[i].inicioFila;
                fantasmas[i].col = fantasmas[i].inicioCol;

                fantasmas[i].targetFila = fantasmas[i].inicioFila;
                fantasmas[i].targetCol = fantasmas[i].inicioCol;

                fantasmas[i].x = fantasmas[i].inicioCol * TAM;
                fantasmas[i].y = fantasmas[i].inicioFila * TAM;

                fantasmas[i].dirFila = 0;
                fantasmas[i].dirCol = 0;

                fantasmas[i].moviendo = 0;
                fantasmas[i].vivo = 1;
            }

            continue;
        }

        if(fantasmas[i].moviendo == 0){
            iniciarMovimientoFantasma(i);
        }

        if(fantasmas[i].moviendo == 1){

            if(poderActivo == 1){
                velocidad = VELOCIDAD_FANTASMA_ASUSTADO;
            }
            else{
                velocidad = VELOCIDAD_FANTASMA;
            }

            avance = velocidad * delta;

            objetivoX = fantasmas[i].targetCol * TAM;
            objetivoY = fantasmas[i].targetFila * TAM;

            moverHaciaObjetivo(&fantasmas[i].x, &fantasmas[i].y, objetivoX, objetivoY, avance);

            if(fantasmas[i].x == objetivoX && fantasmas[i].y == objetivoY){
                fantasmas[i].fila = fantasmas[i].targetFila;
                fantasmas[i].col = fantasmas[i].targetCol;

                fantasmas[i].moviendo = 0;
            }
        }
    }
}

void revisarColisionesConFantasmas(){
    double marioCentroX = marioX + TAM / 2;
    double marioCentroY = marioY + TAM / 2;

    for(int i = 0; i < totalFantasmas; i++){

        if(fantasmas[i].vivo == 1){
            double fantasmaCentroX = fantasmas[i].x + TAM / 2;
            double fantasmaCentroY = fantasmas[i].y + TAM / 2;

            double dx = marioCentroX - fantasmaCentroX;
            double dy = marioCentroY - fantasmaCentroY;

            if(dx * dx + dy * dy < 300){
                if(poderActivo == 1){
                    comerFantasma(i);
                }
                else{
                    perderVida();
                    return;
                }
            }
        }
    }
}

void actualizarPoder(int deltaMs){
    if(poderActivo == 1){
        poderTiempo = poderTiempo - deltaMs;

        if(poderTiempo <= 0){
            poderActivo = 0;
            poderTiempo = 0;
        }
    }
}

void guardarPuntaje(){
    FILE *archivo;

    archivo = fopen("puntajes.txt", "a");

    if(archivo == NULL){
        printf("No se pudo guardar el puntaje.\n");
        return;
    }

    fprintf(archivo, "Puntaje: %d | Vidas restantes: %d\n", score, vidas);

    fclose(archivo);
}

void actualizarTitulo(SDL_Window *ventana){
    char titulo[150];

    if(puntosRestantes == 0){
        sprintf(titulo, "Castle Maze | Puntos: %d | Vidas: %d | Ve con la princesa", score, vidas);
    }
    else{
        sprintf(titulo, "Castle Maze | Puntos: %d | Vidas: %d | Restantes: %d", score, vidas, puntosRestantes);
    }

    SDL_SetWindowTitle(ventana, titulo);
}

void destruirTexturas(){
    if(texturaCaballero != NULL){
        SDL_DestroyTexture(texturaCaballero);
    }

    if(texturaOrco != NULL){
        SDL_DestroyTexture(texturaOrco);
    }

    if(texturaPrincesa != NULL){
        SDL_DestroyTexture(texturaPrincesa);
    }

    if(texturaRamo != NULL){
        SDL_DestroyTexture(texturaRamo);
    }

    if(texturaPower != NULL){
        SDL_DestroyTexture(texturaPower);
    }
}

int main(){

    SDL_Window *ventana;
    SDL_Renderer *renderer;
    SDL_Event evento;

    int salir = 0;

    Uint32 tiempoActual;
    Uint32 tiempoAnterior;
    int deltaMs;
    double delta;

    srand(time(NULL));

    cargarMapa();

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Error al iniciar SDL: %s\n", SDL_GetError());
        return 1;
    }

    if((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0){
        printf("Error al iniciar SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    ventana = SDL_CreateWindow(
        "Castle Maze",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        COLUMNAS * TAM,
        FILAS * TAM,
        SDL_WINDOW_SHOWN
    );

    if(ventana == NULL){
        printf("Error al crear ventana: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);

    if(renderer == NULL){
        printf("Error al crear renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    texturaCaballero = cargarTexturaPNG(renderer, "caballero.png");
    texturaOrco = cargarTexturaPNG(renderer, "orco.png");
    texturaPrincesa = cargarTexturaPNG(renderer, "princesa.png");
    texturaRamo = cargarTexturaPNG(renderer, "ramo.png");
    texturaPower = cargarTexturaPNG(renderer, "power.png");

    tiempoAnterior = SDL_GetTicks();

    while(salir == 0 && victoria == 0 && gameOver == 0){

        tiempoActual = SDL_GetTicks();
        deltaMs = tiempoActual - tiempoAnterior;
        tiempoAnterior = tiempoActual;

        delta = deltaMs / 1000.0;

        while(SDL_PollEvent(&evento)){

            if(evento.type == SDL_QUIT){
                salir = 1;
            }

            if(evento.type == SDL_KEYDOWN){

                if(evento.key.keysym.sym == SDLK_ESCAPE){
                    salir = 1;
                }

                if(evento.key.keysym.sym == SDLK_w || evento.key.keysym.sym == SDLK_UP){
                    dirDeseadaFila = -1;
                    dirDeseadaCol = 0;
                }

                if(evento.key.keysym.sym == SDLK_s || evento.key.keysym.sym == SDLK_DOWN){
                    dirDeseadaFila = 1;
                    dirDeseadaCol = 0;
                }

                if(evento.key.keysym.sym == SDLK_a || evento.key.keysym.sym == SDLK_LEFT){
                    dirDeseadaFila = 0;
                    dirDeseadaCol = -1;
                }

                if(evento.key.keysym.sym == SDLK_d || evento.key.keysym.sym == SDLK_RIGHT){
                    dirDeseadaFila = 0;
                    dirDeseadaCol = 1;
                }
            }
        }

        actualizarMario(delta);
        actualizarFantasmas(delta, deltaMs);
        revisarColisionesConFantasmas();
        actualizarPoder(deltaMs);
        actualizarTitulo(ventana);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        dibujarMapa(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    if(victoria == 1){
        guardarPuntaje();
        printf("VICTORIA. Llegaste con la princesa. Puntaje final: %d\n", score);
        SDL_Delay(2500);
    }

    if(gameOver == 1){
        guardarPuntaje();
        printf("GAME OVER. Puntaje final: %d\n", score);
        SDL_Delay(2500);
    }

    destruirTexturas();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);

    IMG_Quit();
    SDL_Quit();

    return 0;
}