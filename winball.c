#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<X11/Xlib.h>
#include<X11/Xatom.h>
#include<X11/Xutil.h>
#include<X11/Xresource.h>
#include<X11/keysymdef.h>
#include<X11/XKBlib.h>
#include<X11/extensions/shape.h>
#include<stdarg.h>
#include<unistd.h>
#include<math.h>
#include<time.h>

#define WHITE 0xffffffffffff
#define SPRITES 10

#define VECT 0x0

#define WINDOW_SIZE_X 700
#define WINDOW_SIZE_Y 700
#define XLIM 700
#define YLIM 700

#define DEFAULT_KEY_DELAY 660
#define DEFAULT_KEY_RATE 25

#define KEY_DELAY 1
#define KEY_RATE 200

#define UP 0xffffff80
#define UP_INDX 13
#define DOWN 0x10
#define DOWN_INDX 14
#define LEFT 0x2
#define LEFT_INDX 14
#define RIGHT 0x4
#define RIGHT_INDX 14

#define AUTO 0x0
#define MANUAL 0x1
#define flag(x) printf("flag %s\n", x)

typedef struct _window_data
{
    Display *display;
    Window window;
    XEvent event;
    int screen;
    int size_x, size_y;
    char *mode;
    Bool resize;
    GC *gc; 
    Atom atom, delWindow;
}
window_t;

typedef struct _sprite_data
{
    int x, y;
    int xlimit, ylimit;

    //  velocity
    int vx,vy;
    int vxlimit, vylimit;
    
    int size_x, size_y;
    int center;
    short speed_x, speed_y;
    char *spr_name;
    GC gc;

}sprite_t;

void initKeyboard(window_t *window)
{
    XkbSetAutoRepeatRate(window->display, 
            XkbUseCoreKbd,
            KEY_DELAY,
            KEY_RATE);
}

void destKeyboard(window_t *window)
{
    XkbSetAutoRepeatRate(window->display, 
            XkbUseCoreKbd,
            DEFAULT_KEY_DELAY,
            DEFAULT_KEY_RATE);
}

void WindowSetting(window_t *window)
{
    XSizeHints *size_hint;
    Atom WindowAtom;//, ActionAtom;
    long winval;//, actval;
    const char *windowtype;
    windowtype = window->mode;
    
    if(window->resize == False){ 
        size_hint = XAllocSizeHints();
        size_hint->flags = PMinSize | PMaxSize;
        size_hint->min_width = window->size_x;
        size_hint->max_width = window->size_x;
        size_hint->min_height = window->size_y;
        size_hint->max_height = window->size_y;
        XSetWMNormalHints(window->display, window->window,size_hint);
    
    }else if(window->resize == True){ 
    }
    
    WindowAtom = XInternAtom(window->display,"_NET_WM_WINDOW_TYPE",False);
    
    if(strcmp("normal",windowtype)==0){
    
        winval = XInternAtom(window->display,
                "_NET_WM_WINDOW_TYPE_NORMAL",
                False);

    }else if(strcmp("dialog",windowtype)==0){

        winval = XInternAtom(window->display,
                "_NET_WM_WINDOW_TYPE_DIALOG",
                False);
    
    }else if(strcmp("dock",windowtype)==0){

        winval = XInternAtom(window->display,
                "_NET_WM_WINDOW_TYPE_DOCK",
                False);
        fprintf(stderr,
                "mode dock\n");

    }else{

        winval = XInternAtom(window->display,
                "_NET_WM_WINDOW_TYPE_NORMAL",
                False);

        fprintf(stderr,
                "window type not specified (now type set as 'normal' type)\n");
    }
    XChangeProperty(window->display,window->window,WindowAtom,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char *)&winval,
            1);
    
    window->delWindow = XInternAtom(window->display, 
            "WM_DELETE_WINDOW",
            False);
    
    XSetWMProtocols(window->display, window->window,&window->delWindow,1);
}

