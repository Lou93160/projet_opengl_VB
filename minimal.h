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
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;


/* Type of image */
#define BACKGROUND 'f'
#define SHIP 's'
#define ARROW 'a'

/* Bouding Box size */
#define BOX 4

typedef struct arrow{
    SDL_Surface* img;
    GLuint textureID;
    float bbox[BOX];
    float posX; // Position
    float posY;
    float speed;
    struct arrow *Asuiv, *Aprec;
} Arrow, *ListeArrow;

/* Ship has infinit arrows number */
typedef struct ship{
    GLuint textureID;
    float bbox[BOX];
    int nb_life;
    int score;
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

typedef struct elements{
    float posX, posY;
    float w, h;// width & height
    int nb_life;
    float bbox[BOX];
    GLuint textureID;
    int collision;
    struct elements *next;
} Element, *ListeElement;

typedef struct worldgame{
    int width, height;
    ListeElement mobs;
    ListeElement bonus;
    ListeElement keys;
    ListeElement obstacles;
    Ship ships;
    int level;
} WorldGame;




/**** Global variable World *****/

WorldGame World;


/******* PROTOTYPES *******/


/* Resize of the window */
/* But still working without it... */
void resizeViewport(int w, int h);
/* Background X scrolling */
void moveBackground(Background *bg, int pStart);
/* Function which manages the ship move */
void moveShip(int i, WorldGame *w, Ship *s);
/* Same but for arrows
returns 1 if the arrow is moving 
0 calls the deleteArrow function and free the memory */
int moveArrow(Arrow *a);
void drawShip(Ship *ship, float scaleW, float scaleH, int mode);
void drawBG(Background *bg, float scaleW, float scaleH);
void drawArrow(Arrow *a, float scaleW, float scaleH);
/* Function which call the other specifics to draw arrow, bg or ship */
void draw(GLuint textureID, float scaleW, float scaleH);
/* Apply texture on image */
void createImgAlpha(GLuint *textureID, SDL_Surface* img);
void createImg(GLuint *textureID, SDL_Surface* img);
/* Initializes the bakcground and loads its image */
Background loadBG(float speed, float pos);
/* Initializes the ship and loads its image */
Ship loadShip();

/* Function called by the pushArrow which allocs, load and return the list
Argu are:
the arrowlist in the ship structure;
the source image;
pInList indicates the position of the image to load in the list (+1 if next, 0 if the instant one);
position Y of the ship (arrow start point).
*/
ListeArrow loadArrow(ListeArrow list,GLuint textureID, int pInList, float pos);

/* Called when space touch is down
and it allocates or pushes into the ship's list arrows */
void pushArrow(Ship *s, GLuint textureID);
/* Free the memory taken by loadArrow */
void deleteArrow(Ship *s, ListeArrow elt);
Element* initElement(GLuint textureID, float x, float y, int life);
void addElementToList(ListeElement *lst, GLuint textureID, float x, float y, int life);
void pushElements(ListeElement *bonus, ListeElement *mobs, ListeElement *obstacles, ListeElement *keys, int level);
/* Draws element passed in argu with increment value passed in agru for its x position */
void drawElement(Element *elt, GLuint textureID,float incremente);
/* Reads each elements passed in argu + sends them to the drawElements function */
void drawListElements(ListeElement lst,float incremente);
void collisionTest(WorldGame *w);
/* 2 above for debugging */
void displayList(ListeElement lst);
void drawsquare();
