#ifndef AKEBONO_H
#define AKEBONO_H

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

#define DEFAULT_KEY_DELAY 660
#define DEFAULT_KEY_RATE 25

#define KEY_DELAY 1
#define KEY_RATE 10

#define UP 0xffffff80
#define UP_INDX 13
#define DOWN 0x10
#define DOWN_INDX 14
#define LEFT 0x2
#define LEFT_INDX 14
#define RIGHT 0x4
#define RIGHT_INDX 14

/*
    window.c
*/
typedef struct _window_data
{
    Display *display;
    Window window;
    Pixmap buffer;
    XEvent event;
    int screen;
    int width, height;
    char *mode;
    char *icondir;
    Bool resize;
    GC *gc; 
    Atom atom, delWindow;
}
window_t;

typedef struct _sprite_data
{
    float x, y;
    float xlimit, ylimit;
    float vx,vy;
    float width, height;
    float center;
    char *spr_name;
}sprite_t;


typedef struct _frame_data
{
    int frame;
    int current_frame;
    int refrate;
    int counter;
    double time;
}framedata_t;

typedef struct _space_param
{
    float gravity;
    float airfric;
    float time;
    float **vector;
    float vx,vy,vz;
    float scalar;
}physic_t;

typedef struct _arrow_
{
    unsigned int length;
    unsigned int width;
    unsigned int nvertex;
    sprite_t *vertex;
}arrow_t;

void initKeyboard(window_t *window);
void destKeyboard(window_t *window);
void WindowSetting(window_t *window);
void initWindow(window_t *window_main,unsigned int palsize);
void DestWindow(window_t *window);
void SetupGC(window_t *window);
double udifftime(clock_t start, clock_t end);

void RefreshWindow(window_t window);
void DrawPoint(window_t window, 
        unsigned int pal, 
        int x, int y);

void DrawLine(window_t window, 
        unsigned int pal,
        unsigned int width,
        int x1, int y1, 
        int x2, int y2);

void DrawSkeltonLine(window_t window, 
        unsigned int pal,
        unsigned int width,
        int x1, int y1, 
        int x2, int y2);

void DrawGraph(window_t window,unsigned int pal);

void DrawLinear(window_t window, 
        unsigned int pal,
        float tanx, float tany,
        int offx, int offy);

void DrawBox(window_t window, 
        unsigned int pal,
        int x,int y,
        unsigned int len,
        unsigned int wid);

void FillBackground(window_t window,unsigned int pal);

void DrawString(window_t window , 
        unsigned int pal,
        int x ,int y ,
        const char *format, ...);

void DrawText(window_t window, 
        unsigned int pal,
        int x, int y, 
        unsigned int fontsize,
        const char *fontname, 
        const char *format,...);

void DrawGrid(window_t window, 
        unsigned int pal, 
        int wx, int wy);

void DrawCircle(window_t window,
        unsigned int pal,
        unsigned int rad,
        int x,int y);

void SetupWierdBar(window_t window,
        arrow_t *arrow,
        unsigned int width,
        unsigned int vertex,
        float x, float y,
        float vx, float vy);

void SetupArrow(window_t window,
        arrow_t *arrow,
        unsigned int width,
        unsigned int length,
        float x, float y,
        float vx, float vy);

void DrawArrow(window_t window,
        arrow_t arrow,
        unsigned int pal);

void keyControl(char *keymap, sprite_t *sprite);
void ObjectBounce(sprite_t *sp);
void BounceArrow(window_t window,arrow_t *arrow);
#endif
