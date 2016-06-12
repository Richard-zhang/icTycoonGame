//
// Created by R on 6/4/16.
//
#ifndef FINALTRIAL_MAIN_H
#define FINALTRIAL_MAIN_H

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#define WIDTH 1918
#define HEIGHT 1086
#define FONT_SIZE 100
#define NUM_PLAYER 4
#define WIDTH_OF_PLAYER 45
#define NUM_SQUARE 41
#define SPEED 30
#define DICE_SIZE 6
#define OFFSET 224

#define X_TOPLEFT_MONEY 1547
#define Y_TOPLEFT_MONEY 134
#define W_MONEY 70
#define H_MONEY 28
#define GAP_MONEY 40

#define HEIGHT_CARD 330
#define WIDTH_CARD 577
#define NUM_OF_CHANCE 8
#define X_MIDDLE_BOARD 354
#define Y_MIDDLE_BOARD 369



//animation
#define ANIMATION_FRAMES 3
#define ANIMATION_DISPLACE 10


//estate
#define WIDTH_OF_MARK 20
void add(int i);
int estates[NUM_PLAYER][NUM_SQUARE];
void initEstates();
void renderMarks();
int num_of_estats[NUM_PLAYER];
void helpRenderMark(int posi, int i);

enum student{
    INTERNATIONAL = 0,
    LOCAL,
    EUROPEAN,
    JAPAN
};

struct player {
    int money;
    int currentBlock;
    bool inUCL;
    bool inGame;
    enum student type;
    SDL_Rect rect;
};


typedef enum TYPE {
    GO,
    DEPARTMENT,
    STATION,
    CHANCE,
    UCL,
    FREEPIZZA,
    GOTOUCL,
    TAX,
    UTILITY
} Type;

typedef struct square {
    int price;
    int fine;
    Type type;
    int owner;
} Square;

typedef enum CHANCE {
    GOUCL,
    MONEY,
    MOVE
} Chance;

struct chanceCard {
    int amount;
    Chance type;
};

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

SDL_Texture* marks;
SDL_Texture* board;
SDL_Texture* players;
SDL_Texture* gChance;
SDL_Rect renderQuad[NUM_PLAYER];
struct player *listOfPlayers[NUM_PLAYER];
TTF_Font *gfont;
SDL_Rect startRect;
SDL_Rect listOfSquare[NUM_SQUARE];
SDL_Rect posiVecMoney[NUM_PLAYER];
SDL_Rect aniSprite[NUM_PLAYER][NUM_PLAYER][ANIMATION_FRAMES];
struct square *listOfLogicSquare[NUM_SQUARE];
struct chanceCard *listOfChances[NUM_OF_CHANCE];

//square processing
void squareProcessing(int id, Square *currentSquare, int dice);
void goToUCL(int id);
void department(int id);
void station(int id);
void chance(int id);
void ucl(int id);
void freePizza(int id);
void tax(int id);
void utility(int id, int distacne);


//animation
void loadIndCha(int id);
bool animatedRowOne(int start, int end, int i);
bool animatedColOne(int start, int end, int i);
bool animatedRowTwo(int start, int end, int i);
bool animatedColTwo(int start, int end, int i);

void aniRenderPlayer(int i,int direction, int frame, int x, int y);
void renderAllPlayer();
void initAnimation();


void buy(int id);

void isError(void *);
void setRect(SDL_Rect *, int, int , int, int);
int mimicDice();

void lostMoney(int id, int amount);
void gainMoney(int id, int amount);

void renderChance(int index);
void globalRender();

void init();
void loadBoard();
void loadCharacter();
SDL_Texture *loadMedia(char *path);
void initPlayer();
void initSquaresHelp();
void initSquares();
void initChances();

void renderPlayer(int number);
void animation(int previousBlock, int id);
void collisionDetection(int id);
void renderMoney();
SDL_Texture *createFromText(char *);

//marking the estate


void close();





















void SetupPriceAndFine(int i, int price, int fine);
void SetupDepartment(int i, int price, int fine);
void SetupUtility(int i, int price, int fine);
void SetupStation(int i, int price, int fine);

void initBoard(void);
void initBuyable(void);
void initUtility(void);
Square *createUtility(void);
void initStations(void);
Square *createStation(void);
void initDepartments(void);
Square *createSquare(void);
void initBrown(void);
void initSkyBlue(void);
void initPink(void);
void initOrange(void);
void initRed(void);
void initYellow(void);
void initGreen(void);
void initBlue(void);
void initNotBought(void);
void initPizza(void);
void initSuperTax(void);
void initIncomeTax(void);
void initChance(void);
Square *createChance(void);
void initGO(void);
void initUCL(void);
void initGoToUCL(void);
Square *initNonBuyable(void);
void deleteBoard(void);
void isInitSquare(Square *square);
char *push_string(char *current, const char *append);

#endif //FINALTRIAL_MAIN_H
