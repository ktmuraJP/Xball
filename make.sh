#!/bin/sh
NAME="winball"
EXNAME="ball"
gcc -Wall -g -O3 $NAME.c  -o $EXNAME -lX11 -lXext -lm
#gcc -Wall -g -O3 $NAME.c  -o $EXNAME -L /usr/X11/lib -I /usr/X11/include -lX11 -lm
./$EXNAME
