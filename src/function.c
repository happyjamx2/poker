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
    poker->judge = NOTYET;
    poker->result = DRAW;

    for (int i = 0; i < HAND; i++) {
        player->pocket[i] = 0;
        player->mark[i] = 0;
        player->value[i] = 0;
        player->record[i] = 0;
    }
    player->hand = 0;

    for (int i = 0; i < HAND; i++) {
        dealer->pocket[i] = 0;
        dealer->mark[i] = 0;
        dealer->value[i] = 0;
        dealer->record[i] = 0;
    }
    dealer->hand = 0;
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
        user->mark[i] = (user->pocket[i] - 1) % 4;
        user->record[i] = user->mark[i];
    }
    for (int i = 0; i < HAND; i++) {
        if (user->pocket[i] <= 48) {
            user->value[i] = (user->pocket[i] + 7) / 4;
        } else {
            user->value[i] = 1;
        }
    }
}

//  手札を配る
void dealCards( game *poker, users *user ) {

    int cardId;

    for (int i = 0; i < HAND; i++) {
        cardId = generateNumber(poker);
        user->pocket[i] = cardId;
    }
    convertNumber(user);
}

//  カードの表示
void printCard( users *user, int index ) {

    changeColor(&user->record[index]);
    if (user->value[index] == 10) {
        printf("%c", suit[user->mark[index]]);
        printf("%d", user->value[index]);
    } else {
        printf(" ");
        printf("%c", suit[user->mark[index]]);
        printf("%c", rank[user->value[index]]);
    }
    resetColor();
}

//  手札の表示
void printCards( game *poker, users *self, users *opponent ) {

    printf("\n%sの手札:", self->name);
    for (int i = 0; i < HAND; i++) {
        if (self->value[i] == 10) {
            printf(" ");
        }
        if (poker->judge) {
            if (self->value[i] != 10 && opponent->value[i] == 10) {
                printf(" ");
            }
        }
        printCard(self, i);
    }
}

//  入力を変換する
void convertInput( char *input, command *output ) {

    int column = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        column++;
    }
    switch (column) {
        case 1:
            if (input[0] == 'y') {
                *output = YES;
            } else if (input[0] == 'n') {
                *output = NO;
            } else {
                *output = INVALID;
            }
            break;
        default:
            *output = INVALID;
            break;
    }
}

//  カードを引く
void drawCard( game *poker, users *user, int index ) {

    int cardId;

    cardId = generateNumber(poker);
    user->pocket[index] = cardId;    
}

//  通し番号を昇順にソート
void sortCardId( users *user ) {

    int min;

    for (int i = 0; i < HAND - 1; i++) {
        min = user->pocket[i];
        for (int j = i + 1; j < HAND; j++) {
            if (min > user->pocket[j]) {
                min = user->pocket[j];
                user->pocket[j] = user->pocket[i];
                user->pocket[i] = min;
            }
        }
    }
    convertNumber(user);
}

