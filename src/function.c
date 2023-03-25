#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/poker.h"

//  Virtual Terminal機能を有効にする
void virtualTerminal( void ) {

    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;
    GetConsoleMode(stdOut, &consoleMode);
    SetConsoleMode(stdOut, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

//  エスケープシーケンスを標準出力する
void escapeSequence( char const *string, char const character ) {

    printf("\x1b[");
    printf(string);
    putchar(character);
}

//  RGBを指定して文字の色を変更する
void changeColor( color *code ) {

    char buf[64];

	snprintf(buf, 64, "38;2;%d;%d;%d", RGB[*code].r, RGB[*code].g, RGB[*code].b);
    escapeSequence(buf, 'm');
}

//  色情報をリセットする
void resetColor( void ) {

    escapeSequence("0", 'm');
}

//  値をセットする
void setValue( game *poker, users *player, users *dealer ) {

    for (int i = 0; i < CARD; i++) {
        poker->card[i] = 0;
    }
    for (int i = 0; i < HAND; i++) {
        poker->gFlag[i] = 0;
    }
    poker->judge = NOTYET;

    for (int i = 0; i < HAND; i++) {
        player->uFlag[i] = 0;
        player->uCard[i] = 0;
        player->uMark[i] = 0;
        player->uRank[i] = 0;
    }

    for (int i = 0; i < HAND; i++) {
        dealer->uFlag[i] = 0;
        dealer->uCard[i] = 0;
        dealer->uMark[i] = 0;
        dealer->uRank[i] = 0;
    }
}

//  1~52の番号をランダムに生成する
int generateNumber( game *poker ) {

    int N;

    do {
        N = rand() % 52 + 1;    
    } while (poker->card[N]);
    poker->card[N]++;

    return N;
}

//  カードの通し番号をスート、ランクへ変換する
void convertNumber( users *user ) {

    for (int i = 0; i < HAND; i++) {
        user->uMark[i] = (user->uCard[i] - 1) % 4;
        if (user->uCard[i] <= 48) {
            user->uRank[i] = (user->uCard[i] + 7) / 4;
        } else {
            user->uRank[i] = 1;
        }
    }
}

//  手札を配る
void dealCards( game *poker, users *user ) {

    int cardId;

    for (int i = 0; i < HAND; i++) {
        cardId = generateNumber(poker);
        user->uCard[i] = cardId;
    }

    /*  任意の値入力用
    player->uCard[0] = 3;
    player->uCard[1] = 17;
    player->uCard[2] = 18;
    player->uCard[3] = 5;
    player->uCard[4] = 36;

    dealer->uCard[0] = 3;
    dealer->uCard[1] = 17;
    dealer->uCard[2] = 18;
    dealer->uCard[3] = 5;
    dealer->uCard[4] = 36;
    */
}

//  カードの表示
void printCard( game *poker, users *user, int index ) {

    color code;

    if (poker->judge == DONE) {
        if (user->uFlag[index]) {
            code = STRONG;
        } else {
            code = PALE;
        }
    } else {
        code = user->uMark[index];
    }
    changeColor(&code);

    if (user->uRank[index] == 10) {
        printf("%c", suit[user->uMark[index]]);
        printf("%d", user->uRank[index]);
    } else {
        printf(" ");
        printf("%c", suit[user->uMark[index]]);
        printf("%c", rank[user->uRank[index]]);
    }
    resetColor();
}

//  手札の表示
void printCards( game *poker, users *user, userID *userId ) {

    convertNumber(user);

    if (0) {
    } else if (*userId == YOU) {
        printf("\nプレーヤーの手札:");
    } else if (*userId == DEALDER) {
        printf("\nディーラーの手札:");
    }
    for (int i = 0; i < HAND; i++) {
        if (user->uRank[i] == 10) {
            printf(" ");
        }
        if (poker->gFlag[i] && user->uRank[i] != 10) {
            printf(" ");
        }
        printCard(poker, user, i);
    }
}

//  入力を変換する
void convertInput( char *input, command *output ) {

    int column = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        column++;
    }
    if (column == 1) {
        if (input[0] == 'y') {
            *output = YES;
        } else if (input[0] == 'n') {
            *output = NO;
        } else {
            *output = INVALID;
        }
    } else {
        *output = INVALID;
    }
}

//  プレーヤー
void playerTurn( game *poker, users *player ) {

    char input[SIZE];
    command output;

    int cardId;

    printf("\n\n");
    for (int i = 0; i < HAND; i++) {
        printCard(poker, player, i);
        printf("を交換しますか? (Y/N) ");
        while (1) {
            scanf("%2s%*[^\n]", input);
            getchar();
            convertInput(input, &output);
            if (output == YES || output == NO) {
                break;
            }
            printf("有効な値ではありません。");
        }
        if (output == YES) {
            cardId = generateNumber(poker);
            player->uCard[i] = cardId;
        }
    }
}

//  ディーラー
void dealerTurn( game *poker, users *dealer ) {

    int count[4] = {0};

    int cardId;
    int flag;

    sortCardId(dealer);
    judgeHand(dealer);

    switch (dealer->uHand) {
        case ROYALFLUSH:
        case STRAIGHTFLUSH:
        case FOUR:
        case FULLHOUSE:
        case FLUSH:
        case STRAIGHT:
            printf("...\n");
            break;
        case THREE:
        case TWO:
        case ONE:
            for (int i = 0; i < HAND; i++) {
                if (!dealer->uFlag[i]) {
                    printf("...\n");
                    cardId = generateNumber(poker);
                    dealer->uCard[i] = cardId;
                }
            }
            break;
        case HIGH:
            for (int i = 0; i < HAND; i++) {
                count[dealer->uMark[i]]++;
            }
            for (int i = 0; i < SUIT - 1; i++) {
                if (count[i] >= HAND - 1) {
                    flag = i;
                    break;
                }
                flag = -1;
            }
            if (flag == -1) {
                for (int i = 0; i < HAND - 1; i++) {
                    printf("...\n");
                    cardId = generateNumber(poker);
                    dealer->uCard[i] = cardId;
                }
            } else {
                for (int i = 0; i < HAND; i++) {
                    if (dealer->uMark[i] != flag) {
                        printf("...\n");
                        cardId = generateNumber(poker);
                        dealer->uCard[i] = cardId;
                    }
                }
            }
            break;
        default:
            break;
    }
    printf("\n交換が終了しました。");
}

//  通し番号を昇順にソート
void sortCardId( users *user ) {

    int min;

    for (int i = 0; i < HAND - 1; i++) {
        min = user->uCard[i];
        for (int j = i + 1; j < HAND; j++) {
            if (min > user->uCard[j]) {
                min = user->uCard[j];
                user->uCard[j] = user->uCard[i];
                user->uCard[i] = min;
            }
        }
    }
    convertNumber(user);
}

//  役の判定
void judgeHand( users *user ) {

    int match = 0;
    int samesuit;

    for (int i = 0; i < HAND - 1; i++) {
        if (user->uMark[i] == user->uMark[i + 1]) {
            samesuit = 1;
        } else {
            samesuit = 0;
            break;
        }
    }

    if (samesuit) {
        for (int i = 0; i < HAND; i++) {
            user->uFlag[i] = 1;
        }
        for (int i = HAND; i > 0; i--) {
            if (user->uRank[0] == 10 && user->uRank[1] == 11 &&\
                user->uRank[2] == 12 && user->uRank[3] == 13 &&\
                user->uRank[4] == 1)
            {
                user->uHand = ROYALFLUSH;
                break;
            } else if (user->uRank[0] == 2 && user->uRank[1] == 3 &&\
                       user->uRank[2] == 4 && user->uRank[3] == 5 &&\
                       user->uRank[4] == 1)
            {
                user->uHand = STRAIGHTFLUSH;
                break;
            } else if (user->uRank[i] - user->uRank[i - 1] == 1) {
                user->uHand = STRAIGHTFLUSH;
            } else {
                user->uHand = FLUSH;
                break;
            }
        }
    } else {   
        for (int i = 0; i < HAND; i++) {
            user->uFlag[i] = 0;
        }
        for (int i = 0; i < HAND - 1; i++) {
            for (int j = i + 1; j < HAND; j++) {
                if (user->uRank[i] == user->uRank[j]) {
                    user->uFlag[i] = 1;
                    user->uFlag[j] = 1;
                    match++;
                }
            }
        }
        switch (match) {
            case 6:
                user->uHand = FOUR;
                break;
            case 4:
                user->uHand = FULLHOUSE;
                break;
            case 3:
                user->uHand = THREE;
                break;
            case 2:
                user->uHand = TWO;
                break;
            case 1:
                user->uHand = ONE;
                break;
            default:
                for (int i = 0; i < HAND; i++) {
                    user->uFlag[i] = 1;
                }
                for (int i = HAND - 1; i > 0; i--) {
                    if (user->uRank[0] == 10 && user->uRank[1] == 11 &&\
                        user->uRank[2] == 12 && user->uRank[3] == 13 &&\
                        user->uRank[4] == 1)
                    {
                        user->uHand = STRAIGHT;
                        break;
                    } else if (user->uRank[0] == 2 && user->uRank[1] == 3 &&\
                               user->uRank[2] == 4 && user->uRank[3] == 5 &&\
                               user->uRank[4] == 1)
                    {
                        user->uHand = STRAIGHT;
                        break;
                    } else if (user->uRank[i] - user->uRank[i - 1] == 1) {
                        user->uHand = STRAIGHT;
                    } else {
                        user->uHand = HIGH;
                        for (int i = 0; i < HAND; i++) {
                            user->uFlag[i] = 0;
                        }
                        break;
                    }
                }
                break;
        }
    }
}

//  ユーザーのターン
void userTurn( game *poker, users *user, userID *userId ) {

    if (0) {
    } else if (*userId == YOU) {
        printf("\n\n---プレーヤーのターン---\n");
        printCards(poker, user, userId);
        playerTurn(poker, user);
        printCards(poker, user, userId);
    } else if (*userId == DEALDER) {
        printf("\n\n---ディーラーのターン---\n");
        dealerTurn(poker, user);
    }
    sortCardId(user);
    for (int i = 0; i < HAND; i++) {
        if (user->uRank[i] == 10) {
            poker->gFlag[i] = 1;
        }
    }
    judgeHand(user);
}

//  役の表示
void printHand( users *user ) {

    switch (user->uHand) {
        case ROYALFLUSH:
            printf("  ロイヤルストレートフラッシュ");
            break;
        case STRAIGHTFLUSH:
            printf("  ストレートフラッシュ");
            break;
        case FOUR:
            printf("  フォーカード");
            break;
        case FULLHOUSE:
            printf("  フルハウス");
            break;
        case FLUSH:
            printf("  フラッシュ");
            break;
        case STRAIGHT:
            printf("  ストレート");
            break;
        case THREE:
            printf("  スリーカード");
            break;
        case TWO:
            printf("  ツーペア");
            break;
        case ONE:
            printf("  ワンペア");
            break;
        case HIGH:
            printf("  ハイカード");
            break;
        default:
            break;
    }
}

//  勝敗の判定
void judgeResult( game *poker, users *player, users *dealer ) {

    int max_p;
    int max_d;
    int pair_p;
    int pair_d;
 
    if (player->uHand > dealer->uHand) {
        poker->result = LOSE;
    } else if (player->uHand < dealer->uHand) {
        poker->result = WIN;
    } else {
        switch (player->uHand) {
            case ROYALFLUSH:
                poker->result = DRAW;
                break;
            case STRAIGHTFLUSH:
            case STRAIGHT:
                if (player->uRank[4] == dealer->uRank[4]) {
                    poker->result = DRAW;
                } else if (player->uCard[4] > dealer->uCard[4]) {
                    poker->result = WIN;
                } else if (player->uCard[4] < dealer->uCard[4]) {
                    poker->result = LOSE;
                }
                break;
            case FOUR:
            case FULLHOUSE:
            case THREE:
                if (player->uCard[2] > dealer->uCard[2]) {
                    poker->result = WIN;
                } else if (player->uCard[2] < dealer->uCard[2]) {
                    poker->result = LOSE;
                }
                break;
            case FLUSH:
            case HIGH:
                for (int i = HAND - 1; i >= 0; i--) {
                    if (player->uRank[i] == dealer->uRank[i]) {
                        poker->result = DRAW;
                    } else if (player->uCard[i] > dealer->uCard[i]) {
                        poker->result = WIN;
                        break;
                    } else if (player->uCard[i] < dealer->uCard[i]) {
                        poker->result = LOSE;
                        break;
                    }
                }
                break;
            case TWO:
                if (player->uRank[3] == dealer->uRank[3]) {
                    if (player->uRank[1] == dealer->uRank[1]) {
                        for (int i = HAND - 1; i >= 0; i --) {
                            if (!player->uFlag[i]) {
                                max_p = i;
                                break;
                            }
                        }
                       for (int i = HAND - 1; i >= 0; i --) {
                            if (!dealer->uFlag[i]) {
                                max_d = i;
                                break;
                            }
                        }
                        if (player->uRank[max_p] == dealer->uRank[max_d]) {
                            poker->result = DRAW;
                        } else if (player->uCard[max_p] > dealer->uCard[max_d]) {
                            poker->result = WIN;
                        } else if (player->uCard[max_p] < dealer->uCard[max_d]) {
                            poker->result = LOSE;
                        }
                    } else if (player->uCard[1] > dealer->uCard[1]) {
                        poker->result = WIN;
                    } else if (player->uCard[1] < dealer->uCard[1]) {
                        poker->result = LOSE;
                    } 
                } else if (player->uCard[3] > dealer->uCard[3]) {
                    poker->result = WIN;
                } else if (player->uCard[3] < dealer->uCard[3]) {
                    poker->result = LOSE;
                }
                break;
            case ONE:
                for (int i = HAND - 1; i >= 0; i--) {
                    if (player->uFlag[i]) {
                        pair_p = i;
                        break;
                    }
                }
                for (int i = HAND - 1; i >= 0; i--) {
                    if (dealer->uFlag[i]) {
                        pair_d = i;
                        break;
                    }
                }
                if (player->uRank[pair_p] == dealer->uRank[pair_d]) {
                    for (int i = 0; i < HAND - 2; i++) {
                        for (int j = HAND - 1; j >= 0; j--) {
                            if (!player->uFlag[j]) {
                                max_p = j;
                                player->uFlag[j] = 1;
                                break;
                            }
                        }
                        for (int j = HAND - 1; j >= 0; j--) {
                            if (!dealer->uFlag[j]) {
                                max_d = j;
                                dealer->uFlag[j] = 1;
                                break;
                            }
                        }
                        if (player->uRank[max_p] == dealer->uRank[max_d]) {
                            poker->result = DRAW;
                        } else if (player->uCard[max_p] > dealer->uCard[max_d]) {
                            poker->result = WIN;
                            break;
                        } else if (player->uCard[max_p] < dealer->uCard[max_d]) {
                            poker->result = LOSE;
                            break;
                        }
                    }
                } else if (player->uCard[pair_p] > dealer->uCard[pair_d]) {
                    poker->result = WIN;
                } else if (player->uCard[pair_p] < dealer->uCard[pair_d]) {
                    poker->result = LOSE;
                }
                break;
            default:
                break;
        }
    }
    poker->judge = DONE;
}

//  掛け金の計算
void calculateBet( game *poker, int *bet ) {

    if (poker->result == WIN) {
        printf("\n\nプレーヤーの勝ちです。");
        printf("+%dG", *bet);
        poker->gold += *bet;
    } else if (poker->result == DRAW) {
        printf("\n\n引き分けです。");
        printf("±0G");
    } else if (poker->result == LOSE) {
        printf("\n\nプレーヤーの負けです。");
        printf("-%dG", *bet);
        poker->gold -= *bet;
    }
}