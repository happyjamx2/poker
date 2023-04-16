#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "../include/poker.h"

const char suit[SUIT] = "SHCD";
const char rank[RANK] = "0A234567891JQK";

const rgb RGB[6] = { {  30,  30, 250 },\
                     { 250,  50, 200 },\
                     {   0, 200,  20 },\
                     { 240, 220,  50 },\
                     { 250, 250, 250 },\
                     { 100, 100, 100 } };

const int finish = 3;  //  最大ゲーム数