void initWindow(window_t *window_main,unsigned int palsize)
{  

    Display *display; 
    Window window;
    GC *gc;
    int screen;
    display = XOpenDisplay(NULL);
    
    if(!display){
        fprintf(stderr, "DisplayOpenError\n");
        exit(EXIT_FAILURE);
    }

    screen = DefaultScreen(display);

    window = XCreateSimpleWindow(
        display, 
        RootWindow(display,  screen),
        200, 
        200, 
        window_main->size_x, 
        window_main->size_y, 
        0, 
        BlackPixel(display, screen),
        WhitePixel(display, screen)
        //BlackPixel(display, screen)
    ); 
    

    gc = (GC *)malloc(sizeof(GC)*palsize);
    for(int i=0;i<palsize;i++){
        gc[i] = XCreateGC(display,window,0,NULL);
    }

    window_main->display = display;
    window_main->window = window;
    window_main->screen = screen;
    window_main->gc = gc;

    WindowSetting(window_main);


    XSelectInput(display, window, 
            ExposureMask | 
            KeyPressMask | 
            KeyReleaseMask | 
            ButtonPressMask |
            ButtonReleaseMask |
            ResizeRedirectMask |
            LeaveWindowMask |
            EnterWindowMask |
            FocusChangeMask |
            PointerMotionMask 

            );
    
    XMapWindow(display, window);
    initKeyboard(window_main);
    
    window_main->display = display;
    window_main->window = window;
    window_main->screen = screen;
    window_main->gc = gc;
}

void DestWindow(window_t *window)
{ 
    destKeyboard(window);
    XDestroyWindow(window->display, window->window);
    XCloseDisplay(window->display);
}

void keyControl(char *keymap, sprite_t *sprite)
{
    int up, down,left,right;
    int x, y;
    int xlim, ylim;
    int speed_x, speed_y;
    
    x = sprite->x;
    y = sprite->y;
    xlim = sprite->xlimit;
    ylim = sprite->ylimit;

    speed_x = sprite->speed_x;
    speed_y = sprite->speed_y;

    up = keymap[UP_INDX];
    down = keymap[DOWN_INDX];
    left = keymap[LEFT_INDX];
    right = keymap[RIGHT_INDX];
   
    if( (left == LEFT   ||  left == LEFT+DOWN)
        &&
        x > 0 ){

        sprite->x -= speed_x;
    }
    if( up == UP
        &&
        y > 0 ){
        
        sprite->y -= speed_y;
    }
    if( (right == RIGHT  ||  right == RIGHT+DOWN)
        &&
        x < xlim){

        sprite->x += speed_x;
    }
    if( (down == DOWN   ||  down == DOWN+LEFT   || down == DOWN+RIGHT)
        &&
        y < ylim){

        sprite->y += speed_y;
    }
}

void DrawPoint(window_t window, int pal, int x, int y)
{
    XDrawPoint(window.display, 
            window.window,
            (GC)window.gc[pal],
            x,
            y);
}

void DrawLine(window_t window,  int pal,int x1, int y1, int x2, int y2)
{
    XDrawLine(window.display, 
            window.window,
            (GC)window.gc[pal],
            x1,
            y1,
            x2,
            y2);
}

void DrawLinear(window_t window, int pal,int x, int y)
{
    int fx;
    
    for(int i=0;i<1000;i++){
        fx = (x/y)*i;
        XDrawLine(window.display, window.window, (GC)window.gc[pal],i,fx,i+1,fx+1);
    }
}

void DrawBox(window_t window, int pal,int x,int y,unsigned int len,unsigned int wid)
{
    XFillRectangle(window.display,
            window.window, 
            (GC)window.gc[pal], 
            x, 
            y, 
            len,
            wid);
}

void DrawString(window_t window , int pal,int x ,int y ,const char *format, ...)
{
    va_list ap;
    char *buff;
    buff = (char *)malloc(sizeof(char)+1*strlen(format)+1);
    va_start(ap, format);
    vsprintf(buff, format,ap);
    
    XDrawString(window.display, window.window,(GC)window.gc[pal],x,y,buff,strlen(buff));
    va_end(ap);
    if(buff!=NULL){         
        free(buff);
    }    
}

void DrawGrid(window_t window, int pal, int wx, int wy)
{

    for(int x = 0;x < window.size_x; x++){
        if(x % wx == 0){
            XDrawLine(window.display, window.window, (GC)window.gc[pal], x, 0, x, window.size_y);
        }
    }
    
    for(int y = 0;y < window.size_y; y++){
        if(y % wy == 0){    
            XDrawLine(window.display, window.window, (GC)window.gc[pal], 0, y, window.size_x, y);
        }
    }
}

void SetCoordPlane(window_t window,int pal,int zoom)
{ 
    int grid,width,height;
    grid = 1;
    width = grid*zoom;
    height = grid*zoom;
    DrawGrid(window, pal,width,height);
}

void DrawGraph(window_t window,int pal)
{
    #define testf(x) (sin((x)*0.1)*100)+400
    for(int x=0;x<window.size_x;x++){
        DrawLine(window,pal,
                
                x,testf(x),
                
                x+1,testf(x+1)
                
                );

    }
}

