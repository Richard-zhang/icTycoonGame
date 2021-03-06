#include <assert.h>
#include "main.h"

int main() {
    init();

    bool quit = false;
    SDL_Event e;
    int count = 0;

    while(!quit) {

        SDL_WaitEvent(&e);

        if (e.type == SDL_QUIT) quit = true;

        if (e.key.keysym.sym == SDLK_q) quit = true;

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            if (e.key.keysym.sym == SDLK_r) {
                int i = count;

                struct player *currentPlayer = listOfPlayers[i];

                int previousBoard = currentPlayer->currentBlock;

                int dice = mimicDice();

                if (currentPlayer->inUCL) {
                    if (dice == 12 || dice == 1 || dice == 2) {
                        currentPlayer->inUCL = false;
                        currentPlayer->currentBlock = 10;
                        currentPlayer->rect = listOfSquare[10];
                    }
                    goto RENDER;
                }

                currentPlayer->currentBlock = (currentPlayer->currentBlock + dice) % (NUM_SQUARE - 1);

                currentPlayer->rect = listOfSquare[currentPlayer->currentBlock];

                Square *currentSquare = listOfLogicSquare[currentPlayer->currentBlock];

                animation(previousBoard, i);

                collisionDetection(i);

                globalRender();

                squareProcessing(i, currentSquare, dice);

                count = (count + 1) % NUM_PLAYER;
            } 
        }

        RENDER:
        globalRender();
    }



    close();
    deleteBoard();
}



































void globalRender() {
    SDL_RenderClear(gRenderer);
    SDL_RenderCopy(gRenderer, board, NULL, NULL);
    renderMoney();
    renderMarks();
    renderAllPlayer();
    SDL_RenderPresent(gRenderer);
}

void squareProcessing(int i, Square *currentSquare, int dice) {
    switch (currentSquare->type) {
        case DEPARTMENT:
            department(i);
            break;
        case STATION:
            station(i);
            break;
        case UTILITY:
            utility(i, dice);
            break;
        case CHANCE:
            chance(i);
            break;
        case TAX:
            tax(i);
            break;
        case FREEPIZZA:
            freePizza(i);
            break;
        case UCL:
            ucl(i);
            break;
        case GOTOUCL:
            goToUCL(i);
            break;
        case GO:
            break;
        default:
            printf("even god can't let this be printed");
            exit(EXIT_FAILURE);
    }
}

void goToUCL(int id) {
    struct player *currentPlayer = listOfPlayers[id];

    currentPlayer->inUCL = true;
    currentPlayer->rect = listOfSquare[40];
    currentPlayer->currentBlock = 40;
}

void department(int id) {

    struct player *currentPlayer = listOfPlayers[id];

    Square *curSquare = listOfLogicSquare[currentPlayer->currentBlock];
    if(curSquare->owner == -1) {
        buy(id);
    } else {
        if (curSquare->owner != id) {
            lostMoney(id, curSquare->fine);
            gainMoney(curSquare->owner, curSquare->fine);
        }
    }

}
void station(int id) {
    struct player *currentPlayer = listOfPlayers[id];
    Square *curSquare = listOfLogicSquare[currentPlayer->currentBlock];
    if(curSquare->owner == -1) {
        buy(id);
    } else {
        if (curSquare->owner != id) {
            int stations = 0;
            int owner = curSquare->owner;

            if (listOfLogicSquare[5]->owner == owner) stations++;
            if (listOfLogicSquare[15]->owner == owner) stations++;
            if (listOfLogicSquare[25]->owner == owner) stations++;
            if (listOfLogicSquare[35]->owner == owner) stations++;

            int penalty = (curSquare->fine) << (stations - 1);

            gainMoney(owner, penalty);
            lostMoney(id, penalty);
        }
    }
}

void utility(int id, int distance) {
    struct player *currentPlayer = listOfPlayers[id];
    Square *curSquare = listOfLogicSquare[currentPlayer->currentBlock];
    if(curSquare->owner == -1) {
        buy(id);
    } else {
        if (curSquare->owner != id) {
            int idAnotherUtil = (currentPlayer->currentBlock == 12) ? 28 : 12;
            int penalty;
            if (listOfLogicSquare[idAnotherUtil]->owner == curSquare->owner) {
                penalty = distance * 12;
            } else {
                penalty = distance * 4;
            }

            lostMoney(id, penalty);
            gainMoney(curSquare->owner, penalty);

        }
    }

}

