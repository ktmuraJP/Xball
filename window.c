#include<myxwin.h>

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
        size_hint->min_width = window->width;
        size_hint->max_width = window->width;
        size_hint->min_height = window->height;
        size_hint->max_height = window->height;
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

void initWindow(window_t *window_main,unsigned int palsize) //unsigned int layer)
{  

    Display *display; 
    Window window;
    XWindowAttributes wa;
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
        window_main->width, 
        window_main->height, 
        0, 
        BlackPixel(display, screen),
        //WhitePixel(display, screen),
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
    
    XGetWindowAttributes(window_main->display,window_main->window,&wa);
    

    window_main->buffer = XCreatePixmap(window_main->display,
            window_main->window,wa.width,wa.height,wa.depth);
    
    window_main->screen = screen;
    window_main->gc = gc;
}

void DestWindow(window_t *window)
{ 
    destKeyboard(window);
    XDestroyWindow(window->display, window->window);
    XCloseDisplay(window->display);
}

void SetupGC(window_t *window)
{
    XSetForeground(window->display, window->gc[1],0x000000);
    XSetForeground(window->display, window->gc[2],0xffffff);
    XSetForeground(window->display, window->gc[3],0x0000ff);
    XSetForeground(window->display, window->gc[4],0x00ff00);
    XSetForeground(window->display, window->gc[5],0xff0000);
    XSetForeground(window->display, window->gc[6],0xff00ff);
    XSetForeground(window->display, window->gc[7],0xffff00);
    XSetForeground(window->display, window->gc[8],0x00ffff);
}

double udifftime(clock_t start, clock_t end)
{
    double diff;
    diff = -1*(double)(start - end)/CLOCKS_PER_SEC;
    return diff;
}

void RefreshWindow(window_t window)
{
    XCopyArea(window.display,
            window.buffer,
            window.window,
            window.gc[0],
            0,0,
            window.width,window.height,
            0,0);
}

void DrawPoint(window_t window, 
        unsigned int pal, 
        int x, int y)
{
    XDrawPoint(window.display, 
            window.buffer,
            (GC)window.gc[pal],
            x,
            y);
}

void DrawLine(window_t window,  
        unsigned int pal,
        unsigned int width,
        int x1, int y1, 
        int x2, int y2)
{
    for(int i = 0; i < width; i++){
        
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                x1,
                y1+i,
                x2,
                y2+i);
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                x1+i,
                y1,
                x2+i,
                y2);
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                (x1+width),
                y1+i,
                (x2+width),
                y2+i); 
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                x1+i,
                y1+width,
                x2+i,
                y2+width); 
    }
    XFillRectangle(window.display,
            window.buffer, 
            (GC)window.gc[pal], 
            x1, 
            y1, 
            width,
            width);
    XFillRectangle(window.display,
            window.buffer, 
            (GC)window.gc[pal], 
            x2, 
            y2, 
            width,
            width);
}

void DrawSkeltonLine(window_t window,  
        unsigned int pal,
        unsigned int width,
        int x1, int y1, 
        int x2, int y2)
{
    for(int i = 0; i < width; i++){
        
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                x1,
                y1,
                x2,
                y2);
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                x1+width,
                y1+width,
                x2+width,
                y2+width);
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                (x1+width),
                y1,
                (x2+width),
                y2); 
        XDrawLine(window.display, 
                window.buffer,
                (GC)window.gc[pal],
                x1,
                y1+width,
                x2,
                y2+width); 
    }
    XDrawRectangle(window.display,
            window.buffer, 
            (GC)window.gc[pal], 
            x1, 
            y1, 
            width,
            width);
    XDrawRectangle(window.display,
            window.buffer, 
            (GC)window.gc[pal], 
            x2, 
            y2, 
            width,
            width);
}


void DrawString(window_t window , 
        unsigned int pal,
        int x ,int y ,
        const char *format, ...)
{
    va_list ap;
    char *buff;

    buff = (char *)malloc(sizeof(char)+1*strlen(format)+1);
    va_start(ap, format);
    vsprintf(buff, format,ap);
    
    XDrawString(window.display, 
            window.buffer,
            window.gc[pal],
            x,y,
            buff,
            strlen(buff));
    
    va_end(ap);
    if(buff!=NULL){         
        free(buff);
    }    
}
//gdb bt
void DrawText(window_t window, 
        unsigned int pal,
        int x, int y, 
        unsigned int fontsize,
        const char *fontname, 
        const char *format,...)
{
    va_list ap;
    char *buff,*fontstr;
    int ref;
    Font font;
    XTextItem Items[1],Item;

    ref = 0;
    buff = (char *)malloc(sizeof(char)+strlen(format)+1);
    va_start(ap, format);
    vsprintf(buff, format,ap);
    fontstr = (char *)malloc(sizeof(char)*(strlen(fontname)+40+ref));
    sprintf(fontstr,"-*-%s-*-*-*-*-%d-*-*-*-*-*-*-*",fontname,fontsize);
    font = XLoadFont(window.display,fontstr); 
    
    Item.chars = buff;
    Item.nchars = strlen(buff);
    Item.font = font;
    Items[0] = Item;
    XDrawText(window.display,
            window.buffer,
            window.gc[pal],
            x,y,
            Items,1);
    
    if(fontstr!=NULL)
        free(fontstr);
    if(buff!=NULL)
        free(buff);  
    va_end(ap);
}
void DrawGraph(window_t window,
        unsigned int pal)
{
    for(int x=0;x<window.width;x++){

    }
}

void DrawLinear(window_t window, 
        unsigned int pal,
        float tanx, float tany,
        int offx, int offy)
{
}

void DrawBox(window_t window, 
        unsigned int pal,
        int x,int y,
        unsigned int len,
        unsigned int wid)
{
    XFillRectangle(window.display,
            window.buffer, 
            (GC)window.gc[pal], 
            x, 
            y, 
            len,
            wid);
}

void FillBackground(window_t window,
        unsigned int pal)
{
    DrawBox(window, 
            pal, 
            0, 0, 
            window.width, 
            window.height);
}

void DrawGrid(window_t window, 
        unsigned int pal, 
        int wx, int wy)
{
    for(int x = 0;x < window.width; x++){
        if(x % wx == 0){
            XDrawLine(window.display, 
                    window.buffer, 
                    (GC)window.gc[pal], 
                    x, 0, 
                    x, window.height);
        }
    }
    
    for(int y = 0;y < window.height; y++){
        if(y % wy == 0){    
            XDrawLine(window.display, 
                    window.buffer, 
                    (GC)window.gc[pal], 
                    0, y, 
                    window.width, y);
        }
    }
}

void DrawCircle(window_t window,
        unsigned int pal,
        unsigned int rad,
        int x, int y)
{
    XFillArc(window.display,
            window.buffer,
            window.gc[pal],
            x,y,
            rad*2,rad*2,
            0,360*64);
    XDrawArc(window.display,
            window.buffer,
            window.gc[pal+1],
            x+rad/2,y+(rad/2),
            rad,rad,
            0,360*64);
}
