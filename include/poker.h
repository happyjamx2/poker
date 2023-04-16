#ifndef INCLUDE_POKER
#define INCLUDE_POKER

#define CARD 53
#define HAND 5
#define SIZE 16
#define SUIT 5
#define RANK 15

typedef enum UserID {
    YOU, DEALDER
} userID;

typedef enum Color {
    SPADE, HEART, CLUB, DIAMOND, LIGHT, DARK
} color;

typedef enum Hands {
    ROYALFLUSH, STRAIGHTFLUSH, FOUR, FULLHOUSE, FLUSH, STRAIGHT, THREE, TWO, ONE, HIGH
} hands;

typedef enum Command {
    INVALID, YES, NO
} command;

typedef enum Fight {
    WIN, DRAW, LOSE
} fight;

typedef enum Status {
    NOTYET, DONE
} status;

typedef struct Game {
    int card[CARD];
    int gold;
    status judge;
    fight result;
} game;

typedef struct Users {
    char* name;
    int pocket[HAND];    //  カードの通し番号保存用配列
    int mark[HAND];      //  カードのスート保存用配列
    int value[HAND];     //  カードのランク保存用配列
    color record[HAND];  //  役の判定・表示用配列
    hands hand;          //  役の番号
} users;

typedef struct Rgb {
    int r;
    int g;
    int b;
} rgb;

void virtualTerminal(void);
void escapeSequence(char const *string, char const character);
void changeColor(color *code);
void resetColor(void);
void setValue(game *poker, users *player, users *dealer);
int generateNumber(game *poker);
void convertNumber(users *user);
void dealCards(game *poker, users *user);
void printCard(users *user, int index);
void printCards(game *poker, users *self, users *opponent);
void convertInput(char *input, command *output);
void drawCard(game *poker, users *user, int index);
void sortCardId(users *user);
void judgeHand(users *user);
void playerTurn(game *poker, users *player, users *dealer);
void dealerTurn(game *poker, users *dealer);
void printHand(users *user);
void judgeResult(game *poker, users *player, users *dealer);
void calculateBet(game *poker, users *player, int *bet);

extern const char suit[SUIT];
extern const char rank[RANK];
extern const rgb RGB[6];
extern const int finish;

#endif//INCLUDE_POKER