void chance(int id) {
    srand((unsigned int) time(NULL));

    int index = rand() % NUM_OF_CHANCE;


    struct chanceCard *cur = listOfChances[index];

    switch (cur->type) {
        case GOUCL:
			listOfPlayers[id]->inUCL = true;
            listOfPlayers[id]->currentBlock = 40;
            listOfPlayers[id]->rect = listOfSquare[40];
            break;
        case MONEY:
            listOfPlayers[id]->money+=cur->amount;
            break;
        case MOVE:
            break;
        default:
            printf("can't touch it\n");
            exit(EXIT_FAILURE);
    }

    renderChance(index);

}

void renderChance(int index) {
    SDL_Rect posi;
    setRect(&posi, 0, index *  HEIGHT_CARD, WIDTH_CARD, HEIGHT_CARD);

    SDL_Rect middleOfBoard;
    setRect(&middleOfBoard, X_MIDDLE_BOARD, Y_MIDDLE_BOARD, WIDTH_CARD, HEIGHT_CARD);

    SDL_RenderCopy(gRenderer, gChance, &posi, &middleOfBoard);
    SDL_RenderPresent(gRenderer);
    SDL_Delay(1000);
}

void ucl(int id) {
}
void freePizza(int id) {

}
void tax(int id) {
    int amount;
    struct player *currentPlayer = listOfPlayers[id];
    switch (currentPlayer->currentBlock) {
        case 4:
            amount = 200;
            break;
        case 38:
            amount = 100;
            break;
        default:
            printf("Should not enter tax");
            exit(EXIT_FAILURE);
    }
    lostMoney(id, amount);
}

void buy(int id) {
    Square *cur = listOfLogicSquare[listOfPlayers[id]->currentBlock];
    SDL_Event e;
    bool quit=false;

    while(!quit) {
        SDL_WaitEvent(&e);

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            if (e.key.keysym.sym == SDLK_b) {
                printf("wise choice man\n");

                if (listOfPlayers[id]->money >= cur->price) {
                    cur->owner = id;
                    lostMoney(id, cur->price);
                    add(id);
                    //TODO Notification
                } else {
                    //TODO Trading
                }

                quit = true;
            } else {
                printf("not buy\n");
                quit = true;
            }
        }

        if (e.type == SDL_QUIT) {
            quit = true;
        }
    }


}

void lostMoney(int id, int amount) {
    listOfPlayers[id]->money -= amount;
}

void gainMoney(int id, int amount) {
    listOfPlayers[id]->money += amount;
}

void renderMoney() {
    for (int i = 0; i < NUM_PLAYER; i++) {

        struct player *cur = listOfPlayers[i];

        int aInt = cur->money;
        char str[15];
        sprintf(str, "%d", aInt);


        SDL_Texture *sdl_texture = createFromText(str);
        SDL_RenderSetViewport(gRenderer, &posiVecMoney[i]);
        SDL_RenderCopy(gRenderer, sdl_texture, NULL, NULL);
    }

    SDL_RenderSetViewport(gRenderer, NULL);
}

void collisionDetection(int i) {
    assert (i <= 3);

    for (int q = 0; q < NUM_PLAYER; q++) {
        if (q != i) {
            int id = listOfPlayers[q]->currentBlock;
            if (id == listOfPlayers[i]->currentBlock) {
                assert(id <= 40);
                assert(id >= 0);
                if (id >= 0 && id <= 10) {
                    (listOfPlayers[i]->rect).y += WIDTH_OF_PLAYER;

                } else if (id >= 11 && id <= 20) {
                    (listOfPlayers[i]->rect).x -= WIDTH_OF_PLAYER;

                } else if (id >= 21 && id <= 30) {
                    (listOfPlayers[i]->rect).y -= WIDTH_OF_PLAYER;

                } else if (id >= 31 && id <= 39) {
                    (listOfPlayers[i]->rect).x += WIDTH_OF_PLAYER;

                } else {
                    assert(id == 40);
                    (listOfPlayers[i]->rect).y += WIDTH_OF_PLAYER;

                }
            }
        }
    }

}

