#ifdef __APPLE__
#include <openGL/gl.h>
#include <openGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <SDL/SDL.h>
#include <stdlib.h>
#include <stdio.h>

/***** VARIABLES & STRUCTURES *****/

static unsigned int WINDOW_WIDTH = 939;
static unsigned int WINDOW_HEIGHT = 587;
static const unsigned int BIT_PER_PIXEL = 32;
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

/* Type of image */
#define BACKGROUND 'f'
#define SHIP 's'
#define ARROW 'a'

typedef struct ship{
    int nb_life;
    int move; // 1 si haut -1 si bas 0 si bouge pas
    float pos; // Une seule position Y
    SDL_Surface* img;
    GLuint textureID;
    float speed;
} Ship;

typedef struct background{
    float position; // bouge sur la position X
    SDL_Surface* img;
    GLuint textureID;
} Background;

typedef struct arrow{
    int dir; // Direction 
    float posX; // Position
    float posY;
    float speed;
    SDL_Surface* img;
    GLuint textureID;
} Arrow;

typedef struct worldgame{
    Arrow arrows;
    Background backgrounds;
    Ship ships;
    int level;
    float speed;
} WorldGame;


/****** PROTOTYPES *******/

void resizeViewport(int w, int h);
void moveShip(int i, Ship *s);
void drawShip(Ship *ship, float scaleW, float scaleH);
void drawBG(Background *bg, float scaleW, float scaleH);
void draw(GLuint textureID, float scaleW, float scaleH);
void createImg(GLuint textureID, SDL_Surface* img);
void loadBG(Background *bg, const char *imgSrc);
void loadShip(Ship *ship, const char *imgSrc);


/****** FONCTIONS *******/

/* Fonctions qui resize la fenêtre */
/* Marche même quand la fonction n'est pas appelée ! */
void resizeViewport(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION); //MODELVIEW
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);

}

/* 
TO DO : 
moveBackground();
*/

void moveShip(int i, Ship *s){
    if (i == 1){ //Vers le haut
        if(s->pos + s->speed < 1){
            s->pos += s->speed;
        }
    }
    else if (i == -1){ //Vers le bas
        if(s->pos - s->speed > -1){
            s->pos -= s->speed;
        }
    }
}

void moveArrow(Arrow *a){
    if(a->posX + a->speed < 1){
        a->posX += a->speed;
    }
}

void drawArrow(Arrow *a, float scaleW, float scaleH){
    glPushMatrix();
        glTranslatef(a->posX, a->posY, 0);
        draw(a->textureID,scaleW,scaleH);
    glPopMatrix();
}

void drawShip(Ship *s, float scaleW, float scaleH){
    glPushMatrix();
        glTranslatef(-0.7, s->pos, 0);
        draw(s->textureID,scaleW,scaleH);
    glPopMatrix();
}

void drawBG(Background *bg, float scaleW, float scaleH){
    draw(bg->textureID, scaleW,scaleH);
}

/* draw */
void draw(GLuint textureID, float scaleW, float scaleH){
        /* Transparency */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /* Brackground drawing */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
            glPushMatrix();
                glScalef(scaleW,scaleH,0);
                glBegin(GL_QUADS);
                glTexCoord2f(0,0);
                glVertex2f(-1,1);

                glTexCoord2f(1,0);
                glVertex2f(1,1);

                glTexCoord2f(1,1);
                glVertex2f(1,-1);

                glTexCoord2f(0,1);
                glVertex2f(-1,-1);
                glEnd();
            glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
}


/*
A REVOIR LA TRANSPARENCE
*/

/* Apply texture on image */
void createImg(GLuint textureID, SDL_Surface* img){
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    SDL_SetColorKey(img,SDL_SRCCOLORKEY,SDL_MapRGB(img->format,0,255,0));
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        img->w,
        img->h,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        img->pixels
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_FreeSurface(img);
}

void loadBG(Background *bg, const char *imgSrc){
    bg->position = 0;
    bg->img = SDL_LoadBMP(imgSrc);
    if(bg->img == NULL){
        printf("Error at loading BG image\n");
        return;
    }
    glGenTextures(1, &bg->textureID);
    createImg(bg->textureID, bg->img);
}

