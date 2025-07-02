/* * * * * * * * * * * * * * * * * */
/* MIT License                     */
/* Copyright (c) 2024 ANSI-Christ  */
/* * * * * * * * * * * * * * * * * */

#ifndef SG_WINDOW_H
#define SG_WINDOW_H

#include <stddef.h>
#include "sgc.h"
#include "sgk.h"

enum SGE{
    SGE_NONE=0,
    SGE_CLOSE,
    SGE_ASYNC,     /* SGE.async */
    SGE_RECTANGLE, /* SGW.rectangle */
    SGE_PRESS,     /* SGE.key */
    SGE_RELEASE,   /* SGE.key */
    SGE_CURSOR,    /* SGW.cursor */
    SGE_SCROLL,    /* SGE.scroll */
};


typedef union{

    void *async;

    SGK key;

    enum{
        SGE_SCROLL_UP=-1,
        SGE_SCROLL_DOWN=1
    }scroll;

}SGE;


enum SGW{
    SGW_XY=0,
    SGW_WH,
    SGW_XYWH,
    SGW_MAX,
    SGW_TRAY,
    SGW_FULLSCREEN,
};

typedef struct _sgw{

    const char *title;

    SGC *pixel;

    void*(*allocator)(size_t);
    void(*deallocator)(void*);

    enum SGW mode;

    SGK keys;

    struct{
        int x,y;
        unsigned int w,h;
    }rectangle;

    struct{
        int x,y;
    }cursor;

    unsigned char bitness;

}const SGW;


SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*));

SGC *sgw_pixel(SGW *w,unsigned int x,unsigned int y);

void sgw_close(SGW *w);

void sgw_render(SGW *w);

void sgw_fill(SGW *w,SGC c);

void sgw_rect(SGW *w,enum SGW mode,...);

void sgw_async(SGW *w,const void *p);

void sgw_title(SGW *w,const char *title);

enum SGE sgw_event(SGW *w,int t,SGE *e);


#endif /* SG_WINDOW_H */
