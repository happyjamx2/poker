#ifndef INCLUDE_POKER
#define INCLUDE_POKER

#define SIZE 16
#define HAND 5
#define CARD 53
#define SUIT 5
#define RANK 15

typedef enum UserID {
    YOU, DEALDER
} userID;

typedef enum Color {
    SPADE, HEART, CLUB, DIAMOND, STRONG, PALE
} color;

typedef enum Hand {
    ROYALFLUSH, STRAIGHTFLUSH, FOUR, FULLHOUSE, FLUSH, STRAIGHT, THREE, TWO, ONE, HIGH
} hand;

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
    int gFlag[HAND];
    int gold;
    status judge;
    fight result;
} game;

typedef struct Users {
    int uFlag[HAND];   //  役の判定・表示用配列
    int uCard[HAND];  //  カードの通し番号保存用配列
    int uMark[HAND];  //  カードのスート保存用配列
    int uRank[HAND];  //  カードのランク保存用配列
    hand uHand;    //  役の番号
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
void printCard(game *poker, users *user, int index);
void printCards(game *poker, users *user, userID *userId);
void convertInput(char *input, command *output);
void playerTurn(game *poker, users *player);
void dealerTurn(game *poker, users *dealer);
void sortCardId(users *user);
void judgeHand(users *user);
void userTurn(game *poker, users *user, userID *userId);
void printHand(users *user);
void judgeResult(game *poker, users *player, users *dealer);
void calculateBet(game *poker, int *bet);

extern const char suit[SUIT];
extern const char rank[RANK];
extern const rgb RGB[SUIT + 1];
extern const int finish;

#endif//INCLUDE_POKER