int mimicDice() {
    int dice;
    srand((unsigned int) time(NULL));

    dice = rand() % DICE_SIZE + 1;
    dice += rand() % DICE_SIZE + 1;

    return dice;
}

bool animatedRowOne(int start, int end, int i) {
    if (end <= start) {
        return false;
    }
    //board from 0 to 9;
    int frame = 0;
    int direction = 1;
    int start_x = listOfSquare[start].x;
    int end_x = listOfSquare[end].x;

    int y_axis = listOfSquare[start].y;

    while(start_x >= end_x) {

        SDL_RenderCopy(gRenderer, board, NULL, NULL);
        for (int q = 0; q < NUM_PLAYER; q++) {
            if (q != i) renderPlayer(q);
        }


        aniRenderPlayer(i, direction, frame, start_x, y_axis);

        renderMoney();
        renderMarks();

        SDL_RenderPresent(gRenderer);
        SDL_Delay(SPEED);
        SDL_RenderClear(gRenderer);


        frame = (frame + 1) % ANIMATION_FRAMES;
        start_x -= ANIMATION_DISPLACE;
    }

    return true;
}


bool animatedRowTwo(int start, int end, int i) {
    if(end <= start) {
        return false;
    }
    //board from 0 to 9;
    int frame = 0;
    int direction = 2;
    int start_x = listOfSquare[start].x;
    int end_x = listOfSquare[end].x;

    int y_axis = listOfSquare[start].y;

    while(start_x <= end_x) {

        SDL_RenderCopy(gRenderer, board, NULL, NULL);
        for (int q = 0; q < NUM_PLAYER; q++) {
            if (q != i) renderPlayer(q);
        }


        aniRenderPlayer(i, direction, frame, start_x, y_axis);

        renderMoney();
        renderMarks();

        SDL_RenderPresent(gRenderer);
        SDL_Delay(SPEED);
        SDL_RenderClear(gRenderer);


        frame = (frame + 1) % ANIMATION_FRAMES;
        start_x += ANIMATION_DISPLACE;
    }

    return true;
}


bool animatedColOne(int start, int end, int i) {
    if (end <= start) {
        return false;
    }
    int frame = 0;
    int direction = 3;
    int start_y = listOfSquare[start].y;
    int end_y = listOfSquare[end].y;

    int x_axis = listOfSquare[start].x;

    while(start_y >= end_y) {
        SDL_RenderCopy(gRenderer, board, NULL, NULL);
        for (int q = 0; q < NUM_PLAYER; q++) {
            if (q != i) renderPlayer(q);
        }

        aniRenderPlayer(i, direction, frame, x_axis, start_y);

        renderMoney();
        renderMarks();

        SDL_RenderPresent(gRenderer);
        SDL_Delay(SPEED);
        SDL_RenderClear(gRenderer);


        frame = (frame + 1) % ANIMATION_FRAMES;
        start_y -= ANIMATION_DISPLACE;
    }


    return true;
}

bool animatedColTwo(int start, int end, int i) {
    if (end <= start) {
        return false;
    }
    int frame = 0;
    int direction = 0;
    int start_y = listOfSquare[start].y;
    int end_y = listOfSquare[end].y;

    int x_axis = listOfSquare[start].x;

    while(start_y <= end_y) {
        SDL_RenderCopy(gRenderer, board, NULL, NULL);
        for (int q = 0; q < NUM_PLAYER; q++) {
            if (q != i) renderPlayer(q);
        }

        aniRenderPlayer(i, direction, frame, x_axis, start_y);

        renderMoney();
        renderMarks();

        SDL_RenderPresent(gRenderer);
        SDL_Delay(SPEED);
        SDL_RenderClear(gRenderer);


        frame = (frame + 1) % ANIMATION_FRAMES;
        start_y += ANIMATION_DISPLACE;
    }


    return true;
}

void aniRenderPlayer(int i,int direction, int frame ,int x, int y) {
    SDL_Rect re;
    setRect(&re, x, y, WIDTH_OF_PLAYER, WIDTH_OF_PLAYER);

    SDL_RenderCopy(gRenderer, players ,&aniSprite[i][direction][frame], &re);

}

