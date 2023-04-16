#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/poker.h"

int main(void) {

    game poker;
    users player;
    users dealer;

    char string[SIZE];
    int bet;
    int round = 1;

    virtualTerminal();
    srand((unsigned int)time(NULL));

    player.name = "プレイヤー";
    dealer.name = "ディーラー";
    poker.gold = 10000;

    while (round <= finish) {

        setValue(&poker, &player, &dealer);

        printf("\n%d回戦目のゲームを開始します。", round);
        printf("\n\n%sの所持金は%dGです。", player.name, poker.gold);
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

        playerTurn(&poker, &player, &dealer);
        dealerTurn(&poker, &dealer);

        printf("\n\n勝負です。\n");
        judgeResult(&poker, &player, &dealer);

        printCards(&poker, &player, &dealer);
        printHand(&player);
        printCards(&poker, &dealer, &player);
        printHand(&dealer);

        calculateBet(&poker, &player, &bet);

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