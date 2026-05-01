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
    SGE_NONE = 0,
    SGE_CLOSE      = 1<<0,
    SGE_ASYNC      = 1<<1, /* SGE.async */
    SGE_RECTANGLE  = 1<<2, /* SGW.rectangle */
    SGE_PRESS      = 1<<3, /* SGE.key */
    SGE_RELEASE    = 1<<4, /* SGE.key */
    SGE_CURSOR     = 1<<5, /* SGW.cursor */
    SGE_SCROLL     = 1<<6, /* SGE.scroll */
};


typedef union{

    void *async;

    SGK key;

    enum{
        SGE_SCROLL_UP    = -1,
        SGE_SCROLL_DOWN  = 1
    }scroll;

}SGE;


enum SGW{

    SGW_MODES    = 0xFF,
        SGW_X    = 1<<0,
        SGW_Y    = 1<<1,
        SGW_W    = 1<<2,
        SGW_H    = 1<<3,
        SGW_XY   = SGW_X|SGW_Y,
        SGW_WH   = SGW_W|SGW_H,
        SGW_XYWH = SGW_XY|SGW_WH,

    SGW_STATES      = 0xFF<<8,
        SGW_FIXED   = 1<<9,
        SGW_MUTABLE = 1<<10
};

typedef struct _sgw{

    SGC *pixel;

    void*(*allocator)(size_t);
    void(*deallocator)(void*);

    SGK keys;

    struct{
        int x,y;
        unsigned int w,h;
        enum SGW flags;
    }rectangle;

    struct{
        int x,y;
        unsigned char visible;
    }cursor;

}const SGW;


SGW *sgw_open(void*(*allocator)(size_t),void(*deallocator)(void*));

SGC *sgw_pixel(SGW *w,unsigned int x,unsigned int y);

void sgw_close(SGW *w);

void sgw_render(SGW *w);

void sgw_fill(SGW *w,SGC c);

void sgw_async(SGW *w,const void *p);

void sgw_title(SGW *w,const char *title);

void sgw_cursor(SGW *w,unsigned char visible);

void sgw_rect(SGW *w,enum SGW,int x,int y,unsigned int width,unsigned int height);

enum SGE sgw_event(SGW *w,int t,SGE *e);


#endif /* SG_WINDOW_H */
