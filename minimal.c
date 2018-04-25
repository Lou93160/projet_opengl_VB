#include "minimal.h"
#include "main.c"

/******* FUNCTIONS *******/

/* Resize of the window */
/* But still working without it... */
void resizeViewport(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION); // MODELVIEW?
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);
}

/* Background X scrolling */
void moveBackground(Background *bg, int pStart){
    if(bg->position < - 1.98) bg->position = 1.99;
    else bg->position -= bg->speed;
}

/* Function which manages the ship move */
void moveShip(int move, Ship *s){
    /* Up */
    if (move == 1){
        if(s->pos + s->speed <= 0.9){
            s->pos += s->speed;
        }
    }
    /* Down */
    else if (move == -1){
        if(s->pos - s->speed >= -1){
            s->pos -= s->speed;
        }
    }
}

/* Same but for arrows
returns 1 if the arrow is moving 
0 calls the deleteArrow function and free the memory */
int moveArrow(Arrow *a){
    if(a->posX + a->speed >= 2){
        return 0;
    }
    a->posX += a->speed;
    return 1;
}

/*
Voir pour changer ses 3 fonctions trop répétitives : faire avec world ?
*/

void drawArrow(Arrow *a, float scaleW, float scaleH){
    glPushMatrix();
        glTranslatef(a->posX, a->posY, 0);
        draw(a->textureID,scaleW,scaleH);
    glPopMatrix();
}

void drawShip(Ship *s, float scaleW, float scaleH, int move){
    glPushMatrix();
        glTranslatef(-0.7, s->pos, 0);
        if(move != 0){      
            glRotatef(10*move, 1, 1, 1);
        }
        draw(s->textureID,scaleW,scaleH);
    glPopMatrix();
}

void drawBG(Background *bg, float scaleW, float scaleH){
    glPushMatrix();
        glTranslatef(bg->position, 0, 0);
        glScalef(scaleW, scaleH, 1);
        draw(bg->textureID, scaleW,scaleH);
    glPopMatrix();       
}

/* Function which call the other specifics to draw arrow, bg or ship 
Apply texture on a quads */
void draw(GLuint textureID, float scaleW, float scaleH){
        /* Transparency */
        //glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glShadeModel(GL_SMOOTH);
        /* Brackground drawing */
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
        glBindTexture(GL_TEXTURE_2D, 0);
        //glDisable(GL_BLEND);
}

/*
A REVOIR LA TRANSPARENCE
*/

/* Apply texture on image */
void createImg(GLuint textureID, SDL_Surface* img){
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //SDL_SetColorKey(img,SDL_SRCCOLORKEY,SDL_MapRGBA(img->format,0,255,0,0));
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        img->w,
        img->h,
        0,
        GL_BGR,
        GL_UNSIGNED_BYTE,
        img->pixels
    );
    /* Debind texture */
    glBindTexture(GL_TEXTURE_2D, 0);
    /* Free memory */
    SDL_FreeSurface(img);
}

/*
Pareil pour ses 3 fonctions
*/

/* Initializes the bakcground and loads its image */
void loadBG(Background *bg, const char *imgSrc, float speed, float pos){
    bg->position = pos;
    bg->speed = speed;
    bg->img = SDL_LoadBMP(imgSrc);
    if(bg->img == NULL){
        printf("Error at loading BG image\n");
        return;
    }
    glGenTextures(1, &bg->textureID);
    createImg(bg->textureID, bg->img);
}

/* Initializes the ship and loads its image */
void loadShip(Ship *s, const char *imgSrc){
    s->nb_life = 10;
    s->move = 0;
    s->pos = 0.0;
    s->speed = 0.03;
    s->lstA = NULL;
    s->img = SDL_LoadBMP(imgSrc);
    if(s->img == NULL){
        printf("Error at loading SHIP image\n");
        return;
    }
    glGenTextures(1, &s->textureID);
    createImg(s->textureID, s->img);
}

/* Function called by the pushArrow which allocs, load and return the list
Argu are:
the arrowlist in the ship structure;
the source image;
pInList indicates the position of the image to load in the list (+1 if next, 0 if the instant one);
position Y of the ship (arrow start point).
*/
ListeArrow loadArrow(ListeArrow list, const char *imgSrc, int pInList, float pos){
    /* Allocation */
    list = (Arrow*)malloc(sizeof(Arrow));
    if(list == NULL){
        printf("ERROR AT MALLOC ARROW\n");
    }
    /* If we want to load the next element of the list */
    if(pInList == 1){ 
        printf("pInList %d\n", pInList);  
        list = list->Asuiv;
    }
    /* Initialize its elements */
    list->posX = -0.71;
    list->posY = pos+0.05;
    list->speed = 0.2;
    list->Asuiv = NULL;
    if(pInList == 1) list->Aprec = list;
    else list->Aprec = NULL;
    /* Image loading */
    list->img = SDL_LoadBMP(imgSrc);
    if(list->img == NULL){
        printf("Error at loading ARROW image\n");
        return NULL;
    }
    glGenTextures(1, &list->textureID);
    createImg(list->textureID, list->img);

    return list;
}

/* Function called when space touch is down
and it allocates or pushes into the ship's list arrows */
void pushArrow(Ship *s){
    /* Verification of the listArrow : filled or not ? 
    if yes, we cross the list and place the instant arrow at the end */
    if (s->lstA != NULL){
        while(s->lstA->Asuiv != NULL){
            s->lstA = s->lstA->Asuiv;
        }
        /* Allocation of the new arrow & image loading in loadArrow*/
        s->lstA = loadArrow(s->lstA, "./img/elts/arrow.png", 1, s->pos);
    }
    /* Else we malloc and initialize the list */
    s->lstA = loadArrow(s->lstA, "./img/elts/arrow.png", 0, s->pos);
}

/* Free the memory taken by loadArrow */
void deleteArrow(Ship *s, ListeArrow elt){
    if (s->lstA == elt && elt->Asuiv == NULL){
        s->lstA = NULL;
        free(elt);
        return;
    }
    if (elt->Asuiv == NULL){
        elt->Aprec->Asuiv = NULL;
        free(elt);
        return;
    }
    elt->Asuiv->Aprec = elt->Aprec;
    elt->Aprec->Asuiv = elt->Asuiv;
    glDeleteTextures(1, &elt->textureID);
    free(elt);
}