void animation(int previousBoard, int i) {

    int currentBoard = listOfPlayers[i]->currentBlock;
    if (previousBoard <= currentBoard) {

        if (currentBoard <= 10) {
            animatedRowOne(previousBoard, currentBoard, i);
        } else if (currentBoard <= 20) {
            if (animatedRowOne(previousBoard, 10, i)) {
                animatedColOne(10, currentBoard, i);
            } else {
                animatedColOne(previousBoard, currentBoard, i);
            }

        } else if (currentBoard <= 30) {

            if (animatedRowOne(previousBoard, 10, i)) {
                animatedColOne(10, 20, i);
                animatedRowTwo(20, currentBoard, i);
            } else if (animatedColOne(previousBoard, 20, i)){
                animatedRowTwo(20, currentBoard, i);
            } else {
                animatedRowTwo(previousBoard, currentBoard, i);
            }

        } else if (currentBoard <= 39) {

            if (animatedColTwo(previousBoard, 20, i)) {
                animatedRowTwo(20, 30, i);
                animatedColTwo(30, currentBoard, i);
            } else if (animatedRowTwo(previousBoard, 30,i)){
                animatedColTwo(30, currentBoard, i);
            } else {
                animatedColTwo(previousBoard, currentBoard, i);
            }

        } else {
            fprintf(stderr, "animation wrong\n");
            exit(EXIT_FAILURE);
        }

    } else {
        if (previousBoard < 30) {
            animatedRowTwo(previousBoard, 30, i);
            animatedColTwo(30, 39, i);
            animatedRowOne(0, currentBoard, i);
        } else {
            animatedColTwo(previousBoard, 39, i);
            animatedRowOne(0, currentBoard, i);
        }
    }

}


void renderAllPlayer() {
    for (int q = 0; q < NUM_PLAYER; q++) {
        renderPlayer(q);
    }
}


void renderPlayer(int number) {
    SDL_RenderCopy(gRenderer, players, &renderQuad[number], &(listOfPlayers[number]->rect));
}


void init() {
    initEstates();
    initAnimation();

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    gWindow = SDL_CreateWindow("FINAL Trial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                               SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL );  
    
    
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    gfont = TTF_OpenFont("Font/future.ttf", FONT_SIZE);


    for (int i = 0; i < NUM_PLAYER; i++) {
        setRect(&renderQuad[i], 0, i*4*WIDTH_OF_PLAYER, WIDTH_OF_PLAYER, WIDTH_OF_PLAYER);
    }

    setRect(&startRect, 1240 - OFFSET, 920,WIDTH_OF_PLAYER , WIDTH_OF_PLAYER);

    for (int i = 0; i < NUM_PLAYER; i++) {
        setRect(&posiVecMoney[i],X_TOPLEFT_MONEY, Y_TOPLEFT_MONEY + i * GAP_MONEY, W_MONEY, H_MONEY);
    }

    initAnimation();
    loadBoard();
    loadCharacter();
    initPlayer();
    initSquares();
    initChances();

}

void initPlayer() {
    for (int i = 0; i < NUM_PLAYER; i++) {
        listOfPlayers[i] = (struct player *) malloc(sizeof(struct player));
        listOfPlayers[i]->rect = startRect;
    }


    for (int i = 0; i < NUM_PLAYER; i++) {
        listOfPlayers[i]->inGame = true;
        listOfPlayers[i]->inUCL = false;
        //listOfPlayers[i]->position = startRect;
        listOfPlayers[i]->type = (enum student) i;
        listOfPlayers[i]->currentBlock = 0;
        listOfPlayers[i]->money = 1000;
    }
}

void initSquaresHelp() {
    //first row
    for (int i = 0; i <= 10; i++) {
        setRect(&listOfSquare[i],startRect.x - 80 *i , startRect.y, WIDTH_OF_PLAYER, WIDTH_OF_PLAYER);
    }

    for (int i = 11; i <= 20; i++) {
        setRect(&listOfSquare[i], listOfSquare[10].x, listOfSquare[10].y-80 * (i-10), WIDTH_OF_PLAYER, WIDTH_OF_PLAYER);
    }

    for (int i = 21; i <= 30; i++) {
        setRect(&listOfSquare[i], listOfSquare[20].x + 80 * (i- 20), listOfSquare[20].y, WIDTH_OF_PLAYER, WIDTH_OF_PLAYER);
    }

    for (int i = 31; i <= 39; i++) {
        setRect(&listOfSquare[i], listOfSquare[30].x, listOfSquare[30].y + 80 * (i-30), WIDTH_OF_PLAYER, WIDTH_OF_PLAYER);
    }

    setRect(&listOfSquare[40], listOfSquare[10].x-85, listOfSquare[10].y, WIDTH_OF_PLAYER, WIDTH_OF_PLAYER);

}

void initSquares() {
    initSquaresHelp();
    initBoard();
}

SDL_Texture *createFromText(char *str) {
    SDL_Surface *sdl_surface;
    SDL_Color color = {0,0,0xFF};
    sdl_surface = TTF_RenderText_Solid(gfont, str, color);
    isError(sdl_surface);
    SDL_Texture *newTexture;
    newTexture = SDL_CreateTextureFromSurface(gRenderer, sdl_surface);
    isError(newTexture);
    SDL_FreeSurface(sdl_surface);

    return newTexture;
}

void close() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_DestroyTexture(board);
    SDL_DestroyTexture(players);
}