//  役の判定
void judgeHand( users *user ) {

    int sameSuit = 1;

    sortCardId(user);
    for (int i = 0; i < HAND - 1; i++) {
        if (user->mark[i] != user->mark[i + 1]) {
            sameSuit = 0;
            break;
        }
    }

    if (sameSuit) {
        for (int i = 0; i < HAND; i++) {
            user->record[i] = LIGHT;
        }
        user->hand = STRAIGHTFLUSH;
        for (int i = HAND; i > 0; i--) {
            if (user->value[0] == 10 && user->value[1] == 11 &&\
                user->value[2] == 12 && user->value[3] == 13 &&\
                user->value[4] == 1) {
                user->hand = ROYALFLUSH;
                break;
            } else if (user->value[0] == 2 && user->value[1] == 3 &&\
                       user->value[2] == 4 && user->value[3] == 5 &&\
                       user->value[4] == 1) {
                break;
            } else if (user->value[i] - user->value[i - 1] != 1) {
                user->hand = FLUSH;
                break;
            }
        }
    } else {
    
        int match = 0;

        for (int i = 0; i < HAND; i++) {
            user->record[i] = DARK;
        }
        for (int i = 0; i < HAND - 1; i++) {
            for (int j = i + 1; j < HAND; j++) {
                if (user->value[i] == user->value[j]) {
                    user->record[i] = LIGHT;
                    user->record[j] = LIGHT;
                    match++;
                }
            }
        }
        switch (match) {
            case 6:
                user->hand = FOUR;
                break;
            case 4:
                user->hand = FULLHOUSE;
                break;
            case 3:
                user->hand = THREE;
                break;
            case 2:
                user->hand = TWO;
                break;
            case 1:
                user->hand = ONE;
                break;
            default:
                user->hand = STRAIGHT;
                for (int i = HAND - 1; i > 0; i--) {
                    if (user->value[0] == 10 && user->value[1] == 11 &&\
                        user->value[2] == 12 && user->value[3] == 13 &&\
                        user->value[4] == 1) {                        
                        break;
                    } else if (user->value[0] == 2 && user->value[1] == 3 &&\
                               user->value[2] == 4 && user->value[3] == 5 &&\
                               user->value[4] == 1) {
                        break;
                    } else if (user->value[i] - user->value[i - 1] != 1) {
                        user->hand = HIGH;
                        break;
                    }
                }
                if (user->hand == STRAIGHT) {
                    for (int i = 0; i < HAND; i++) {
                        user->record[i] = LIGHT;
                    }
                }
                break;
        }
    }
}

//  プレーヤー
void playerTurn( game *poker, users *player, users *dealer ) {

    char input[SIZE];
    command output;

    printf("\n\n---%sのターン---\n", player->name);
    printCards(poker, player, dealer);
    printf("\n\n");
    for (int i = 0; i < HAND; i++) {
        printCard(player, i);
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
            drawCard(poker, player, i);
        }
    }
    convertNumber(player);
    printCards(poker, player, dealer);
    judgeHand(player);
}

//  ディーラー
void dealerTurn( game *poker, users *dealer ) {

    printf("\n\n---%sのターン---\n", dealer->name);
    judgeHand(dealer);
    switch (dealer->hand) {
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
                if (dealer->record[i] == DARK) {
                    printf("...\n");
                    drawCard(poker, dealer, i);
                }
            }
            break;
        case HIGH:
        {
            int count[4] = {0};
            int maxSuit = -1;

            for (int i = 0; i < HAND; i++) {
                count[dealer->mark[i]]++;
            }
            for (int i = 0; i < SUIT - 1; i++) {
                if (count[i] >= HAND - 1) {
                    maxSuit = i;
                    break;
                }
            }
            if (maxSuit == -1) {
                for (int i = 0; i < HAND - 1; i++) {
                    printf("...\n");
                    drawCard(poker, dealer, i);
                }
            } else {
                for (int i = 0; i < HAND; i++) {
                    if (dealer->mark[i] != maxSuit) {
                        printf("...\n");
                        drawCard(poker, dealer, i);
                    }
                }
            }
        }
            break;
        default:
            break;
    }
    printf("\n交換が終了しました。");
    judgeHand(dealer);
}

