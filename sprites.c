#include"myxwin.h"

void keyControl(char *keymap, sprite_t *sprite)
{
    int up, down,left,right;
    int x, y;
    int xlim, ylim;

    x = sprite->x;
    y = sprite->y;
    xlim = sprite->xlimit;
    ylim = sprite->ylimit;

    up = keymap[UP_INDX];
    down = keymap[DOWN_INDX];
    left = keymap[LEFT_INDX];
    right = keymap[RIGHT_INDX];
   
    if( (left == LEFT   ||  left == LEFT+DOWN)
        &&
        x > 0 ){


    }
    if( up == UP
        &&
        y > 0 ){
        

    }
    if( (right == RIGHT  ||  right == RIGHT+DOWN)
        &&
        x < xlim){


    }
    if( (down == DOWN   ||  down == DOWN+LEFT   || down == DOWN+RIGHT)
        &&
        y < ylim){


    }
}

void ObjectBounce(sprite_t *sp)
{
    float dec;
    dec = 0;
    if(sp->x >= (sp->xlimit - sp->width)||
            sp->x <= 0){
        (sp->vx <= 0) ? sp->vx += dec : 0;
        (sp->vx >= 0) ? sp->vx -= dec : 0;
        sp->vx*=-1;

    }
     if(sp->y >= (sp->ylimit - sp->height)||
            sp->y <= 0){
        (sp->vy <= 0) ? sp->vy += dec : 0;
        (sp->vy >= 0) ? sp->vy -= dec : 0;
        sp->vy*=-1;
    }
    sp->x += sp->vx;
    sp->y += sp->vy;
}
