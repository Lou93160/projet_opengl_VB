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
            glRotatef(10.0*move, 0, 0, 1.0);
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
Background loadBG(float speed, float pos){
	Background *tmp = (Background*)malloc(sizeof(Background));
    tmp->position = pos;
    tmp->speed = speed;
    return *tmp;
}

/* Initializes the ship and loads its image */
Ship loadShip(Ship *s){
	Ship *tmp = (Ship*)malloc(sizeof(Ship));
    tmp->nb_life = 10;
    tmp->move = 0;
    tmp->pos = 0.0;
    tmp->speed = 0.03;
    tmp->lstA = NULL;
    return *tmp;
}

/* Function called by the pushArrow which allocs, load and return the list
Argu are:
the arrowlist in the ship structure;
the source image;
pInList indicates the position of the image to load in the list (+1 if next, 0 if the instant one);
position Y of the ship (arrow start point).
*/
ListeArrow loadArrow(ListeArrow list, GLuint textureID, int pInList, float pos){
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
    list->textureID = textureID;
    if(pInList == 1) list->Aprec = list;
    else list->Aprec = NULL;

    return list;
}

/* Function called when space touch is down
and it allocates or pushes into the ship's list arrows */
void pushArrow(Ship *s, GLuint textureID){
    /* Verification of the listArrow : filled or not ? 
    if yes, we cross the list and place the instant arrow at the end */
    if (s->lstA != NULL){
        while(s->lstA->Asuiv != NULL){
            s->lstA = s->lstA->Asuiv;
        }
        /* Allocation of the new arrow & image loading in loadArrow*/
        s->lstA = loadArrow(s->lstA, textureID, 1, s->pos);
    }
    /* Else we malloc and initialize the list */
    s->lstA = loadArrow(s->lstA, textureID, 0, s->pos);
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

Element* initElement(GLuint textureID, float x, float y, int life){
	Element *tmp = (Element*)malloc(sizeof(Element));
	tmp->textureID = textureID;
	tmp->posX = x;
	tmp->posY = y;
	tmp->nb_life = life;
	tmp->next = NULL;
	return tmp;
}

void addElementToList(ListeElement *lst, GLuint textureID, float x, float y, int life){
	Element *tmp = initElement(textureID, x, y, life);
	if (lst != NULL){
		tmp->next = *lst;
    }
    *lst = tmp;
}

void pushElements(ListeElement *bonus, ListeElement *mobs, ListeElement *obstacles){
	char line[100];
	FILE *file = fopen("./img/ppm/level1.ppm", "r");
	/* Read line per line the file */
	fgets(line, sizeof(line), file); /* Line P3 */
	fgets(line, sizeof(line), file); /* Line GIMP... */
	fgets(line, sizeof(line), file); /* Line Width & Height */
	sscanf(line," %d %d", &(World.width), &(World.height));
	fgets(line, sizeof(line), file); /* 255 line */
	int count = 0;
	int r, g, b;
	SDL_Surface *imgObstacles = IMG_Load("./img/elts/CHOIX/chameau.png");
	if(imgObstacles == NULL){
		printf("ERROR at loading OBSTACLES image\n");
		exit(1);
	}
	GLuint idObstacles;
	createImgAlpha(&idObstacles, imgObstacles);
	SDL_Surface *imgBonus = IMG_Load("./img/elts/CHOIX/oeil.png");
	if(imgBonus == NULL){
		printf("ERROR at loading BONUS image\n");
		exit(1);
	}
	GLuint idBonus;
	createImgAlpha(&idBonus, imgBonus);
	SDL_Surface *imgMobs = IMG_Load("./img/elts/CHOIX/momie.png");
	if(imgMobs == NULL){
		printf("ERROR at loading MOBS image\n");
		exit(1);
	}
	GLuint idMobs;
	createImgAlpha(&idMobs, imgMobs);
	while(fgets(line, sizeof(line), file) != NULL){
		if(count%3 == 0) {
			sscanf(line, " %d", &r);
		}
		else if(count%3 == 1){
			sscanf(line, " %d", &g);
		} 
		else if(count%3 == 2){
			sscanf(line, " %d", &b);	
			if( r == 0 && g == 0 && b == 0){
				addElementToList(obstacles, idObstacles, (count/3)%((int)World.width), floor((count/3)/((int)World.width)), 2);
			}
			else if(r == 0 && g == 255 && b == 0){
				addElementToList(bonus, idBonus, (count/3)%((int)World.width), floor((count/3)/(int)(World.width)), 1);
			}
			else if(r==255 && g == 0 && b == 0){
				addElementToList(mobs, idMobs, (count/3)%((int)World.width), floor((count/3)/((int)World.width)), 1);
			}
		}
		count++;
	}
}

void drawElement(Element elt, GLuint textureID, float incremente){
	glPushMatrix();
		glTranslatef(-1.0,1.0,0.0);
        glScalef(2.0/World.height,-2.0/World.height, 1.0);
        glTranslatef(elt.posX-incremente, elt.posY, 0);
		draw(textureID, 1, -1);
	//drawsquare();
    glPopMatrix();
}

void drawListElements(ListeElement lst, float incremente){
	while(lst!=NULL){
		drawElement(*lst, lst->textureID, incremente);
		lst=lst->next;
	}
}

void displayList(ListeElement lst){
	while(lst!=NULL){
		printf("%f %f\n", lst->posX, lst->posY);
		lst=lst->next;
	}
}

void drawsquare(){
	glBegin(GL_POLYGON);
    glVertex2f(-1,1);

    glVertex2f(1,1);

    glVertex2f(1,-1);

    glVertex2f(-1,-1);
    glEnd();
}
/************************************

À faire :

- Me rendre encore un peu plus propre ces séries de 3 fonctions que se balladent (parce que le jour où tu auras la mobs, les boss, les murs... tu vas avoir 15 fonctions différentes et ça c'est pas super cool

Et puis là t'auras déjà une bonne base.

*************************************/