void isError(void *ip) {
    if (ip == NULL) {
        printf("Error is here:%s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void loadCharacter() {
    SDL_Surface *sprite;
    sprite = IMG_Load("Sprite/character.png");

    players = SDL_CreateTextureFromSurface(gRenderer, sprite);

    SDL_FreeSurface(sprite);
}

void initAnimation() {
    for (int i = 0; i < NUM_PLAYER; i++) {
        for (int q = 0; q < NUM_PLAYER; q++) {
            for (int j = 0; j < NUM_PLAYER; j++) {

                SDL_Rect ha;
                ha.x = j * WIDTH_OF_PLAYER;
                ha.y = i * (4 * WIDTH_OF_PLAYER) + q * WIDTH_OF_PLAYER;
                ha.w = WIDTH_OF_PLAYER;
                ha.h = WIDTH_OF_PLAYER;


                aniSprite[i][q][j] = ha;
            }
        }
    }
}


void loadBoard() {
    SDL_Surface *background;
    background = SDL_LoadBMP("/Sprite/board_final.bmp");
    board = SDL_CreateTextureFromSurface(gRenderer, background);


    SDL_FreeSurface(background);
}

void setRect(SDL_Rect *rect, int x, int y, int w, int h) {
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}


//init square
/*typedef struct square {
    int price;
    int fine;
    Type type;
    int owner;
} Square;*/

void initBoard(void) {
    //for (int i = 0; i < NUM_SQUARE; i++) listOfLogicSquare[i] = (Square *) malloc(sizeof(Square));

    //listOfLogicSquare[0]->type = GO;

    initNotBought();
    initBuyable();
}

void SetupPriceAndFine(int i, int price, int fine) {
    listOfLogicSquare[i]->fine = fine;
    listOfLogicSquare[i]->price = price;
}

void SetupDepartment(int i, int price, int fine) {
    SetupPriceAndFine(i, price, fine);
    listOfLogicSquare[i]->type = DEPARTMENT;
}

void SetupStation(int i, int price, int fine) {
    SetupPriceAndFine(i, price, fine);
    listOfLogicSquare[i]->type = STATION;
}

void SetupUtility(int i, int price, int fine) {
    SetupPriceAndFine(i, price, fine);
    listOfLogicSquare[i]->type = UTILITY;
}



/* Initialises 28 buyable squares */
void initBuyable(void) {
    initStations();
    initDepartments();
    initUtility();
}

/* Initilaises the utility squares */
void initUtility(void) {
    Square *square1 = createUtility();
    Square *square2 = createUtility();

    // Initialise Union
    //square1->name = "Union";
    listOfLogicSquare[12] = square1;

    // Initialise CSG
    //square2->name = "CSG";
    listOfLogicSquare[28] = square2;
}

Square *createUtility(void) {
    Square *square = (Square *) malloc(sizeof(Square));
    isInitSquare(square);

    square->price = 150;
    square->fine = 0;
    square->type = UTILITY;
    //square->rect = NULL; //TODO: initialise this.
    square->owner = -1;
    return square;
}

/* Initialises four stations */
void initStations(void) {
    Square *square1 = createStation();
    Square *square2 = createStation();
    Square *square3 = createStation();
    Square *square4 = createStation();

    //square1->name = "TasteImperial: Fusion";
    //square2->name = "QTR";
    //square3->name = "JCR";
    //square4->name = "Library cafe";

    listOfLogicSquare[5] = square1;
    listOfLogicSquare[15] = square2;
    listOfLogicSquare[25] = square3;
    listOfLogicSquare[35] = square4;
}

/* Creates a station */
Square *createStation(void) {
    Square *square = (Square *) malloc(sizeof(Square));
    isInitSquare(square);

    square->price = 200;
    square->fine = 25;
    square->type = STATION;
    //square->rect = NULL; //TODO: initialise this
    square->owner = -1;
    return square;
}


/* Initialises departments */
void initDepartments(void) {
    initBrown();
    initSkyBlue();
    initPink();
    initOrange();
    initRed();
    initYellow();
    initGreen();
    initBlue();
}

/* create square for department */
Square *createSquare(void) {
    Square *square = (Square *) malloc(sizeof(Square));
    isInitSquare(square);

    square->type = DEPARTMENT;
    square->owner = -1;
    //square->rect = NULL; //TODO: Initialise this
    return square;
}

/* Initialises brown squares on board */
void initBrown(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();

    // Initialise chemEng
    //square1->name = "chemEng";
    square1->price = 60;
    square1->fine = 2;
    listOfLogicSquare[1] = square1;

    // Initialise biology
    //square2->name = "Biology";
    square2->price = 60;
    square2->fine = 4;
    listOfLogicSquare[3] = square2;
}


/* Initialises sky blue squares on board */
void initSkyBlue(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();
    Square *square3 = createSquare();

    // Initialise Business School
    //square1->name = "Business School";
    square1->price = 100;
    square1->fine = 6;
    listOfLogicSquare[6] = square1;

    // Initialise chemistry
    //square2->name = "Chemistry";
    square2->price = 100;
    square2->fine = 6;
    listOfLogicSquare[8] = square2;

    // Initialise Maths
    //square3->name = "Maths";
    square3->price = 120;
    square3->fine = 8;
    listOfLogicSquare[9] = square3;
}


/* Initialises pink squares on board */
void initPink(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();
    Square *square3 = createSquare();

    // Initialise Physics
    //square1->name = "Physics";
    square1->price = 140;
    square1->fine = 10;
    listOfLogicSquare[11] = square1;

    // Initialise Geology
    //square2->name = "Geology";
    square2->price = 140;
    square2->fine = 10;
    listOfLogicSquare[13] = square2;

    // Initialise Metric
    //square3->name = "Metric";
    square3->price = 160;
    square3->fine = 12;
    listOfLogicSquare[14] = square3;
}

/* Initialises orange squares on board */
void initOrange(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();
    Square *square3 = createSquare();

    // Initialise Materials
    //square1->name = "Materials";
    square1->price = 180;
    square1->fine = 14;
    listOfLogicSquare[16] = square1;

    // Initialise DesignEng
    //square2->name = "DesignEng";
    square2->price = 180;
    square2->fine = 14;
    listOfLogicSquare[18] = square2;

    // Initialise 568
    //square3->name = "568";
    square3->price = 200;
    square3->fine = 16;
    listOfLogicSquare[19] = square3;
}


/* Initialises red squares on board */
void initRed(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();
    Square *square3 = createSquare();

    // Initialise BioEng
    //square1->name = "BioEng";
    square1->price = 220;
    square1->fine = 18;
    listOfLogicSquare[21] = square1;

    // Initialise Ethos
    //square2->name = "Ethos";
    square2->price = 240;
    square2->fine = 18;
    listOfLogicSquare[23] = square2;

    // Initialise Civil
    //square3->name = "Civil";
    square3->price = 240;
    square3->fine = 20;
    listOfLogicSquare[24] = square3;
}


/* Initialises yellow squares on board */
void initYellow(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();
    Square *square3 = createSquare();

    // Initialise Mech Eng
    //square1->name = "Mech Eng";
    square1->price = 260;
    square1->fine = 22;
    listOfLogicSquare[26] = square1;

    // Initialise Great Hall
    //square2->name = "Great Hall";
    square2->price = 260;
    square2->fine = 22;
    listOfLogicSquare[27] = square2;

    // Initialise EEE
    //square3->name = "EEE";
    square3->price = 280;
    square3->price = 280;
    square3->fine = 22;
    listOfLogicSquare[29] = square3;
}

/* Initialises green squares on board */
void initGreen(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();
    Square *square3 = createSquare();

    // Initialise Central Library
    //square1->name = "Central Library";
    square1->price = 300;
    square1->fine = 26;
    listOfLogicSquare[31] = square1;

    // Initialise Medicine
    //square2->name = "Medicine";
    square2->price = 300;
    square2->fine = 26;
    listOfLogicSquare[32] = square2;

    // Initialise Aero
    //square3->name = "Aero";
    square3->price = 320;
    square3->fine = 28;
    listOfLogicSquare[34] = square3;
}

/* Initialises blue squares on board */
void initBlue(void) {
    Square *square1 = createSquare();
    Square *square2 = createSquare();

    // Initialise Computing
    //square1->name = "Computing";
    square1->price = 350;
    square1->fine = 35;
    listOfLogicSquare[37] = square1;

    // Initialise Queen's Tower
    //square2->name = "Queen's Tower";
    square2->price = 400;
    square2->fine = 50;
    listOfLogicSquare[39] = square2;
}

/* Initialises the 12 non-buyable squares, with 6 being unique.
   Chance squares are not unique. */
void initNotBought(void) {
    initGO();
    initUCL();
    initPizza();
    initGoToUCL();
    initSuperTax();
    initIncomeTax();
    initChance();
}

/* Initialises free pizza square */
void initPizza(void) {
    Square *square = initNonBuyable();
    //square->name = "Free pizza";
    square->type = FREEPIZZA;
    listOfLogicSquare[20] = square;
}

/* Initialises super tax */
void initSuperTax(void) {
    Square *square = initNonBuyable();
    //square->name = "Super Tax";
    square->type = TAX;
    listOfLogicSquare[38] = square;
}

/* Initialises income tax */
void initIncomeTax(void) {
    Square *square = initNonBuyable();
    //square->name = "Income Tax";
    square->type = TAX;
    listOfLogicSquare[4] = square;
}

/* Initialises chance squares */
void initChance(void) {
    Square *chance1 = createChance();
    Square *chance2 = createChance();
    Square *chance3 = createChance();
    Square *chance4 = createChance();
    Square *chance5 = createChance();
    Square *chance6 = createChance();

    listOfLogicSquare[2] = chance1;
    listOfLogicSquare[7] = chance2;
    listOfLogicSquare[17] = chance3;
    listOfLogicSquare[22] = chance4;
    listOfLogicSquare[33] = chance5;
    listOfLogicSquare[36] = chance6;
}

/* initialises a chance */
Square * createChance(void) {
    Square *square = initNonBuyable();
    //square->name = "Chance";
    square->type = CHANCE;
    return square;
}

/* Initialises the GO square */
void initGO(void) {
    Square *square = initNonBuyable();
    //square->name = "GO";
    square->type = GO;
    listOfLogicSquare[0] = square;
}

/* Initialises the UCL (Jail) */
void initUCL(void) {
    Square *square = initNonBuyable();
    //square->name = "UCL";
    square->type = UCL;
    listOfLogicSquare[10] = square;

    //SECOND UCL SQUARE (square 40)
    Square *square2 = initNonBuyable();
    //square2->name = "UCL2";
    square2->type = UCL;
    listOfLogicSquare[40] = square2;
}

/* Initialises go to UCL (go to jail) */
void initGoToUCL(void) {
    Square *square = initNonBuyable();
    //square->name = "GO TO UCL";
    square->type = GOTOUCL;
    listOfLogicSquare[30] = square;
}

Square *initNonBuyable(void) {
    Square *square = (Square *) malloc(sizeof(Square));
    isInitSquare(square);

    square->price = 0;
    square->fine = 0;
    square->owner = -1;
    //square->rect = NULL; //TODO: initialise this
    return square;
}

void deleteBoard(void) {
    int i;
    for(i = 0; i < NUM_SQUARE; i++) {
        free(listOfLogicSquare[i]);
    }
}

void isInitSquare(Square *square) {
    if (square == NULL) {
        perror("Square not initialised");
        exit(EXIT_FAILURE);
    }
}

SDL_Texture *loadMedia(char *path) {
    SDL_Surface *sprite;
    sprite = IMG_Load(path);

    if (sprite == NULL) {
        printf("Error in Loading media probably wrong path or %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Texture *gTexture;
    gTexture = SDL_CreateTextureFromSurface(gRenderer, sprite);
    SDL_FreeSurface(sprite);

    return gTexture;
}

void initChances() {
    SDL_Surface *sprite;
    sprite = IMG_Load("Sprite/chance.png");

    gChance = SDL_CreateTextureFromSurface(gRenderer, sprite);


    listOfChances[0] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[0]->amount = -170;
    listOfChances[0]->type = MONEY;

    listOfChances[1] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[1]->amount = -50;
    listOfChances[1]->type = MONEY;

    listOfChances[2] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[2]->amount = -100;
    listOfChances[2]->type = MONEY;

    listOfChances[3] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[3]->amount = 50;
    listOfChances[3]->type = MONEY;

    listOfChances[4] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[4]->amount = 0;
    listOfChances[4]->type = GOUCL;

    listOfChances[5] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[5]->amount = 150;
    listOfChances[5]->type = MONEY;

    listOfChances[6] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[6]->amount = -50;
    listOfChances[6]->type = MONEY;

    listOfChances[7] = (struct chanceCard *) malloc(sizeof(struct chanceCard));
    listOfChances[7]->amount = 200;
    listOfChances[7]->type = MONEY;
    
}

void initEstates() {
    for (int i = 0; i < NUM_PLAYER; i++) {
        for (int j = 0; j < NUM_SQUARE; j++)
            estates[i][j] = -1;

        num_of_estats[i] = 0;
    }
}

void add(int i) {
    int cur = listOfPlayers[i]->currentBlock;
    int index = num_of_estats[i];
    estates[i][index] = cur;

    num_of_estats[i]++;
}

void renderMarks() {
    for (int i = 0; i < NUM_PLAYER; i++) {
        int num = num_of_estats[i];

        for (int q = 0; q < num; q++) {
            helpRenderMark(estates[i][q], i);
        }
    }

}

void helpRenderMark(int posi, int i) {
    //first row
    if (i == 0) {
        SDL_SetRenderDrawColor( gRenderer,204 , 99, 151, 0xFF );
    } else if (i == 1) {
        SDL_SetRenderDrawColor(gRenderer, 0, 12, 61, 0xFF);
    } else if (i == 2) {
        SDL_SetRenderDrawColor(gRenderer, 0, 0xFF, 0, 0xFF);
        //SDL_SetRenderDrawColor(gRenderer,195, 195, 196, 0xFF);
    } else {
        SDL_SetRenderDrawColor(gRenderer, 0xFF,0,0,0xFF);
    }

    SDL_Rect ref = listOfSquare[posi];
    SDL_Rect fillrect;
    fillrect.w = WIDTH_OF_MARK;
    fillrect.h = WIDTH_OF_MARK;
    if (posi >= 1 && posi <= 9) {
        fillrect.x = ref.x;
        fillrect.y = 880;
    } else if (posi >= 11 && posi <= 19) {
        fillrect.x = 280;
        fillrect.y = ref.y;
    } else if (posi >= 21 && posi <= 29) {
        fillrect.x = ref.x;
        fillrect.y = 182;
    } else if (posi >= 31 && posi <= 39) {
        fillrect.x = 980;
        fillrect.y = ref.y;
    } else {
        printf("not a buyable thing, can't give a mark\n");
        exit(EXIT_FAILURE);
    }

    SDL_RenderFillRect(gRenderer, &fillrect);
}