//  役の表示
void printHand( users *user ) {

    switch (user->hand) {
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
 
    if (player->hand > dealer->hand) {
        poker->result = LOSE;
    } else if (player->hand < dealer->hand) {
        poker->result = WIN;
    } else {
        switch (player->hand) {
            case ROYALFLUSH:
                poker->result = DRAW;
                break;
            case STRAIGHTFLUSH:
            case STRAIGHT:
                if (player->value[4] == dealer->value[4]) {
                    poker->result = DRAW;
                } else if (player->pocket[4] > dealer->pocket[4]) {
                    poker->result = WIN;
                } else if (player->pocket[4] < dealer->pocket[4]) {
                    poker->result = LOSE;
                }
                break;
            case FOUR:
            case FULLHOUSE:
            case THREE:
                if (player->pocket[2] > dealer->pocket[2]) {
                    poker->result = WIN;
                } else if (player->pocket[2] < dealer->pocket[2]) {
                    poker->result = LOSE;
                }
                break;
            case FLUSH:
            case HIGH:
                for (int i = HAND - 1; i >= 0; i--) {
                    if (player->value[i] == dealer->value[i]) {
                        poker->result = DRAW;
                    } else if (player->pocket[i] > dealer->pocket[i]) {
                        poker->result = WIN;
                        break;
                    } else if (player->pocket[i] < dealer->pocket[i]) {
                        poker->result = LOSE;
                        break;
                    }
                }
                break;
            case TWO:
            {
                int playerIndex;
                int dealerIndex;

                if (player->value[3] == dealer->value[3]) {
                    if (player->value[1] == dealer->value[1]) {
                        for (int i = HAND - 1; i >= 0; i --) {
                            if (player->record[i] == DARK) {
                                playerIndex = i;
                                break;
                            }
                        }
                        for (int i = HAND - 1; i >= 0; i --) {
                            if (dealer->record[i] == DARK) {
                                dealerIndex = i;
                                break;
                            }
                        }
                        if (player->value[playerIndex] == dealer->value[dealerIndex]) {
                            poker->result = DRAW;
                        } else if (player->pocket[playerIndex] > dealer->pocket[dealerIndex]) {
                            poker->result = WIN;
                        } else if (player->pocket[playerIndex] < dealer->pocket[dealerIndex]) {
                            poker->result = LOSE;
                        }
                    } else if (player->pocket[1] > dealer->pocket[1]) {
                        poker->result = WIN;
                    } else if (player->pocket[1] < dealer->pocket[1]) {
                        poker->result = LOSE;
                    } 
                } else if (player->pocket[3] > dealer->pocket[3]) {
                    poker->result = WIN;
                } else if (player->pocket[3] < dealer->pocket[3]) {
                    poker->result = LOSE;
                }
            }
                break;
            case ONE:
            {
                int playerIndex = -1;
                int dealerIndex = -1;
                int playerPair;
                int dealerPair;

                for (int i = HAND - 1; i >= 0; i--) {
                    if (player->record[i] == LIGHT) {
                        playerPair = i;
                        break;
                    }
                }
                for (int i = HAND - 1; i >= 0; i--) {
                    if (dealer->record[i] == LIGHT) {
                        dealerPair = i;
                        break;
                    }
                }
                if (player->value[playerPair] == dealer->value[dealerPair]) {
                    poker->result = DRAW;
                    for (int i = 0; i < HAND - 2; i++) {
                        for (int j = HAND - 1; j >= 0; j--) {
                            if (player->record[j] == DARK && playerIndex != j) {
                                playerIndex = j;
                                break;
                            }
                        }
                        for (int j = HAND - 1; j >= 0; j--) {
                            if (dealer->record[j] == DARK && dealerIndex != j) {
                                dealerIndex = j;
                                break;
                            }
                        }
                        if (player->value[playerIndex] == dealer->value[dealerIndex]) {
                        } else if (player->pocket[playerIndex] > dealer->pocket[dealerIndex]) {
                            poker->result = WIN;
                            break;
                        } else if (player->pocket[playerIndex] < dealer->pocket[dealerIndex]) {
                            poker->result = LOSE;
                            break;
                        }
                    }

                } else if (player->pocket[playerPair] > dealer->pocket[dealerPair]) {
                    poker->result = WIN;
                } else if (player->pocket[playerPair] < dealer->pocket[dealerPair]) {
                    poker->result = LOSE;
                }
            }
                break;
            default:
                break;
        }
    }
    poker->judge = DONE;
}

//  掛け金の計算
void calculateBet( game *poker, users *player, int *bet ) {

    switch (poker->result) {
        case WIN:
            printf("\n\n%sの勝ちです。+", player->name);
            break;
        case DRAW:
            *bet = 0;
            printf("\n\n引き分けです。±");
            break;
        case LOSE:
            *bet *= -1;
            printf("\n\n%sの負けです。", player->name);
            break;
        default:
            break;
    }
    printf("%dG", *bet);
    poker->gold += *bet;
}
