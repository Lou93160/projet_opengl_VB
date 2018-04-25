
/************** MAIN ***************/


int main(int argc, char** argv) {
    WorldGame world;
    int level = 0; /* Level of the game */

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


    /* Background loading */
    Background bg1;
    Background bg2;
    float scrollSpeed = 0.009; /* Backgroundspeed */
    loadBG(&bg1, "./img/fds/niv1.jpg", scrollSpeed, 0);
    loadBG(&bg2, "./img/fds/niv1.jpg", scrollSpeed, 2);

    /* Ship loading */
    Ship ship;
    int move = 0; /* Ship movement by default */
    loadShip(&ship, "./img/elts/ship.png");

    /* Assignment of the world */
    world.ships = ship;
    world.level = level;

    int loop = 1;
    glClearColor(0, 0, 0, 1.0);


    /*********** LOOP **********/

    while(loop) {

        Uint32 startTime = SDL_GetTicks();

        glClear(GL_COLOR_BUFFER_BIT);

        /* Drawing */
        moveBackground(&bg1, 1);
        moveBackground(&bg2, 1);
        drawBG(&bg1, 1, 1);
        drawBG(&bg2, 1, 1);
        drawShip(&ship, 0.11, 0.135, move);

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
                            /* Fill the arrowlist into the ship structure */
                            pushArrow(&ship);
                            break;
                        default:
                            move = 0;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    if(e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_UP){
                        move = 0;
                    }
                    break;

                default:
                    break;
                }
            }

        /* If move has changed */
        if(move != 0){
           moveShip(move, &ship);
        }

        /* When space touch is down -> ship.lstA is filled */
        while(ship.lstA != NULL){
            drawArrow(&(*ship.lstA), 0.04, 0.04);
            if (moveArrow(&(*ship.lstA)) == 0){
                deleteArrow(&ship, ship.lstA);
            }
        }

        SDL_GL_SwapBuffers();
        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if(elapsedTime < FRAMERATE_MILLISECONDS) {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }

    glDeleteTextures(1, &ship.textureID);
    glDeleteTextures(1, &bg1.textureID);
    glDeleteTextures(1, &bg2.textureID);

    SDL_Quit();

    return EXIT_SUCCESS;
}