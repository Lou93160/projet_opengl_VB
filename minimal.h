/* For apple OS */
#ifdef __APPLE__
#include <openGL/gl.h>
#include <openGL/glu.h>
/* For Linus OS */
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
/* For all */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/***** VARIABLES & STRUCTURES *****/

static unsigned int WINDOW_WIDTH = 938;
static unsigned int WINDOW_HEIGHT = 586;
static const unsigned int BIT_PER_PIXEL = 32;
static const Uint32 FRAMERATE_MILLISECONDS = 1000 /60;


/* Type of image */
#define BACKGROUND 'f'
#define SHIP 's'
#define ARROW 'a'


typedef struct arrow{
    SDL_Surface* img;
    GLuint textureID;
    float posX; // Position
    float posY;
    float speed;
    struct arrow *Asuiv, *Aprec;
} Arrow, *ListeArrow;

/* Ship has infinit arrows number */
typedef struct ship{
    GLuint textureID;
    int nb_life;
    int move; // 1 up -1 down 0 no move
    float pos; // Y axis
    float speed;
    ListeArrow lstA;
} Ship;

typedef struct background{
    GLuint textureID;
    float position; // move on X axis
    float speed; // scroll speed
} Background;

typedef struct worldgame{
    int width, height;
    Ship ships;
    int level;
} WorldGame;

typedef struct elements{
    float posX, posY;
    float w, h;// width & height
    int nb_life;
    GLuint textureID;
    struct elements *next;
} Element, *ListeElement;


/**** Global variable World *****/

WorldGame World;


/******* PROTOTYPES *******/


void resizeViewport(int w, int h);
void moveBackground(Background *bg, int pStart);
void moveShip(int i, Ship *s);
int moveArrow(Arrow *a);
void drawShip(Ship *ship, float scaleW, float scaleH, int mode);
void drawBG(Background *bg, float scaleW, float scaleH);
void drawArrow(Arrow *a, float scaleW, float scaleH);
void draw(GLuint textureID, float scaleW, float scaleH);
void createImgAlpha(GLuint *textureID, SDL_Surface* img);
void createImg(GLuint *textureID, SDL_Surface* img);
Background loadBG(float speed, float pos);
Ship loadShip();
ListeArrow loadArrow(ListeArrow list,GLuint textureID, int pInList, float pos);
void pushArrow(Ship *s, GLuint textureID);
void deleteArrow(Ship *s, ListeArrow elt);
Element* initElement(GLuint textureID, float x, float y, int life);
void addElementToList(ListeElement *lst, GLuint textureID, float x, float y, int life);
void pushElements(ListeElement *bonus, ListeElement *mobs, ListeElement *obstacles);
void drawElement(Element elt, GLuint textureID,float incremente);
void drawListElements(ListeElement lst,float incremente);
void displayList(ListeElement lst);
void drawsquare();