#!/bin/sh
NAME="winball2"
EXNAME="xball"
#gcc -Wall -g -O3 $NAME.c  -o $EXNAME  -lX11 -lXext -lm -I./
gcc -Wall -g -O3 $NAME.c  -o $EXNAME ./include/*.c -lX11 -lXext -lm -I./
#gcc -Wall -g -O3 $NAME.c  -o $EXNAME -L /usr/X11/lib -I /usr/X11/include -lX11 -lm
./$EXNAME
