#include<myxwin.h>

char *cmask(int r)
{
    int i,j;
    int r2 = r*2;
    int rd = r*r;
    int n = 8;
    int w = (r2 + (n-1))/8;
    char *ret = (char*)calloc(1,r2*w);
    for(i=0;i<r2;i++){

        int m = i*w;
        int l1 = (i-r);
        l1 *= l1;
        
        for(j=0;j<r2;j++){
        
            int l2 = (j-r);
            l2 *= l2;
        
            if(l1 + l2 <= rd){
                ret[m + j/n] |= 1<<(j%8);
            }
        }
    }
    return ret;
}
#define flag printf("flag\n");
void mainProcess(window_t window)
{
    int delay,size;
    char *mask;
    framedata_t frame;
    sprite_t ball;
    Pixmap pm;

    frame.current_frame = 0;
    frame.refrate = 100;
    frame.counter = 0;
    frame.frame = 0;
    frame.time = 0;   
    delay = 1;
    size = (int)window.width;
    ball.width = window.width;
    ball.height = window.height;
    ball.x = 10;
    ball.y = 10;
    ball.xlimit = DisplayWidth(window.display,window.screen);
    ball.ylimit = DisplayHeight(window.display,window.screen);
    ball.vx = 3.3;
    ball.vy = 2.1;
    mask = cmask(size/2);
    
    XMoveWindow(window.display,window.window,ball.x,ball.y);

    pm = XCreateBitmapFromData(window.display,
            RootWindow(window.display,window.screen),
            mask,
            size,size);
    free(mask);
    XShapeCombineMask(window.display,
            window.window,
            ShapeBounding,
            0,0,
            pm,
            ShapeSet);
    
    XFlush(window.display);
    XMapWindow(window.display,window.window);

    while(1){
        if(frame.counter==frame.refrate){
            ObjectBounce(&ball);
            XMoveWindow(window.display,window.window,ball.x,ball.y);
            printf("\rx = %f, y = %f : vx = %f, vy = %f   ",ball.x,ball.y,ball.vx,ball.vy);
            XFlush(window.display);
        }
        (frame.counter == frame.refrate) ? frame.counter = 0 : 0;
        frame.counter++;
        usleep(delay);
    }
    DestWindow(&window);
}

int main()
{ 
    int size = 20;
    char mode[] = "dock";
    window_t window;

    window.mode = mode;    
    window.resize = True;
    window.width = size;
    window.height = size;

    initWindow(&window,10);
    mainProcess(window);
    DestWindow(&window);
}