double udifftime(clock_t start, clock_t end)
{
    double diff;
    diff = -1*(double)(start - end)/CLOCKS_PER_SEC;
    return diff;
}
void SetupGC(window_t *window)
{
    XSetForeground(window->display, window->gc[0],0x000000);
    XSetForeground(window->display, window->gc[1],0xffffff);
    XSetForeground(window->display, window->gc[2],0x0000ff);
    XSetForeground(window->display, window->gc[3],0x00ff00);
    XSetForeground(window->display, window->gc[4],0xff0000);
    XSetForeground(window->display, window->gc[5],0xff00ff);
    XSetForeground(window->display, window->gc[6],0xffff00);
    XSetForeground(window->display, window->gc[7],0x00ffff);
}

void ObjectBounce(sprite_t *sprite)
{
    if(sprite->x >= (sprite->xlimit - sprite->size_x) || 
            sprite->x <= 0){
        
        sprite->vx*=-1;
    
    }
    if(sprite->y >= (sprite->ylimit - sprite->size_y) || 
            sprite->y <= 0){
        
        sprite->vy*=-1;
    }
    sprite->x += sprite->vx;
    sprite->y += sprite->vy;
}

char *testmask(int r)
{
    int i,j;
    int r2 = r*2;
    int rd = r*r;
    int n = 8;
    int w = (r2 + (n-1))/n;
    char *ret = (char*)calloc(1,r2*w);
    for(i=0;i<r2;i++){

        int m = i*w;
        int l1 = (i-r);
        l1 *= l1;
        
        for(j=0;j<r2;j++){
        
            int l2 = (j-r);
            l2 *= l2;
        
            if(l1 + l2 <= rd){
                ret[m + j/n] |= 1<<(j%n);
            }
        }
    }
    return ret;
}

void EventProcess(window_t window)
{
    char *keymap;
    char *mask;
    int count, frame;
    //clock_t start, end;
    //double diff;
    XKeyboardState ks;
    Pixmap pm;
    keymap = (char *)malloc(sizeof(char)*32);
    frame = 100;
    count = 0;
    int size = window.size_x;
    SetupGC(&window);
    sprite_t ball;
    ball.size_x = window.size_x;
    ball.size_y = window.size_y;    
    ball.xlimit = DisplayWidth(window.display,window.screen);
    ball.ylimit = DisplayHeight(window.display,window.screen);
    ball.x = ball.size_x;
    ball.y = ball.size_y;
    ball.vx = 2;
    ball.vy = 2;
    mask = testmask(size/2);
    pm = XCreateBitmapFromData(window.display,RootWindow(window.display,window.screen),mask,size,size);
    free(mask);
    XShapeCombineMask(window.display,window.window,ShapeBounding,0,0,pm,ShapeSet);

    while(1){

        //start = clock();
        usleep(1);
        if(count%frame==0){
            /*
            end = clock();
            diff = udifftime(start, end);
            */
            ObjectBounce(&ball);

            XMoveWindow(window.display,window.window,ball.x,ball.y);

            count = 0;
        }
        //printf("\e[H\e[3j");
        printf("\rx = %d, y = %d : vx = %d, vy = %d",ball.x,ball.y,ball.vx,ball.vy);

        count++;
        while(XPending(window.display)){
            
            XNextEvent( window.display,  &window.event );
            XGetKeyboardControl( window.display,  &ks );
            XQueryKeymap( window.display,  keymap );

            if(XFilterEvent( &window.event,  None )){
                printf("null\n");
                continue;
            }

            switch(window.event.type){
                
                case FocusIn:
                    
                    initKeyboard(&window);
                    break;

                case FocusOut:
                    
                    destKeyboard(&window);
                    break;

                case ClientMessage:
                    
                    printf("program end\n");
                    DestWindow(&window);
                    exit(EXIT_SUCCESS);
                    break;

                case KeyPress:

 
                    //printf("%x\n", window.event.xkey.keycode); 
                    //printf("%d\n",magnif);


                    break;

                case Expose:

                    break;
            }
        }


        XFlush(window.display);

    }
    DestWindow(&window);
    free(keymap);
}

int main()
{ 
    int size = 100;

    //char mode[] = "normal";
    char mode[] = "dock";
    Pixmap pm;

    window_t window;

    window.mode = mode;    
    window.resize = True;
    window.size_x = 1*size;
    window.size_y = 1*size;

    initWindow(&window,10);

    destKeyboard(&window);
    EventProcess(window);

    DestWindow(&window);
}
