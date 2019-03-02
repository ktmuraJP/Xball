#!/bin/sh
NAME="winball2"
EXNAME="xball"
gcc -Wall -g -O3 $NAME.c  -o $EXNAME ./include/*.c -lX11 -lXext -lm -I./
./$EXNAME