void loadShip(Ship *s, const char *imgSrc){
    s->nb_life = 10;
    s->move = 0;
    s->pos = 0.0;
    s->speed = 0.03;

    s->img = SDL_LoadBMP(imgSrc);
    if(s->img == NULL){
        printf("Error at loading SHIP image\n");
        return;
    }
    glGenTextures(1, &s->textureID);
    createImg(s->textureID, s->img);
}

void loadArrow(Arrow *a, const char *imgSrc){
    a->dir= 1;
    a->posX = -0.7;
    a->speed = 0.04;
    a->img = SDL_LoadBMP(imgSrc);
    if(a->img == NULL){
        printf("Error at loading ARROW image\n");
        return;
    }
    glGenTextures(1, &a->textureID);
    createImg(a->textureID, a->img);
}


/************** MAIN ***************/


int main(int argc, char** argv) {
    int move = 0; /* Movement mode by default */
    int attack = 0;
    int level = 0;
    WorldGame world;

    /* SDL initialization */
    if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
        return EXIT_FAILURE;
    }

    /* Window opening and OpenGL context creation */
    if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE)) {
        fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
        return EXIT_FAILURE;
    }
    /* Name of the window */
    SDL_WM_SetCaption("Egypt of the future", NULL);

    resizeViewport(WINDOW_WIDTH, WINDOW_HEIGHT);

    int loop = 1;
    glClearColor(0, 0, 0, 1.0);

    /* Background loading */
    Background bg1;
    loadBG(&bg1, "./img/fds/niv1.bmp");

    /* Ship loading */
    Ship ship;
   loadShip(&ship, "./img/elts_green/ship.bmp");

    world.ships = ship;
    world.backgrounds = bg1;
    world.level = level;

    /* Arrow loading */
    Arrow arrow;
    loadArrow(&arrow, "./img/elts_green/arrow.bmp");


    /*********** LOOP **********/

    while(loop) {

        Uint32 startTime = SDL_GetTicks();

        glClear(GL_COLOR_BUFFER_BIT);

        /* If move has changed */
        if(move == 1){
           moveShip(move, &ship);
        }
        if(move == -1){
            moveShip(move, &ship);
        }

        if(attack == 1){ 
            world.arrows = arrow;
            arrow.posY = 1.0;
            moveArrow(&arrow);  
            drawArrow(&arrow, 0.1, 0.3); 
        }

        drawBG(&bg1, 1, 1);
        drawShip(&ship, 0.11, 0.135);

        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            switch(e.type) {

                case SDL_QUIT:
                    loop = 0;
                    break;

                /* A REVOIR PRBL DE RESIZE (CORRECTION DU TROP GRAND) */
                 case SDL_VIDEORESIZE:
                    if(e.resize.w > WINDOW_WIDTH || WINDOW_HEIGHT < e.resize.h){
                        resizeViewport(WINDOW_WIDTH,WINDOW_HEIGHT);
                        glClear(GL_COLOR_BUFFER_BIT);
                        SDL_GL_SwapBuffers();
                    }
                    break;

                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                    printf("Keydown : %c\n", e.key.keysym.sym);
                        case SDLK_q:
                            loop = 0;
                            break;
                        case SDLK_ESCAPE:
                            /* ABANDON */
                            loop = 0;
                            break;
                        case SDLK_UP:
                            /* GO UP */
                            printf("UP\n");
                            move = 1;
                            break;
                        case SDLK_DOWN:
                            /* GO DOWN */
                            printf("DOWN\n");
                            move = -1;
                            break;
                        case SDLK_SPACE:
                            /* SHOOT */
                            printf("SHOOT\n");
                            attack = 1;
                            break;
                        default:
                            attack = 0;
                            move = 0;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    if(e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_UP){
                        move = 0;
                    }
                    if(e.key.keysym.sym == SDLK_SPACE){
                        attack = 0;
                    }
                    break;

                default:
                    break;
                }
            }

        SDL_GL_SwapBuffers();
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    // SDL_FreeSurface(ship);
    // glDeleteTextures(1, &texture3);

    SDL_Quit();

    return EXIT_SUCCESS;
}
