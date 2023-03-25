#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/poker.h"

int main(void) {

    userID userId;

    game poker;
    users player;
    users dealer;

    char string[SIZE];

    int bet;
    int round = 1;

    virtualTerminal();
    srand((unsigned int)time(NULL));

    poker.gold = 10000;

    while (round <= finish) {

        setValue(&poker, &player, &dealer);

        printf("\n%d回戦目のゲームを開始します。", round);
        printf("\n\nプレーヤーの所持金は%dGです。", poker.gold);
        printf("\n掛け金を入力してください。");

        while (1) {
            scanf("%d", &bet);
            if (bet > 0 && bet < 10000000) {
                break;
            }
            printf("有効な値ではありません。");
        }

        printf("\nカードを配ります。");
        dealCards(&poker, &player);
        dealCards(&poker, &dealer);

        userId = YOU;
        userTurn(&poker, &player, &userId);
        userId = DEALDER;
        userTurn(&poker, &dealer, &userId);

        printf("\n\n勝負です。\n");
        judgeResult(&poker, &player, &dealer);

        userId = YOU;
        printCards(&poker, &player, &userId);
        printHand(&player);
        userId = DEALDER;
        printCards(&poker, &dealer, &userId);
        printHand(&dealer);

        calculateBet(&poker, &bet);

        if (round < finish) {
            printf("\n\n次の試合に進みます。");
            printf("\n何かキーを押してください。");
            scanf("%1s%*[^\n]", string); 
        }
        round++;
    }
    printf("\n\n現在の所持金は%dGです。\n", poker.gold);

    return 0;
}