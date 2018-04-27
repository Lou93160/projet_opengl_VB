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
    glMatrixMode(GL_MODELVIEW); // PROJECTION?
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

/* Function which call the other specifics to draw arrow, bg or ship */
void draw(GLuint textureID, float scaleW, float scaleH){
        /* Transparency */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        /* Brackground drawing */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
            glPushMatrix();
                glScalef(scaleW,scaleH,1.0); /* z à 1 */
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
}

/*
A REVOIR LA TRANSPARENCE
*/

/* Apply texture on image */
void createImgAlpha(GLuint *textureID, SDL_Surface* img){	
	glGenTextures(1,&(*textureID));
	glBindTexture(GL_TEXTURE_2D,*textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img->w,img->h,0,GL_RGBA,GL_UNSIGNED_BYTE,img->pixels);
	glBindTexture(GL_TEXTURE_2D,0);
}

void createImg(GLuint *textureID, SDL_Surface* img){
	glGenTextures(1,&(*textureID));
	glBindTexture(GL_TEXTURE_2D,*textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_RGB,GL_UNSIGNED_BYTE,img->pixels);
	glBindTexture(GL_TEXTURE_2D,0);
}
/*
Pareil pour ces 3 fonctions
*/

/* Initializes the bakcground and loads its image */
void loadBG(Background *bg, float speed, float pos){
    bg->position = pos;
    bg->speed = speed;
}

/* Initializes the ship and loads its image */
void loadShip(Ship *s){
    s->nb_life = 10;
    s->move = 0;
    s->pos = 0.0;
    s->speed = 0.03;
    s->lstA = NULL;
    //glGenTextures(1, &(s->textureID));
    //createImg(s->textureID, s->img);
	/*glBindTexture(GL_TEXTURE_2D,&s->textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,s->img->w,s->img->h,0,GL_RGBA,GL_UNSIGNED_BYTE,s->img->pixels);
	glBindTexture(GL_TEXTURE_2D,0);*/
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
    list->img = IMG_Load(imgSrc);
    if(list->img == NULL){
        printf("Error at loading ARROW image\n");
        return NULL;
    }
    glGenTextures(1, &(list->textureID));
    createImgAlpha(&list->textureID, list->img);

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
    free(elt);
}



/************************************

À faire :

- Me rendre encore un peu plus propre ces séries de 3 fonctions que se balladent (parce que le jour où tu auras la mobs, les boss, les murs... tu vas avoir 15 fonctions différentes et ça c'est pas super cool

Et puis là t'auras déjà une bonne base.

*************************************/