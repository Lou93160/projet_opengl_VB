#include "minimal.h"
#include "main.c"

/******* FUNCTIONS *******/

void resizeViewport(int w, int h) {
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;
    SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW); // PROJECTION?
    glLoadIdentity();
    gluOrtho2D(-1., 1., -1., 1.);
}

void moveBackground(Background *bg, int pStart){
    if(bg->position < - 1.98) bg->position = 1.99;
    else bg->position -= bg->speed;
}

void moveShip(int move, WorldGame *w, Ship *s){
    /* Up */
    if (move == 1){
        if(w->ships.pos + w->ships.speed <= 0.9){
            w->ships.pos += w->ships.speed;
        }
    }
    /* Down */
    else if (move == -1){
        if(w->ships.pos - w->ships.speed >= -1){
            w->ships.pos -= w->ships.speed;
        }
    }
    w->ships.bbox[0] = (-0.7+(0.5))*(0.135/1.2); // x1
    w->ships.bbox[1] = (w->ships.pos+(0.5))*(0.11/1.2); // y1
    w->ships.bbox[2] = (-0.7-(0.5))*(0.135/1.2); // x2
    w->ships.bbox[3] = (w->ships.pos-(0.5))*(0.11/1.2); //y2
    *s = w->ships;
}

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
            glRotatef(10.0*move, 0, 0, 1.0); // rotate down or up
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

void draw(GLuint textureID, float scaleW, float scaleH){
        /* Transparency */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        /* Brackground drawing */
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
            glPushMatrix();
                glScalef(scaleW,scaleH,1.0); /* z at 1 */
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

Background loadBG(float speed, float pos){
	Background *tmp = (Background*)malloc(sizeof(Background));
    tmp->position = pos;
    tmp->speed = speed;
    return *tmp;
}

Ship loadShip(Ship *s){
	Ship *tmp = (Ship*)malloc(sizeof(Ship));
    tmp->nb_life = 10;
    tmp->score = 0;
    tmp->move = 0;
    tmp->pos = 0.0;
    tmp->speed = 0.03;
    tmp->lstA = NULL;
    tmp->bbox[0] = (-0.7+(0.5))*(0.135/1.2); // x1
    tmp->bbox[1] = (tmp->pos+(0.5))*(0.11/1.2); // y1
    tmp->bbox[2] = (-0.7-(0.5))*(0.135/1.2); // x2
    tmp->bbox[3] = (tmp->pos-(0.5))*(0.11/1.2); //y2
    return *tmp;
}

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
	tmp->collision = 0;
	tmp->nb_life = life;
	tmp->next = NULL;
    tmp->bbox[0] = (tmp->posX+(0.5))*(0.029850); // x1 2/67
    tmp->bbox[1] = (tmp->posY+(0.5))*(0.0476190); // y1 1/21
    tmp->bbox[2] = (tmp->posX-(0.5))*(0.029850); // x2 2/67
    tmp->bbox[3] = (tmp->posY-(0.5))*(0.0476190); // y2 1/21
	return tmp;
}

void addElementToList(ListeElement *lst, GLuint textureID, float x, float y, int life){
	Element *tmp = initElement(textureID, x, y, life);
	if (lst != NULL){
		tmp->next = *lst;
    }
    *lst = tmp;
}

void pushElements(ListeElement *bonus, ListeElement *mobs, ListeElement *obstacles, ListeElement *keys, int level){
	char line[100];
	if(level == 1){
		FILE *file = fopen("./img/ppm/level1.ppm", "r");
	}
	FILE *file = fopen("./img/ppm/level1.ppm", "r");
	/* Read line per line the file */
	fgets(line, sizeof(line), file); /* Line P3 */
	fgets(line, sizeof(line), file); /* Line GIMP... */
	fgets(line, sizeof(line), file); /* Line Width & Height */
	sscanf(line," %d %d", &(World.width), &(World.height));
	fgets(line, sizeof(line), file); /* 255 line */
	int count = 0;
	int r, g, b;

	/****** IMAGES LOADING *******/
	/* Wall image loading */
	SDL_Surface *imgObstacles = IMG_Load("./img/elts/wall.png");
	if(imgObstacles == NULL){
		printf("ERROR at loading OBSTACLES image\n");
		exit(1);
	}
	GLuint idObstacles;
	createImgAlpha(&idObstacles, imgObstacles);
	/* Bonus image loading */
	SDL_Surface *imgBonus = IMG_Load("./img/elts/eye.png");
	if(imgBonus == NULL){
		printf("ERROR at loading BONUS image\n");
		exit(1);
	}
	GLuint idBonus;
	createImgAlpha(&idBonus, imgBonus);
	/* Mobs image loading */
	SDL_Surface *imgMobs = IMG_Load("./img/elts/mummy.png");
	if(imgMobs == NULL){
		printf("ERROR at loading MOBS image\n");
		exit(1);
	}
	GLuint idMobs;
	createImgAlpha(&idMobs, imgMobs);
	/* Key image loading */
	SDL_Surface *imgKeys = IMG_Load("./img/elts/key.png");
	if(imgKeys == NULL){
		printf("ERROR at loading KEYS image\n");
		exit(1);
	}
	GLuint idKeys;
	createImgAlpha(&idKeys, imgKeys);

	/******* Reading loop of the ppm image *******/
	while(fgets(line, sizeof(line), file) != NULL){
		if(count%3 == 0) {
			sscanf(line, " %d", &r);
		}
		else if(count%3 == 1){
			sscanf(line, " %d", &g);
		}
		else if(count%3 == 2){
			sscanf(line, " %d", &b);
			/* Black/Obstacles pixels */
			if( r == 0 && g == 0 && b == 0){
				addElementToList(obstacles, idObstacles, (count/3)%((int)World.width), floor((count/3)/((int)World.width)), 2);
			}
			/* Green/Bonus pixels */
			else if(r == 0 && g == 255 && b == 0){
				addElementToList(bonus, idBonus, (count/3)%((int)World.width), floor((count/3)/(int)(World.width)), 1);
			}
			/* Red/Mobs pixels */
			else if(r == 255 && g == 0 && b == 0){
				addElementToList(mobs, idMobs, (count/3)%((int)World.width), floor((count/3)/((int)World.width)), 1);
			}
			/* Blue/Keys pixels */
			else if(r == 0 && g == 0 && b == 255){
				addElementToList(keys, idKeys,(count/3)%((int)World.width), floor((count/3)/((int)World.width)), 1);
			}
		}
		count++;
	}
}

void drawElement(Element *elt, GLuint textureID, float incremente){
	glPushMatrix();
		glTranslatef(-1.0,1.0,0.0);
        glScalef(2.0/World.height,-2.0/World.height, 1.0);
        glTranslatef(elt->posX - incremente, elt->posY, 0);
		draw(textureID, 1, -1);
	//drawsquare(); debugging
    glPopMatrix();
    elt->bbox[0] -= incremente; // x1
    elt->bbox[2] -= incremente; // x2
}

void drawListElements(ListeElement lst, float incremente){
	while(lst!=NULL){
		drawElement(lst, lst->textureID, incremente);
		lst=lst->next;
	}
}

void collisionTest(WorldGame *w){
  //Parcours des listes d'elts de world game

  //test avec les bonus seulement
  ListeElement tmp = w->bonus;
  int count = 0;
  while (tmp != NULL){
    //printf("w1 %f tmp2 %f w3 %f tmp0 %f\n",w->ships.bbox[1], tmp->bbox[2], w->ships.bbox[3], tmp->bbox[0]);
    printf("w0 %f tmp2 %f w2 %f tmp0 %f\n",w->ships.bbox[0], tmp->bbox[2], w->ships.bbox[2], tmp->bbox[0]);
    if(w->ships.bbox[0] >= tmp->bbox[2] && w->ships.bbox[2] <= tmp->bbox[0]){
        printf("BIM\n");
        if(w->ships.bbox[1] >= tmp->bbox[3] && w->ships.bbox[3] <= tmp->bbox[1]){
            //COLLISION
            printf("COLLISION 11\n");
            //retirer des points de vie
            //collision de tmp = 1
        }
    }
    printf("count : %d\n", count);
    tmp = tmp->next;
    count++;
  }
}

/* Debugging */
void displayList(ListeElement lst){
	while(lst!=NULL){
		printf("%f %f\n", lst->posX, lst->posY);
		lst=lst->next;
	}
}

/* Debugging */
void drawsquare(){
	glBegin(GL_POLYGON);
    glVertex2f(-1,1);

    glVertex2f(1,1);

    glVertex2f(1,-1);

    glVertex2f(-1,-1);
    glEnd();
}
